#ifndef NETWORK_H
#define NETWORK_H
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "../data/DataParser.h"
namespace network{
    extern data_parser::Parser parser;
    extern WiFiClientSecure client;
    extern int connectCooldown;
    void init();
    void loop();
    void process(data_parser::Result result);
    void send(String type,String data);
}
#endif