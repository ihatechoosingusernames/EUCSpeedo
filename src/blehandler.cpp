#include "blehandler.h"

#include <functional>
#include <cstring>

#include "utils.h"
#include "logging.h"

namespace euc {

BleHandler::BleHandler(std::function<void(EucType)> connection,
      std::function<void(uint8_t* data, size_t data_size)> notify) :
    connection_callback(connection), notify_callback(notify) {
  
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
}

BleHandler::~BleHandler() {
  shutting_down = true;
  NimBLEDevice::deinit(true);
  btStop();
}

void BleHandler::Scan(std::function<void(void)> scan_done_callback) {
  if (!scanning) {
    should_connect = false;
    scan_finished_callback = scan_done_callback;
    pthread_create(&scan_task, NULL, BleHandler::StartScan, (void*)this);
  }
}

void BleHandler::Update() {
  if (!connected && should_connect && connect_device.size()) {
    if (Connect(connect_device.at(0)))
      should_connect = false;
    else
      connect_device.erase(connect_device.begin()); // Erase this device if it could not be connected to
  }
}

bool BleHandler::isConnected() {
  return connected;
}

bool BleHandler::isConnecting() {
  return should_connect;
}

void* BleHandler::StartScan(void* in) {
  BLEScan* pBLEScan = NimBLEDevice::getScan();
  
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks((BleHandler*)in));
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  ((BleHandler*)in)->scanning = true;
  pBLEScan->start(10, true);
  ((BleHandler*)in)->scanning = false;
  ((BleHandler*)in)->scan_finished_callback();
  vTaskDelete(NULL);
  return NULL;
}

bool BleHandler::Connect(NimBLEAdvertisedDevice* device) {
  LOG_DEBUG_ARGS("Forming a connection to %s %s", device->getName().c_str(), device->getAddress().toString().c_str());
  connected = true;
  BLEClient* pClient = nullptr;

  if(NimBLEDevice::getClientListSize()) {
    pClient = NimBLEDevice::getClientByPeerAddress(device->getAddress());

    if (pClient) {
      if (!pClient->connect(device)) {
        LOG_DEBUG("Failed to connect to server");
        connected = false;
        return false;
      }
      LOG_DEBUG("Reconnected to server");
    }
  }

  if (!pClient) {
    pClient = NimBLEDevice::createClient();
    LOG_DEBUG("Created client");

    pClient->setClientCallbacks(new ClientCallback(this));

    LOG_DEBUG("Connecting to server");

    // Connect to the remote BLE Server.
    if (!pClient->connect(device)) {
      LOG_DEBUG("Failed to connect to server");
      NimBLEDevice::deleteClient(pClient);
      connected = false;
      return false;
    }
    LOG_DEBUG("Connected to server");
  }

  auto services = pClient->getServices(true);
  LOG_DEBUG_ARGS("Client has %d services:", services->size());
  for (NimBLERemoteService* service : *services) {
    LOG_DEBUG_ARGS("\t%s", service->toString().c_str());
    service->getCharacteristics(true);
  }

  EucType type = CheckType(pClient);
  if (type == EucType::kLastType) { // No match
    connected = false;
    return false;
  }

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(NimBLEUUID(kServiceUuids[static_cast<size_t>(type)]));
  if (pRemoteService == nullptr) {
    LOG_DEBUG_ARGS("Failed to find the service UUID: %s", kServiceUuids[static_cast<size_t>(type)]);
    pClient->disconnect();
    connected = false;
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  NimBLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(NimBLEUUID(kReadCharacteristicUuids[static_cast<size_t>(type)]));
  if (pRemoteCharacteristic == nullptr) {
    LOG_DEBUG_ARGS("Failed to find our characteristic UUID: %s", kReadCharacteristicUuids[static_cast<size_t>(type)]);
    pClient->disconnect();
    connected = false;
    return false;
  }

  if(pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->subscribe(true, std::bind(&BleHandler::NotifyCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  connection_callback(type);

  return true;
}

void BleHandler::NotifyCallBack(NimBLERemoteCharacteristic* rc, uint8_t* data, size_t data_size, bool is_notify) {
  if (is_notify) {
    notify_callback(data, data_size);
  }
}

EucType BleHandler::CheckType(NimBLEClient* pClient) {
  std::vector<size_t> matching_types;
  size_t num_client_services = pClient->getServices()->size();

  // Find all the types that match the number of services the device offers. This will filter the list to one type in most cases.
  for (size_t type = 0; type < static_cast<size_t>(EucType::kLastType); type++) {
    if (num_client_services == kServiceList.at(type).size())
      matching_types.emplace_back(type);
  }

  // Check that the device services match the type services
  for (size_t index = 0; index < matching_types.size(); index++) {
    bool matches = true;
    for (const char* uuid : kServiceList.at(matching_types.at(index)))
      matches &= (pClient->getService(NimBLEUUID(uuid)) != nullptr);
    
    // Return the first EucType that matches all.
    if (matches)
      return static_cast<EucType>(matching_types.at(index));
  }

  return EucType::kLastType;  // Error value, no match found
}

BleHandler::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(BleHandler* super_ref) : super_reference(super_ref) {}

void BleHandler::AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice* device) {
  LOG_DEBUG_ARGS("BLE Advertised Device found: %s", device->toString().c_str());

  std::string advertised_name = device->getName();
  // Change name to upper case for matching
  std::transform(advertised_name.begin(), advertised_name.end(), advertised_name.begin(), [](char c){return std::toupper(c); });

  for (size_t type = 0; type < static_cast<size_t>(EucType::kLastType); type++) {
    if (device->isAdvertisingService(BLEUUID(kServiceUuids[type]))) {
      super_reference->connect_device.push_back(device);
      super_reference->should_connect = true;
      return; // Don't add the same device multiple times, as most EUCs have the same Service UUID
    }
  }
}

BleHandler::ClientCallback::ClientCallback(BleHandler* super_pointer) : super_reference(super_pointer) {}

void BleHandler::ClientCallback::onConnect(BLEClient* client) {
  LOG_DEBUG("Bluetooth Connected");
}

void BleHandler::ClientCallback::onDisconnect(BLEClient* client) {
  LOG_DEBUG("Bluetooth Disconnected");
  super_reference->connected = false;

  if (!super_reference->shutting_down) { // If signal is lost, scan to find it again
    super_reference->Scan();
  }
}

void BleHandler::ClientCallback::onAuthenticationComplete(ble_gap_conn_desc* desc) {
  LOG_DEBUG("onAuthenticationComplete()");
  if(!desc->sec_state.encrypted) {
      LOG_DEBUG("Encrypt connection failed - disconnecting");
      /** Find the client with the connection handle provided in desc */
      NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
      return;
  }
}

}