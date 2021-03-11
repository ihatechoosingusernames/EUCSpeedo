#include "blehandler.h"

#include <functional>
#include <memory>

namespace euc {

BleHandler::BleHandler(std::function<void(EucType)> connection,
      std::function<void(uint8_t* data, size_t data_size)> notify) :
    connection_callback(connection), notify_callback(notify) {
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(this));
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  // pBLEScan->start(5, false); // For testing
}

bool BleHandler::isConnected() {
  return connected;
}

bool BleHandler::Connect(BLEAdvertisedDevice* device, EucType type) {
  Serial.printf("Forming a connection to %s ", kBrandName[(size_t)type]);
  Serial.println(device->getAddress().toString().c_str());
  
  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new ClientCallback());

  // Connect to the remote BLE Server.
  pClient->connect(device);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(kServiceUuids[(size_t)type]);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(kServiceUuids[(size_t)type]);
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(kReadUuids[(size_t)type]);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(kReadUuids[(size_t)type]);
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if(pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if(pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(std::bind(&BleHandler::NotifyCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  connected = true;
  connection_callback(type);

  return true;
}

void BleHandler::NotifyCallBack(BLERemoteCharacteristic* rc, uint8_t* data, size_t data_size, bool is_notify) {
  if (is_notify) {
    notify_callback(data, data_size);
  }
}

BleHandler::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(BleHandler* super_ref) : super_reference(super_ref) {}

void BleHandler::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice device) {
  Serial.print("BLE Advertised Device found: ");
  Serial.println(device.toString().c_str());

  for (size_t type = 0; type < kNumEucTypes; type++) {
    if (device.haveServiceUUID() && device.isAdvertisingService(BLEUUID(kServiceUuids[type]))) {
      // Found an EUC, now stopping scan and connecting
      BLEDevice::getScan()->stop();
      super_reference->Connect(&device, (EucType)type);
    }
  }
}

void BleHandler::ClientCallback::onConnect(BLEClient* client) {
  printf("Bluetooth Connected\n");
}

void BleHandler::ClientCallback::onDisconnect(BLEClient* client) {
  printf("Bluetooth Disconnected\n");
}

}