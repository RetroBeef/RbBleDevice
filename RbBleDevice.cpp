#include "RbBleDevice.h"

namespace {
static BLEUUID serviceUUID("8a39df89-82c9-47d6-b16f-86d97d1ca78b");
static BLEUUID writeUUID("86dce9e0-818b-4183-ba5c-6395e30addfe");
static BLEUUID notifyUUID("86dce9e0-818b-4183-ba5c-6395e30addff");
}

int8_t RbBleDevice::connectToDevice() {
  if (!bleClient->connect(BLEAddress(deviceMac.c_str()))) {
    return -1;
  }

  bleClient->setMTU(214);

  service = bleClient->getService(serviceUUID);
  if (service == nullptr) {
    return -2;
  }

  writeCharacteristic = service->getCharacteristic(writeUUID);
  if (writeCharacteristic == nullptr) {
    return -3;
  }

  notificationCharacteristic = service->getCharacteristic(notifyUUID);
  if (notificationCharacteristic == nullptr) {
    return -4;
  }

  if (notificationCharacteristic->canNotify()) {
    notificationCharacteristic->registerForNotify(notificationCallback);
    return 0;
  } else {
    return -5;
  }
}

void RbBleDevice::write(const String& jsonCommand){
    writeCharacteristic->writeValue((uint8_t*)jsonCommand.c_str(), jsonCommand.length(), true);
}
