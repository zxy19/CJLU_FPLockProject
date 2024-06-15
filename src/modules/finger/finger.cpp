#include "finger.h"
const char HEAD[] = {0xF1, 0x1F, 0xE2, 0x2E, 0xB6, 0x6B, 0xA8, 0x8A};

uint8_t finger::buffer[100];
uint8_t finger::data[80];
uint32_t finger::lastError;
uint32_t finger::lastDataLen;
void finger::_write(char c) { FINGER_SERIAL.write(c); }
uint8_t finger::_protocol_get_checksum(const uint8_t *data, uint32_t length) {
    uint32_t i = 0;
    uint8_t sum = 0;
    for (i = 0; i < length; i++) {
        sum += data[i];
    }
    return (uint8_t)((~sum) + 1);
}

uint8_t finger::_protocol_get_checksum(uint32_t pass, uint32_t command, const uint8_t *data, uint32_t length) {
    uint32_t i = 0;
    uint8_t sum = 0;
    sum += (pass >> 24) & 0xff;
    sum += (pass >> 16) & 0xff;
    sum += (pass >> 8) & 0xff;
    sum += (pass) & 0xff;
    sum += (command >> 8) & 0xff;
    sum += command & 0xff;
    for (i = 0; i < length; i++) {
        sum += data[i];
    }
    return (uint8_t)((~sum) + 1);
}

void finger::_sendCommand(uint32_t command, const uint8_t *data, uint32_t length) {
    uint8_t tmp[10];
    // send head
    uint32_t len = length + 7;
    tmp[8] = (len >> 8) & 0xff;
    tmp[9] = len & 0xff;
    for (int i = 0; i < 8; i++) {
        _write(HEAD[i]);
        tmp[i] = HEAD[i];
    }
    _write(((len >> 8) & 0xff));
    _write((len & 0xff));
    _write(_protocol_get_checksum(tmp, 10));
    // send command
    // seg_pass
    _write(((PASS >> 24) & 0xff));
    _write(((PASS >> 16) & 0xff));
    _write((PASS >> 8) & 0xff);
    _write((PASS) & 0xff);
    // seg_command
    _write((command >> 8) & 0xff);
    _write(command & 0xff);
    //
    for (int i = 0; i < length; i++) {
        _write(data[i]);
    }

    _write(_protocol_get_checksum(PASS, command, data, length));
}

bool finger::_receive(uint32_t command) {
    uint32_t len = 0, dataLength = 0, totalLen = 0, errorCode = 0;
    bool headOk = false;
    for (int dead = 0; dead != 20; dead++) {
        while (FINGER_SERIAL.available()) {
            uint8_t c = FINGER_SERIAL.read();
            if (!headOk) {     // reading header
                if (len < 8) { // get head
                    if ((char)c == HEAD[len]) {
                        buffer[len] = c;
                        len++;
                        if (len == 8) {
                            totalLen = 0;
                        }
                    } else {
                        len = 0;
                    }
                } else if (len < 10) { // get len
                    buffer[len++] = c;
                    totalLen = (totalLen << 8) | c;
                } else if (len == 10) { // check sum
                    if (_protocol_get_checksum(buffer, len) == c) {
                        headOk = true;
                    } else {
                        headOk = false;
                    }
                    len = 0;
                }
            } else { // reading data
                buffer[len] = c;
                if (len < 4) {
                    if (c == ((PASS >> ((3 - len) * 8)) & 0xff)) {
                        len++;
                    } else
                        len = 0;
                } else if (len < 5) {
                    if (c == ((command >> 8) & 0xff)) {
                        len++;
                    } else
                        len = 0;
                } else if (len < 6) {
                    if (c == (command & 0xff)) {
                        len++;
                    } else
                        len = 0;
                } else if (len < 10) {
                    errorCode = (errorCode << 8) | c;
                    len++;
                } else if (len < totalLen - 1) {
                    data[dataLength++] = c;
                    len++;
                } else {
                    if (_protocol_get_checksum(buffer, len) == c) {
                        lastError = errorCode;
                        lastDataLen = len - 7;
                        return true;
                    } else {
                        len = 0;
                    }
                }
            }
        }
        delay(50);
    }
    lastError = 0xFFFFFF;
    return false;
}
void finger::init() {
    FINGER_SERIAL.begin(57600);
    pinMode(FINGER_INT, INPUT);
    delay(1000);
}
bool finger::isFingerInt() { return digitalRead(FINGER_INT) != 0; }

finger::result_basic finger::CMD_heatBeat() {
    _sendCommand(CODE_CMD_HEART_BEAT, NULL, 0);
    if (_receive(CODE_CMD_HEART_BEAT)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}
finger::result_bool finger::CMD_isFingerOn() {
    _sendCommand(CODE_CMD_IS_FINGER, NULL, 0);
    if (_receive(CODE_CMD_IS_FINGER)) {
        return result_bool(lastError, data[0] != 0);
    } else {
        return result_bool(0xFFFFFF, false);
    }
}
finger::result_basic finger::CMD_reg(uint8_t idx) {
    uint8_t tmp[1] = {idx};
    _sendCommand(CODE_CMD_REG, tmp, 1);
    if (_receive(CODE_CMD_REG)) {
        return result_basic(lastError);
    }
    return result_basic(0xFFFFFF);
}
finger::result_reg finger::CMD_getRegResult() {
    _sendCommand(CODE_CMD_REG_RESULT, NULL, 0);
    if (_receive(CODE_CMD_REG_RESULT)) {
        uint32_t id = data[0];
        id = (id << 8) | data[1];
        return result_reg(lastError, id, data[2]);
    } else {
        return result_reg(0xFFFFFF, 0, 0);
    }
}
finger::result_basic finger::CMD_cancel() {
    _sendCommand(CODE_CMD_CANCEL, NULL, 0);
    if (_receive(CODE_CMD_CANCEL)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}
finger::result_basic finger::CMD_save(uint32_t id) {
    uint8_t tmp[2] = {(id >> 8) & 0xff, id & 0xff};
    _sendCommand(CODE_CMD_SAVE, tmp, 2);
    if (_receive(CODE_CMD_SAVE)) {
        return result_basic(lastError);
    }
    return result_basic(0xFFFFFF);
}
finger::result_id finger::CMD_getSaveResult() {
    _sendCommand(CODE_CMD_SAVE_RESULT, NULL, 0);
    if (_receive(CODE_CMD_SAVE_RESULT)) {
        uint32_t id = data[0];
        id = (id << 8) | data[1];
        return result_id(lastError, id);
    } else {
        return result_id(0xFFFFFF, 0);
    }
}
finger::result_basic finger::CMD_setLed(uint8_t mode, uint8_t color, uint8_t a1, uint8_t a2, uint8_t a3) {
    uint8_t tmp[5] = {mode, color, a1, a2, a3};
    _sendCommand(CODE_CMD_LED, tmp, 5);
    if (_receive(CODE_CMD_LED)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}

void finger::CMD_autoReg(uint8_t waitFinger, uint8_t pressTimes, uint32_t id) {
    uint8_t tmp[4] = {waitFinger, pressTimes, (id >> 8) & 0xff, id & 0xff};
    _sendCommand(CODE_CMD_AUTO_REG, tmp, 4);
}

finger::result_reg_auto finger::CMD_getRegAutoResult() {
    if (_receive(CODE_CMD_AUTO_REG)) {
        uint32_t id = data[1];
        id = (id << 8) | data[2];
        return result_reg_auto(lastError, id, data[3], data[0]);
    }
}

finger::result_basic finger::CMD_match() {
    _sendCommand(CODE_CMD_MATCH, NULL, 0);
    if (_receive(CODE_CMD_MATCH)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}

finger::result_match finger::CMD_getMatchResult() {
    _sendCommand(CODE_CMD_MATCH_RESULT, NULL, 0);
    if (_receive(CODE_CMD_MATCH_RESULT)) {
        uint32_t result = data[0];
        result = (result << 8) | data[1];
        uint32_t score = data[2];
        score = (score << 8) | data[3];
        uint32_t id = data[4];
        id = (id << 8) | data[5];
        return result_match(lastError, result, id, score);
    } else {
        return result_match(0xFFFFFF, 0, 0, 0);
    }
}

finger::result_basic finger::CMD_update(uint32_t id) {
    uint8_t tmp[2] = {(id >> 8) & 0xff, id & 0xff};
    _sendCommand(CODE_CMD_UPDATE, tmp, 2);
    if (_receive(CODE_CMD_UPDATE)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}
finger::result_basic finger::CMD_getUpdateResult() {
    _sendCommand(CODE_CMD_UPDATE_RESULT, NULL, 0);
    if (_receive(CODE_CMD_UPDATE_RESULT)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}

finger::result_basic finger::CMD_confirm() {
    _sendCommand(CODE_CMD_CONFIRM, NULL, 0);
    if (_receive(CODE_CMD_CONFIRM)) {
        return result_basic(lastError);
    } else {
        return result_basic(0xFFFFFF);
    }
}
finger::result_match finger::CMD_getConfirmResult() {
    _sendCommand(CODE_CMD_CONFIRM_RESULT, NULL, 0);
    if (_receive(CODE_CMD_CONFIRM_RESULT)) {
        uint32_t result = data[0];
        result = (result << 8) | data[1];
        uint32_t score = data[2];
        score = (score << 8) | data[3];
        uint32_t id = data[4];
        id = (id << 8) | data[5];
        return result_match(lastError, result, id, score);
    } else {
        return result_match(0xFFFFFF, 0, 0, 0);
    }
}

finger::result_match finger::CMD_matchSync() {
    _sendCommand(CODE_CMD_MATCH_SYNC, NULL, 0);
    if (_receive(CODE_CMD_MATCH_SYNC)) {
        uint32_t result = data[0];
        result = (result << 8) | data[1];
        uint32_t score = data[2];
        score = (score << 8) | data[3];
        uint32_t id = data[4];
        id = (id << 8) | data[5];
        return result_match(lastError, result, id, score);
    } else {
        return result_match(0xFFFFFF, 0, 0, 0);
    }
}