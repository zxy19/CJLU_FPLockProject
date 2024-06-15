#include "door.h"
#include "../network/network.h"
#include "../preferences/preferences.h"
Servo door::servo;
int door::closeCountdown = 0;
bool door::opened = false;
int door::servoOpen = 0;
void door::open(int time = -1) {
    opened = true;
    servo.writeMicroseconds(door::servoOpen);
    if (time != -1)
        closeCountdown = time;
}
void door::close() {
    opened = false;
    network::send("door", "close");
    servo.writeMicroseconds(door::servoClose);
}

void door::init() {
    servo.attach(door::servoPin);
    servo.writeMicroseconds(door::servoClose);
    servoOpen = preferences::get("servoOpen").toInt();
}
void door::testServoOpen(int openValue){
    servo.writeMicroseconds(openValue);
    servoOpen = openValue;
    preferences::set("servoOpen",String(openValue));
}
void door::loop() {
    if (closeCountdown > 0) {
        closeCountdown--;
        if (closeCountdown == 0)
            close();
    }
}
bool door::isOpen() {
    network::send("door", "open");
    return opened;
}