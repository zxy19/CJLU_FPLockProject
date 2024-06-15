#include "id.h"
#include "../../api/def.h"
#include "../network/network.h"
#include "../preferences/preferences.h"
#include <Esp.h>
String ID::password, ID::id, ID::username;
MD5Builder ID::md5;
void ID::init() {
    char ssid[14];
    snprintf(ssid, 14, "%llX", ESP.getEfuseMac());
    md5.begin();
    md5.add(ssid);
    md5.add("_FP_LOCK_SALT_IDSUFHISPDFIO");
    md5.calculate();
    String mdId = md5.toString();
    id = mdId.substring(0, 32);
    username = id.substring(0, 8);
    password = preferences::get("pw");
#ifdef DEBUG
    Serial.println("ID: " + id);
    Serial.println("Username: " + username);
    Serial.println("Password: " + password);
#endif
}
void ID::setPassword(String pw) {
    if (pw == "") {
        password = "";
    } else {
        md5.begin();
        md5.add(pw);
        md5.add("_FP_LOCK_SALT_UISGHPFOIGSUDI");
        md5.calculate();
        password = md5.toString();
    }
#ifdef DEBUG
    Serial.println("Password set to: " + password);
#endif
    preferences::set("pw", password);
    if (network::client.connected()) {
        network::client.stop();
    }
}