#ifndef door_h_
#define door_h_

#include <Arduino.h>
#include <ESP32Servo.h>
namespace door {
constexpr int servoPin = 1;
constexpr int servoClose = 0;
extern int servoOpen;
extern bool opened;
extern int closeCountdown;
extern Servo servo;
void init();
void loop();
void open(int time);
void close();
bool isOpen();
void testServoOpen(int value);
} // namespace door
#endif