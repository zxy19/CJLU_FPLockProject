#ifndef FINGER_REGISTER_H
#define FINGER_REGISTER_H
#include "../finger/finger.h"
#include <Arduino.h>
namespace finger_reg{
    extern int progress;
    extern int ok;
    extern int step;
    extern bool isAdding;
    extern bool toSend;
    void loop();
    void startAdd();
}

#endif