#include "responseGenerator.h"
String responseGenerator::getHttpBasic(int contentLength) {
    return "HTTP/1.1 200 OK\r\nContent-Type: "
           "text/html\r\nContent-length: " +
           String(contentLength) + "\r\n\r\n";
}
String responseGenerator::getResponseGenerally(Type type, String title, String data) { return getResponseGenerally(type, title, data, 0); }
String responseGenerator::getResponseGenerally(Type type, String title, String data, int flag) {

    switch (type) {
    case Type::BINARY: {
        int len;
        char buffer[15];
        buffer[0] = 0x01;
        buffer[1] = 0x02;
        len = title.length();
        buffer[2] = (len >> 24) & (0xff);
        buffer[3] = (len >> 16) & (0xff);
        buffer[4] = (len >> 8) & (0xff);
        buffer[5] = (len) & (0xff);
        buffer[6] = 0;
        len = data.length();
        buffer[7] = (len >> 24) & (0xff);
        buffer[8] = (len >> 16) & (0xff);
        buffer[9] = (len >> 8) & (0xff);
        buffer[10] = (len) & (0xff);
        buffer[11] = 0;
        return String(buffer, 6) + title + String(buffer + 7, 4) + data;
    }
    case Type::CSV:
        if (flag & RESPONSE_GENERATOR_FLAG_NO_TITLE) {
            return data;
        }
        return title + "," + data;
    case Type::STRING:
        if (flag & RESPONSE_GENERATOR_FLAG_NO_TITLE) {
            return data;
        }
        return "DATA" + title + "\n" + data + "\n";
    case Type::HTTP: {
        String response = "<html><head><title>" + title + "</title><meta charset=\"UTF-8\" /></head><body><h1>" + title + "</h1><p>" + data + "</p></body></html>";
        if (flag & RESPONSE_GENERATOR_FLAG_SIMPLE) {
            response = title + "\n" + data;
            if (flag & RESPONSE_GENERATOR_FLAG_NO_TITLE) {
                response = data;
            }
        } else if (!(flag & RESPONSE_GENERATOR_FLAG_NO_TITLE)) {
            response = "<html><head><meta charset=\"UTF-8\" /></head><body><p>" + data + "</p></body></html>";
        }

        return getHttpBasic(response.length()) + response;
    }

    default:
        break;
    }
    return "";
}
String responseGenerator::getHTTPResponse(String data) { return getResponseGenerally(Type::HTTP, "", data, RESPONSE_GENERATOR_FLAG_SIMPLE | RESPONSE_GENERATOR_FLAG_NO_TITLE); }