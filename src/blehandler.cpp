#include "blehandler.h"

#include <functional>
#include <algorithm>
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

void BleHandler::Scan(std::function<void(void)> scan_done_callback) {
  if (!scanning) {
    scan_finished_callback = scan_done_callback;
    pthread_create(&scan_task, NULL, BleHandler::StartScan, (void*)this);
    // Stack size of 1500 arrived at by trial and error, possible cause of stack overflow in future
    // xTaskCreate(BleHandler::Scan, "scan_task", 2000, (void*)this, 20, scan_task);
  }
}

void BleHandler::Update() {
  if (!scanning && !connected && should_connect && connect_device) {
    Connect(connect_device, brand);
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
  // vTaskDelete(NULL);
  return NULL;
}

bool BleHandler::Connect(NimBLEAdvertisedDevice* device, EucType type) {
  LOG_DEBUG_ARGS("Forming a connection to %s %s", device->getName().c_str(), device->getAddress().toString().c_str());
  connected = true;
  
  BLEClient*  pClient  = NimBLEDevice::createClient();
  LOG_DEBUG("Created client");

  pClient->setClientCallbacks(new ClientCallback());

  LOG_DEBUG("Connecting to server");

  // Connect to the remote BLE Server.
  if (!pClient->connect(device)) {
    LOG_DEBUG("Failed to connect to server");
    return false;
  }
  LOG_DEBUG("Connected to server");

  auto services = pClient->getServices(true);
  LOG_DEBUG_ARGS("Client has %d services", services->size());
  for (NimBLERemoteService* service : *services) {
    LOG_DEBUG_ARGS("\t%s", service->toString().c_str());
    service->getCharacteristics(true);
  }

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(NimBLEUUID(kServiceUuids[static_cast<size_t>(type)]));
  if (pRemoteService == nullptr) {
    LOG_DEBUG_ARGS("Failed to find the service UUID: %s", kServiceUuids[static_cast<size_t>(type)]);
    pClient->disconnect();
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  NimBLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(NimBLEUUID(kReadCharacteristicUuids[static_cast<size_t>(type)]));
  if (pRemoteCharacteristic == nullptr) {
    LOG_DEBUG_ARGS("Failed to find our characteristic UUID: %s", kReadCharacteristicUuids[static_cast<size_t>(type)]);
    pClient->disconnect();
    return false;
  }

  // Read the value of the characteristic.
  if(pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    LOG_DEBUG_ARGS("The characteristic value was: %s", value.c_str());
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

BleHandler::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(BleHandler* super_ref) : super_reference(super_ref) {}

void BleHandler::AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice* device) {
  LOG_DEBUG_ARGS("BLE Advertised Device found: %s", device->toString().c_str());

  // This is the naming method of identification. Quick and dirty, but works for now.
  if (!device->haveName())
    return;

  std::string advertised_name = device->getName();
  // Change name to upper case for matching
  std::transform(advertised_name.begin(), advertised_name.end(), advertised_name.begin(), [](char c){return std::toupper(c); });

  for (size_t type = 0; type < static_cast<size_t>(EucType::kLastType); type++) {
    if (device->isAdvertisingService(BLEUUID(kServiceUuids[type]))) {
      if (advertised_name.find(kBrandName[type]) != std::string::npos) {
        super_reference->brand = static_cast<EucType>(type);
        super_reference->connect_device = device;
        super_reference->should_connect = true;
        return;
      }
    }
  }
}

void BleHandler::ClientCallback::onConnect(BLEClient* client) {
  LOG_DEBUG("Bluetooth Connected");
}

void BleHandler::ClientCallback::onDisconnect(BLEClient* client) {
  LOG_DEBUG("Bluetooth Disconnected");
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