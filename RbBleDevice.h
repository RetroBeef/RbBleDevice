#pragma once
#include <Arduino.h>
#include "BLEDevice.h"
#include <functional>

class RbBleDevice {
  private:
  protected:
    BLEClient* bleClient;

    BLERemoteCharacteristic* writeCharacteristic = 0;
    BLERemoteCharacteristic* notificationCharacteristic = 0;
    BLERemoteService* service = 0;

    String deviceMac;

    std::function<void(BLERemoteCharacteristic*, uint8_t*, size_t, bool)> notificationCallback;
  public:
    RbBleDevice(BLEClient* bleClient, const String& deviceMac, const std::function<void(BLERemoteCharacteristic*, uint8_t*, size_t, bool)>& notificationCallback) : bleClient(bleClient), deviceMac(deviceMac), notificationCallback(notificationCallback) {
      
    }
    virtual ~RbBleDevice() {}
    int8_t connectToDevice();
    bool isConnected() {
      return bleClient->isConnected();
    }

    void write(const String& jsonCommand);
};
