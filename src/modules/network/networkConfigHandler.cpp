#include "networkConfigHandler.h"
#include "../../api/def.h"
#include "../../api/main.h"
#include "../door/door.h"
#include "../finger_register/finger_register.h"
#include "../id/id.h"
#include "../preferences/preferences.h"
// #define NET_SCAN_ASYNC
bool networkConfigHandler::isConfigureMode = false;
data_parser::Parser networkConfigHandler::parser[networkConfigHandler::maxClients];
WiFiClient networkConfigHandler::client[networkConfigHandler::maxClients];
WiFiServer networkConfigHandler::server;
DNSServer networkConfigHandler::dnsServer;
String networkConfigHandler::storePass;
int toCloseConfigMode = 0;
bool startAdding = false;
void networkConfigHandler::init() {
    WiFi.mode(WIFI_AP_STA);
    server.begin(80);

    String ssid = preferences::get("ssid");
    if (ssid != "") {
        String pass = preferences::get("pass");
        WiFi.begin(ssid.c_str(), pass.c_str());
#ifdef DEBUG
        Serial.printf("Connecting to %s\n", ssid.c_str());
#endif
    }
}
void networkConfigHandler::loop() {
    if (storePass != "") {
        if (WiFi.status() == WL_CONNECTED) {
            stopConfigMode();
            preferences::set("ssid", WiFi.SSID());
            preferences::set("pass", storePass);
            storePass = "";
        }
    }
    if (WiFi.status() == WL_CONNECT_FAILED) {
        preferences::set("ssid", "");
#ifdef DEBUG
        Serial.println("WiFi connect failed. clearing");
#endif
    }
    if (server.hasClient()) {
        bool accepted = false;
        for (int i = 0; i < maxClients; i++) {
            if (!client[i].connected()) {
                client[i] = server.available();
                accepted = true;
                break;
            }
        }
        if (!accepted) {
            server.available().stop();
        }
    }
    for (int i = 0; i < maxClients; i++) {
        if (client[i].connected()) {
            while (client[i].available()) {
                parser[i].nextChar(client[i].read());
                if (parser[i].isOK()) {
                    String result = response(parser[i].getResult(), parser[i].getResult().type, &client[i]);
                    if (result != "")
                        client[i].print(result);
                    if (parser[i].getResult().type == Type::HTTP)
                        client[i].stop();
                    parser[i].clear();
                }
            }
        }
    }
    if (toCloseConfigMode > 0) {
        toCloseConfigMode--;
        if (toCloseConfigMode == 0) {
            isConfigureMode = false;
            WiFi.softAPdisconnect();
        }
    }
}

void networkConfigHandler::startConfigMode() {
    isConfigureMode = true;
    WiFi.softAP("FingerprintLock");
    dnsServer.start(53, "*", WiFi.softAPIP());
}
void networkConfigHandler::stopConfigMode() { toCloseConfigMode = 20; }
void networkConfigHandler::setWiFi(String ssid, String pass) {
    WiFi.disconnect();
    storePass = pass;
    WiFi.begin(ssid.c_str(), pass.c_str());
}

String networkConfigHandler::response(data_parser::Result result, Type sourceType, Stream *source) {
#ifdef DEBUG
    Serial.printf("[networkConfigHandler] response: %s||%s\r\n", result.query.c_str(), result.data.c_str());
#endif
    String query = result.query;
    if (query.startsWith("/")) {
        query = query.substring(1);
    }

    if (query == "network") {
        if (!isConfigureMode) {
            return responseGenerator::getHttpBasic(PAGE_NETWORK_NEED_OP.length()) + PAGE_NETWORK_NEED_OP;
        }
        int idx = result.data.indexOf(":::");
        if (idx != -1) {
            String ssid = result.data.substring(0, idx);
            String pass = result.data.substring(idx + 3);
            networkConfigHandler::setWiFi(ssid, pass);
            return responseGenerator::getResponseGenerally(sourceType, "Forming connection to", ssid + ":" + pass);
        }
        return responseGenerator::getResponseGenerally(sourceType, "No data passed", "");
    } else if (query == "add_finger") {
    } else if (query == "networkStatus") {
        if (!isConfigureMode) {
            return responseGenerator::getHttpBasic(PAGE_NETWORK_NEED_OP.length()) + PAGE_NETWORK_NEED_OP;
        }
        switch (WiFi.status()) {
        case WL_CONNECT_FAILED:
        case WL_CONNECTION_LOST:
            return responseGenerator::getResponseGenerally(sourceType, "status", "fail");
        case WL_CONNECTED:
            return responseGenerator::getResponseGenerally(sourceType, "status", "success");
        case WL_DISCONNECTED:
        case WL_IDLE_STATUS:
            return responseGenerator::getResponseGenerally(sourceType, "status", "connecting");
        default:
            return responseGenerator::getResponseGenerally(sourceType, "status", "idle");
        }
    } else if (query == "scan" && result.type == Type::HTTP) {
        if (!isConfigureMode) {
            return responseGenerator::getHttpBasic(PAGE_NETWORK_NEED_OP.length()) + PAGE_NETWORK_NEED_OP;
        }
        WiFi.disconnect();
#ifndef NET_SCAN_ASYNC
        int res = WiFi.scanNetworks(false);
#ifdef DEBUG
        Serial.println("s scan res: " + String(res));
#endif
#else
        int res = WiFi.scanComplete();
#ifdef DEBUG
        Serial.println("scan CP: " + String(res));
#endif
        if (res == -2 || res == -1) {
            if (res == -2)
                WiFi.scanNetworks(true);
            return responseGenerator::getHTTPResponse("{\"finish\":false}");
        }
#endif
        String ret = "{\"finish\":true,\"result\":[";
        for (int i = 0; i < res; i++) {
            ret += "\"" + WiFi.SSID(i) + "\"";
            if (i != res - 1) {
                ret += ",";
            }
        }
        ret += "]}";
        WiFi.scanDelete();
        return responseGenerator::getHttpBasic(ret.length()) + ret;
    } else if (query.startsWith("network?ssid=") && result.type == Type::HTTP) {
        if (!isConfigureMode) {
            return responseGenerator::getHttpBasic(PAGE_NETWORK_NEED_OP.length()) + PAGE_NETWORK_NEED_OP;
        }
        return responseGenerator::getHttpBasic(PAGE_NETWORK_CONF.length()) + PAGE_NETWORK_CONF;
    } else if (query == "op") {
        api::openDoor();
        return responseGenerator::getHTTPResponse("ok");
    } else if (query == "ad") {
        if (startAdding) {
            if (finger_reg::isAdding) {
                return responseGenerator::getHTTPResponse("{\"finish\":false,\"result\":" + String(finger_reg::progress) + "}");
            } else {
                if (finger_reg::ok == 0) {
                    return responseGenerator::getHTTPResponse("{\"finish\":true,\"result\":\"done\"}");
                }
                return responseGenerator::getHTTPResponse("{\"finish\":true,\"result\":" + String(finger_reg::ok) + "}");
            }
        } else {
            startAdding = true;
            if (finger_reg::isAdding) {
                return responseGenerator::getHTTPResponse("{\"finish\":true,\"result\":\"IsAdding\"}");
            } else {
                finger_reg::startAdd();
                return responseGenerator::getHTTPResponse("{\"finish\":false,\"result\":\"starting...\"}");
            }
        }
    } else if (query == "setpw") {
        ID::setPassword(result.data);
        return responseGenerator::getHTTPResponse("ok");
    } else if (query == "getLockState") {
        return responseGenerator::getHTTPResponse(door::isOpen() ? "open" : "close");
    } else if (query == "servo_open_value") {
        return responseGenerator::getHTTPResponse(String(door::servoOpen));
    } else if(query == "set_servo_open_value"){
        door::testServoOpen(result.data.toInt());
        return responseGenerator::getHTTPResponse("ok");
    }

    else if (result.type == Type::HTTP) {
        if (!isConfigureMode) {
            return responseGenerator::getHttpBasic(PAGE_NETWORK_NEED_OP.length()) + PAGE_NETWORK_NEED_OP;
        }
        return responseGenerator::getResponseGenerally(sourceType, "Redirecting", "<script>location.href='network?ssid=" + WiFi.SSID() + "'</script>");
    }
}
