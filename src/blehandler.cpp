#include "blehandler.h"

#include <functional>
#include <pthread.h>
#include <algorithm>
#include <cstring>

#include "utils.h"

namespace euc {

BleHandler::BleHandler(std::function<void(EucType)> connection,
      std::function<void(uint8_t* data, size_t data_size)> notify) :
    connection_callback(connection), notify_callback(notify) {
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
}

void BleHandler::Scan() {
  if (!scanning) {
    scanning = true;
    // pthread_create(&scan_thread, NULL, BleHandler::Scan, (void*)this);
    BleHandler::Scan((void*)this);
  }
}

void BleHandler::Update() {
  if (!connected && should_connect) {
    Connect(connect_device, brand);
  }
}

bool BleHandler::isConnected() {
  return connected;
}

void* BleHandler::Scan(void* in) {
  BLEScan* pBLEScan = NimBLEDevice::getScan();
  
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks((BleHandler*)in));
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10, true);
  ((BleHandler*)in)->scanning = false;
}

bool BleHandler::Connect(NimBLEAdvertisedDevice* device, EucType type) {
  printf("Forming a connection to %s %s\n", device->getName().c_str(), device->getAddress().toString().c_str());
  connected = true;
  
  BLEClient*  pClient  = NimBLEDevice::createClient();
  printf("Created client\n");

  pClient->setClientCallbacks(new ClientCallback());

  printf("Connecting to server\n");

  // Connect to the remote BLE Server.
  if (!pClient->connect(device)) {
    printf("Failed to connect to server\n");
    return false;
  }
  printf("Connected to server\n");

  auto services = pClient->getServices(true);
  printf("Client has %d services\n", services->size());
  for (NimBLERemoteService* service : *services) {
    printf("\t%s\n", service->toString().c_str());
    service->getCharacteristics(true);
  }

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(NimBLEUUID(kServiceUuids[static_cast<size_t>(type)]));
  if (pRemoteService == nullptr) {
    printf("Failed to find the service UUID: %s\n", kServiceUuids[static_cast<size_t>(type)]);
    pClient->disconnect();
    return false;
  }

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  NimBLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(NimBLEUUID(kReadCharacteristicUuids[static_cast<size_t>(type)]));
  if (pRemoteCharacteristic == nullptr) {
    printf("Failed to find our characteristic UUID: %s\n", kReadCharacteristicUuids[static_cast<size_t>(type)]);
    pClient->disconnect();
    return false;
  }

  // Read the value of the characteristic.
  if(pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    printf("The characteristic value was: %s\n", value.c_str());
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
  printf("BLE Advertised Device found: %s\n", device->toString().c_str());

  for (size_t type = 0; type < static_cast<size_t>(EucType::kLastType); type++) {
    if (device->isAdvertisingService(BLEUUID(kServiceUuids[type]))) {
      // This is the naming method of identification. Quick and dirty, but works for now.
      if (!device->haveName())
        continue;

      std::string advertised_name = device->getName();
      // Change name to upper case for matching
      std::transform(advertised_name.begin(), advertised_name.end(), advertised_name.begin(), [](char c){return std::toupper(c); });

      if (advertised_name.find(kBrandName[type]) != std::string::npos) {
        super_reference->brand = static_cast<EucType>(type);
        super_reference->should_connect = true;
        super_reference->connect_device = device;
        return;
      }
    }
  }
}

void BleHandler::ClientCallback::onConnect(BLEClient* client) {
  printf("Bluetooth Connected\n");
}

void BleHandler::ClientCallback::onDisconnect(BLEClient* client) {
  printf("Bluetooth Disconnected\n");
}

void BleHandler::ClientCallback::onAuthenticationComplete(ble_gap_conn_desc* desc) {
  printf("onAuthenticationComplete()\n");
  if(!desc->sec_state.encrypted) {
      printf("Encrypt connection failed - disconnecting\n");
      /** Find the client with the connection handle provided in desc */
      NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
      return;
  }
}

}