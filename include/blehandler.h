#ifndef BLEHANDLER_H_
#define BLEHANDLER_H_

#include <functional>
#include <pthread.h>

#include <Arduino.h>
#include <NimBLEDevice.h>

#include "constants.h"

namespace euc {

class BleHandler {
  public:
    BleHandler(std::function<void(EucType)> connection, std::function<void(uint8_t* data, size_t data_size)> notify);

    void Scan(std::function<void(void)> scan_done_callback = [](){});
    void Update();
    bool isConnected();

  private:
    static void Scan(void* in);

    bool Connect(NimBLEAdvertisedDevice* device, EucType type);
    void NotifyCallBack(NimBLERemoteCharacteristic* rc, uint8_t* data, size_t data_size, bool is_notify);

    class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
      public:
        AdvertisedDeviceCallbacks(BleHandler* super_ref);

        void onResult(NimBLEAdvertisedDevice* device) override;

      private:
        BleHandler* super_reference;
    };

    class ClientCallback : public NimBLEClientCallbacks {
      void onConnect(BLEClient* client) override;
      void onDisconnect(BLEClient* client) override;
      void onAuthenticationComplete(ble_gap_conn_desc* desc) override;
    };

    std::function<void(EucType)> connection_callback;
    std::function<void(uint8_t* data, size_t data_size)> notify_callback;
    std::function<void(void)> scan_finished_callback;

    bool connected = false, scanning = false;
    TaskHandle_t *scan_task;

    volatile bool should_connect;
    EucType brand;
    NimBLEAdvertisedDevice* connect_device;
};

}

#endif