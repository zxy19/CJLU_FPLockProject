#ifndef _FINGER_H_
#define _FINGER_H_
#include "../../api/def.h"
#include "finger_const.h"
#include "finger_type.h"
#include <Arduino.h>
#ifdef DEBUG
#define FINGER_SERIAL Serial2
#else
#define FINGER_SERIAL Serial
#endif
#define FINGER_INT 0
namespace finger {
constexpr uint8_t PASS = 0x00000000;

extern uint8_t buffer[100];
extern uint8_t data[80];
extern uint32_t lastError;
extern uint32_t lastDataLen;
void _write(char c);
void _sendCommand(uint32_t command, const uint8_t *data, uint32_t length);
uint8_t _protocol_get_checksum(const uint8_t *data, uint32_t length);
uint8_t _protocol_get_checksum(uint32_t pass, uint32_t command, const uint8_t *data, uint32_t length);
bool _receive(uint32_t command);

void init();

bool isFingerInt();

/**
 * 心跳
 */
result_basic CMD_heatBeat();
/**
 * 指纹注册
 * @param idx REG_IDX 从1开始，增加到6
 */
result_basic CMD_reg(uint8_t idx);
/**
 * 获取注册结果
 */
result_reg CMD_getRegResult();
/**
 * 保存
 * @param id 指纹ID，由reg返回
 */
result_basic CMD_save(uint32_t id);
/**
 * 获取保存结果
 */
result_id CMD_getSaveResult();
/**
 * 自学习指纹
 * @param id 指纹ID，由match返回
 */
result_basic CMD_update(uint32_t id);
/**
 * 获取自学习结果
 */
result_basic CMD_getUpdateResult();
/**
 * 取消匹配
 */
result_basic CMD_cancel();
/**
 * 自动注册
 * @param waitFinger 是否等待手指
 * @param pressTimes 手指按压次数
 * @param id 指纹ID。使用0xFFFF表示自动分配
 */
void CMD_autoReg(uint8_t waitFinger, uint8_t pressTimes, uint32_t id);
/**
 * 获取自动注册结果
 */
result_reg_auto CMD_getRegAutoResult();
/**
 * 匹配
 */
result_basic CMD_match();
/**
 * 获取匹配结果
 */
result_match CMD_getMatchResult();
/**
 * 匹配（同步模式），直接返回匹配结果
 */
result_match CMD_matchSync();
/**
 * 检测手指是否在位
 */
result_bool CMD_isFingerOn();
/**
 * 确认录入
 */
result_basic CMD_confirm();
/**
 * 获取确认结果
 */
result_match CMD_getConfirmResult();
/**
 * 设置LED模式
 * @param mode 模式
 * @param color 颜色
 * @param a1 最大占空比/点亮时长
 * @param a2 最小占空比/熄灭时长
 * @param a3 占空比每秒变化速率/闪烁周期数量
 */
result_basic CMD_setLed(uint8_t mode, uint8_t color, uint8_t a1, uint8_t a2, uint8_t a3);
} // namespace finger

#endif