#ifndef FINGER_CONST_H
#define FINGER_CONST_H
#include <Arduino.h>
namespace finger {

constexpr uint32_t CODE_CMD_REG = 0x0111;
constexpr uint32_t CODE_CMD_REG_RESULT = 0x0112;
constexpr uint32_t CODE_CMD_SAVE = 0x0113;
constexpr uint32_t CODE_CMD_SAVE_RESULT = 0x0114;
constexpr uint32_t CODE_CMD_CANCEL = 0x0115;
constexpr uint32_t CODE_CMD_UPDATE = 0x0116;
constexpr uint32_t CODE_CMD_UPDATE_RESULT = 0x0117;
constexpr uint32_t CODE_CMD_AUTO_REG = 0x0118;
constexpr uint32_t CODE_CMD_MATCH = 0x0121;
constexpr uint32_t CODE_CMD_MATCH_RESULT = 0x0122;
constexpr uint32_t CODE_CMD_MATCH_SYNC = 0x0123;
constexpr uint32_t CODE_CMD_DELETE = 0x0131;
constexpr uint32_t CODE_CMD_DELETE_RESULT = 0x0132;
constexpr uint32_t CODE_CMD_IS_EXIST = 0x0133;
constexpr uint32_t CODE_CMD_IS_FINGER = 0x0135;
constexpr uint32_t CODE_CMD_CONFIRM = 0x0141;
constexpr uint32_t CODE_CMD_CONFIRM_RESULT = 0x0142;
constexpr uint32_t CODE_CMD_LED = 0x020F;
constexpr uint32_t CODE_CMD_HEART_BEAT = 0x0303;

constexpr uint32_t AUTOREG_ID_AUTO = 0xFFFF;

constexpr uint8_t LED_MODE_OFF = 0x00;
constexpr uint8_t LED_MODE_ON = 0x01;
constexpr uint8_t LED_MODE_ON_WHEN_TOUCH = 0x02;
constexpr uint8_t LED_MODE_PWM = 0x03;
constexpr uint8_t LED_MODE_WINKLE = 0x04;

constexpr uint8_t LED_COLOR_NONE = 0x00;
constexpr uint8_t LED_COLOR_GREEN = 0x01;
constexpr uint8_t LED_COLOR_RED = 0x02;
constexpr uint8_t LED_COLOR_RED_GREEN = 0x03;
constexpr uint8_t LED_COLOR_BLUE = 0x04;
constexpr uint8_t LED_COLOR_RED_BLUE = 0x05;
constexpr uint8_t LED_COLOR_GREEN_BLUE = 0x06;
constexpr uint8_t LED_COLOR_RED_GREEN_BLUE = 0x07;
} // namespace finger
#endif