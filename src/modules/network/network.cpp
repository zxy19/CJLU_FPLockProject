#include "network.h"
#include "../../api/def.h"
#include "../../api/main.h"
#include "../data/responseGenerator.h"
#include "../finger_register/finger_register.h"
#include "../id/id.h"
#include "../values/values.h"
data_parser::Parser network::parser;
WiFiClientSecure network::client;
int network::connectCooldown = 0;
int netcount = 0;
void network::init() {
    client.setCACert(SECUTITY_CA.c_str());
    parser.clear();
}

void network::loop() {
    if (client.connected()) {
        netcount++;
        if (netcount > 100) {
            netcount = 0;
            Serial.println("Ping");
            send("ping", "");
            Serial.println("Ping End");
        }
        while (client.available()) {
#ifdef DEBUG
            Serial.print("#");
#endif
            parser.nextChar(client.read());
            if (parser.isOK()) {
                process(parser.getResult());
                parser.clear();
            }
        }
    } else if (WiFi.status() == WL_CONNECTED && ID::password != "") {
        if (connectCooldown == 0) {
#ifdef DEBUG
            Serial.println("Connecting to server...");
#endif
            if (!client.connect(SERVER_ADDR, SERVER_PORT)) {
                connectCooldown = 100;
            } else {
#ifdef DEBUG
                Serial.println("Success. Sending register");
#endif
                send("reg", ID::id + "|" + ID::username + "|" + ID::password);
            }
        } else {
            connectCooldown--;
        }
    }
}

void network::process(data_parser::Result result) {
    if (result.query == "open") {
        api::openDoor();
    } else if (result.query == "addFinger") {
        finger_reg::startAdd();
    }
}

void network::send(String type, String data) {
#ifdef DEBUG
    Serial.printf("Sending %s %s\n", type.c_str(), data.c_str());
#endif
    client.print(responseGenerator::getResponseGenerally(Type::STRING, type, data));
#ifdef DEBUG
    Serial.printf("Done %s\n", type.c_str(), data.c_str());
#endif
}
