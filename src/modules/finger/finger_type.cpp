#include "finger_type.h"

finger::result_basic::result_basic(uint32_t error) {
    this->error = error;
}

bool finger::result_basic::ok() {
    return error == 0;
}

finger::result_id::result_id(uint32_t error, uint32_t id) : result_basic(error) {
    this->id = id;
}

finger::result_reg::result_reg(uint32_t error, uint32_t id,uint8_t proc) : result_id(error, id) {
    this->proc = proc;
}

finger::result_reg_auto::result_reg_auto(uint32_t error, uint32_t id,uint8_t proc,uint8_t times) : result_reg(error, id,proc) {
    this->times = times;
}

finger::result_match::result_match(uint32_t error,uint32_t result,uint32_t id,uint32_t score) : result_id(error, id) {
    this->score = score;
    this->result = result;
}

finger::result_bool::result_bool(uint32_t error,bool result) : result_basic(error) {
    this->result = result;
}