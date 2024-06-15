#ifndef RESPONSEGENERATOR_H
#define RESPONSEGENERATOR_H

#include "DataType.h"
#include "dataParser.h"
#include <Arduino.h>
namespace responseGenerator {
constexpr int RESPONSE_GENERATOR_FLAG_SIMPLE = (1<<0);
constexpr int RESPONSE_GENERATOR_FLAG_NO_TITLE = (1<<1);
String getResponseGenerally(Type type, String title, String data);
String getResponseGenerally(Type type, String title, String data,int flag);
String getHttpBasic(int contentLength);
String getHTTPResponse(String data);
} // namespace responseGenerator
#endif