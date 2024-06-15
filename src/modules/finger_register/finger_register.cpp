#include "finger_register.h"

int finger_reg::progress = 0;
int finger_reg::step = 1;
int finger_reg::ok;
bool finger_reg::isAdding;
bool finger_reg::toSend = false;
void sendReg(int step) {
    auto resultReg = finger::CMD_reg(step);
    while (!resultReg.ok() && resultReg.error == 4) {
        delay(200);
        resultReg = finger::CMD_reg(step);
    }
}
void finger_reg::loop() {
    if (toSend) {
        auto resultSend = finger::CMD_reg(step);
        if (resultSend.ok()) {
            toSend = false;
            finger::CMD_setLed(finger::LED_MODE_PWM, finger::LED_COLOR_BLUE, 100, 50, 40);
        } else if (resultSend.error != 4) {
            toSend = false;
            ok = resultSend.error;
            isAdding = false;
            finger::CMD_setLed(finger::LED_MODE_OFF, finger::LED_COLOR_RED, 100, 50, 40);
        } else {
            delay(200);
        }
        return;
    }
    auto resultReg = finger::CMD_getRegResult();
    if (resultReg.ok()) {
        finger::CMD_setLed(finger::LED_MODE_ON, finger::LED_COLOR_GREEN, 100, 50, 40);
        if (resultReg.proc > progress) {
            if (resultReg.proc != 100) {
                progress = resultReg.proc;
                step++;
                toSend = true;
            } else {
                progress = 99;
                auto sendSave = finger::CMD_save(resultReg.id);
                while (!sendSave.ok() && sendSave.error == 4) {
                    delay(200);
                    sendSave = finger::CMD_save(resultReg.id);
                }
                auto resultSave = finger::CMD_getSaveResult();
                while (!resultSave.ok() && resultSave.error == 4) {
                    delay(200);
                    resultSave = finger::CMD_getSaveResult();
                }
                if (resultSave.ok()) {
                    ok = 0;
                } else {
                    ok = resultSave.error;
                }
                isAdding = false;
            }
        } else {
            toSend = true;
        }
    } else if (resultReg.error != 4) {
        finger::CMD_setLed(finger::LED_MODE_ON, finger::LED_COLOR_RED, 100, 50, 40);
        ok = resultReg.error;
        isAdding = false;
    } else {
        delay(200);
    }
}

void finger_reg::startAdd() {
    if (isAdding) {
        return;
    }
    isAdding = true;
    progress = 0;
    step = 1;
    toSend = false;
    sendReg(step);
    finger::CMD_setLed(finger::LED_MODE_PWM, finger::LED_COLOR_BLUE, 100, 50, 40);
}
