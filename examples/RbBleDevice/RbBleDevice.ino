#include <BLEDevice.h>
#include <ArduinoJson.h>
#include <RbBleDevice.h>

StaticJsonDocument<64> doc;
BLEClient* bleClient = 0;
BLEClient* bleClient2 = 0;
RbBleDevice* tws10PanTilt = 0;
RbBleDevice* plantScale = 0;

void setup() {
  Serial.begin(115200);

  BLEDevice::init("");
  bleClient = BLEDevice::createClient();
  bleClient2 = BLEDevice::createClient();

  tws10PanTilt = new RbBleDevice(bleClient, "70:04:1d:93:06:46", [](BLERemoteCharacteristic * pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
    DeserializationError error = deserializeJson(doc, pData, length);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    uint16_t pan = doc["pan"];
    uint16_t tilt = doc["tilt"];
    uint32_t dist = doc["dist"];
    Serial.printf("pan(%u), tilt(%u), dist(%u)\r\n", pan, tilt, dist);
  });

  plantScale = new RbBleDevice(bleClient2, "24:6f:28:d1:c8:aa", [](BLERemoteCharacteristic * pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
    DeserializationError error = deserializeJson(doc, pData, length);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    float weight = doc["weight"];
    Serial.printf("weight(%f)\r\n", weight);
  });

  if (tws10PanTilt->connectToDevice() != 0) {
    Serial.println("could not connect to TWS10PanTilt");
  } else {
    Serial.println("connected to TWS10PanTilt");
  }

  if (plantScale->connectToDevice() != 0) {
    Serial.println("could not connect to PlantScale");
  } else {
    Serial.println("connected to PlantScale");
  }
}

StaticJsonDocument<48> cmdJson;
String cmd;
void loop() {
  if (Serial.available() && tws10PanTilt->isConnected() && plantScale->isConnected()) {
    String input = Serial.readString();
    int pos = input.indexOf("pan");
    if (pos > - 1) {
      String valStr = input.substring(pos + 3);
      uint8_t val = valStr.toInt();
      Serial.printf("setting pan to %u. expected analog val(%u)\r\n", val, map(val, 0, 180, 790, 3320));
      cmdJson["cmd"] = "pan";
      cmdJson["val"] = val;
      serializeJson(cmdJson, cmd);
      Serial.printf("sending %s\r\n", cmd.c_str());
      tws10PanTilt->write(cmd);
    } else {
      pos = input.indexOf("tilt");
      if (pos > -1) {
        String valStr = input.substring(pos + 4);
        uint8_t val = valStr.toInt();
        Serial.printf("setting tilt to %u. expected analog val(%u)\r\n", val, map(val, 0, 180, 790, 3320));
        cmdJson["cmd"] = "tilt";
        cmdJson["val"] = val;
        serializeJson(cmdJson, cmd);
        Serial.printf("sending %s\r\n", cmd.c_str());
        tws10PanTilt->write(cmd);
      } else {
        pos = input.indexOf("dist");
        if (pos > -1) {
          cmdJson["cmd"] = "dist";
          serializeJson(cmdJson, cmd);
          Serial.printf("sending %s\r\n", cmd.c_str());
          tws10PanTilt->write(cmd);
        } else {
          pos = input.indexOf("weight");
          if (pos > -1) {
            cmdJson["cmd"] = "weight";
            serializeJson(cmdJson, cmd);
            Serial.printf("sending %s\r\n", cmd.c_str());
            plantScale->write(cmd);
          } else {
            pos = input.indexOf("tare");
            if (pos > -1) {
              cmdJson["cmd"] = "tare";
              serializeJson(cmdJson, cmd);
              Serial.printf("sending %s\r\n", cmd.c_str());
              plantScale->write(cmd);
            } else {
              pos = input.indexOf("calib");
              if (pos > -1) {
                cmdJson["cmd"] = "calib";
                serializeJson(cmdJson, cmd);
                Serial.printf("sending %s\r\n", cmd.c_str());
                plantScale->write(cmd);
              }
            }
          }
        }
      }
    }
  }
  if (!tws10PanTilt->isConnected()) {
    if (tws10PanTilt->connectToDevice() != 0) {
      Serial.println("could not connect to TWS10PanTilt");
    } else {
      Serial.println("connected to TWS10PanTilt");
    }
  }
  if (!plantScale->isConnected()) {
    if (plantScale->connectToDevice() != 0) {
      Serial.println("could not connect to PlantScale");
    } else {
      Serial.println("connected to PlantScale");
    }
  }
}
