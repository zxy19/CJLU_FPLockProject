#ifndef NETWORK_CONFIG_HANDLER_H
#define NETWORK_CONFIG_HANDLER_H
#include "../data/DataParser.h"
#include "../data/DataType.h"
#include "../data/responseGenerator.h"
#include "../values/values.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>

namespace networkConfigHandler {
constexpr int maxClients = 10;
extern WiFiServer server;
extern DNSServer dnsServer;
extern bool isConfigureMode;
extern String storePass;
extern WiFiClient client[maxClients];
extern data_parser::Parser parser[maxClients];
void init();
void loop();
void startConfigMode();
void stopConfigMode();
void setWiFi(String ssid, String pass);
String response(data_parser::Result result, Type sourceType, Stream *source);
} // namespace networkConfigHandler

#endif