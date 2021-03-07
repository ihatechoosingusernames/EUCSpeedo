#ifndef BLEHANDLER_H_
#define BLEHANDLER_H_

#include <functional>

#include <Arduino.h>
#include <BLEDevice.h>

#include "constants.h"

namespace euc {

class BleHandler {
  public:
    BleHandler(std::function<void(EucType)> connection, std::function<void(uint8_t* data, size_t data_size)> notify);

    bool isConnected();

  private:
    bool Connect(BLEAdvertisedDevice* device, EucType type);
    void NotifyCallBack(BLERemoteCharacteristic* rc, uint8_t* data, size_t data_size, bool is_notify);

    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
      public:
        AdvertisedDeviceCallbacks(BleHandler* super_ref);
      private:
        void onResult(BLEAdvertisedDevice device) override;

      BleHandler* super_reference;
    };

    class ClientCallback : public BLEClientCallbacks {
      void onConnect(BLEClient* client) override;
      void onDisconnect(BLEClient* client) override;
    };

    std::function<void(EucType)> connection_callback;
    std::function<void(uint8_t* data, size_t data_size)> notify_callback;

    bool connected = false;
};

}

#endif