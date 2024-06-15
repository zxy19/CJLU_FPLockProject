#include "./src/api/def.h"
#include "./src/api/main.h"
#include "./src/modules/door/door.h"
#include "./src/modules/finger/finger.h"
#include "./src/modules/finger_register/finger_register.h"
#include "./src/modules/id/id.h"
#include "./src/modules/network/network.h"
#include "./src/modules/network/networkConfigHandler.h"
#include "./src/modules/preferences/preferences.h"
#include "./src/modules/values/values.h"
#include <DNSServer.h>
#include <WiFi.h>

const uint8_t HEART_BEAT[] = {0x03, 0x03};
uint32_t count;
bool isConfigureNetwork = false;
int enterConfigSecondRest = 0, enterConfigHit = 0, enterConfigTotal = 0;
bool inScan = false;
int scanCoolDown = 0;

void api::openDoor() {
    scanCoolDown = 100;
    finger::CMD_setLed(finger::LED_MODE_ON, finger::LED_COLOR_GREEN, 0, 0, 0);
    door::open(100);
}
void setup() {
#ifdef DEBUG
    Serial.begin(115200);
#endif
    preferences::init();
    ID::init();
    finger::init();
    door::init();
    sleep(2);
    networkConfigHandler::init();
    network::init();
    enterConfigSecondRest = C_CONFIG_LONG_PRESS;
    finger::CMD_setLed(finger::LED_MODE_ON, finger::LED_COLOR_BLUE, 0, 0, 0);
}
void loop() {
    count++;
    // 判断是否长按进入配网页面的时间
    auto isFingerOn = finger::CMD_isFingerOn();
    if (enterConfigSecondRest > 0) {
        if (isFingerOn.result) {
            enterConfigHit++;
        }
        enterConfigTotal++;
        enterConfigSecondRest--;
        if (enterConfigSecondRest == 0) {
#ifdef DDEBUG
            enterConfigHit = enterConfigTotal;
            Serial.printf("Start debug mode\r\n");
#endif
            if (enterConfigHit > enterConfigTotal * 0.75) {
                finger::CMD_setLed(finger::LED_MODE_WINKLE, finger::LED_COLOR_BLUE, 20, 20, 100);
                networkConfigHandler::startConfigMode();
            } else {
                auto tt = finger::CMD_setLed(finger::LED_MODE_OFF, finger::LED_COLOR_BLUE, 0, 0, 0);
            }
        }
    }

    if (finger_reg::isAdding) {
        finger_reg::loop();
    }
    // 非网络配置模式
    else if (!networkConfigHandler::isConfigureMode) {
        // 非冷却，非识别中，检测到手指，发送识别，蓝色呼吸灯
        if (!inScan && scanCoolDown == 0 && isFingerOn.ok() && isFingerOn.result) {
            inScan = true;
            if (!finger::CMD_match().ok()) {
                finger::CMD_setLed(finger::LED_MODE_ON, finger::LED_COLOR_RED_BLUE, 100, 50, 50);
            } else {
                finger::CMD_setLed(finger::LED_MODE_PWM, finger::LED_COLOR_BLUE, 100, 50, 50);
            }
        }
        if (scanCoolDown > 0) {
            scanCoolDown--;
            if (scanCoolDown == 0) {
                finger::CMD_setLed(finger::LED_MODE_OFF, finger::LED_COLOR_BLUE, 0, 0, 0);
            }
        }
        // 正在识别，读取结果
        if (inScan) {
            auto matchRes = finger::CMD_getMatchResult();
            // 识别完成，
            if (matchRes.ok()) {
                if (matchRes.result) {
                    finger::CMD_setLed(finger::LED_MODE_ON, finger::LED_COLOR_GREEN, 100, 50, 50);
                    scanCoolDown = 100;
                    api::openDoor();
                } else {
                    finger::CMD_setLed(finger::LED_MODE_PWM, finger::LED_COLOR_RED, 100, 50, 50);
                    scanCoolDown = 20;
                }
                inScan = false;
            } else if (matchRes.error != 0x00000004) {
                finger::CMD_setLed(finger::LED_MODE_PWM, finger::LED_COLOR_RED, 100, 50, 50);
            }
        }
    }
    // 网络配置模式
    else {
        if (count % 20 == 0) {
            finger::CMD_setLed(finger::LED_MODE_WINKLE, finger::LED_COLOR_BLUE, 20, 20, 5);
        }
    }

    networkConfigHandler::loop();
    door::loop();
    network::loop();
    delay(20);
}