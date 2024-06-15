#ifndef FINGER_TYPE_H
#define FINGER_TYPE_H
#include <Arduino.h>
namespace finger {
class result_basic {
  public:
    uint32_t error;
    result_basic(uint32_t error);
    bool ok();
};
class result_id : public result_basic {
  public:
    uint32_t id;
    result_id(uint32_t error, uint32_t id);
};
class result_reg : public result_id {
  public:
    uint32_t proc;
    result_reg(uint32_t error, uint32_t id, uint8_t proc);
};
class result_reg_auto : public result_reg {
  public:
    uint32_t times;
    result_reg_auto(uint32_t error, uint32_t id, uint8_t proc, uint8_t times);
};
class result_match : public result_id {
  public:
    uint32_t result;
    uint32_t score;
    result_match(uint32_t error, uint32_t result, uint32_t id, uint32_t score);
};
class result_bool : public result_basic {
  public:
    bool result;
    result_bool(uint32_t error, bool result);
};
} // namespace finger

#endif