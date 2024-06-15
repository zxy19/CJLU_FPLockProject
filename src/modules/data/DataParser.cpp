#include "DataParser.h"
// #define DEBUG
void data_parser::Parser::processHTTP(char c) {
    if (flags[DATA_PARSER_HTTP_FLAG_IN_BODY]) {
        buffer[bufferLen++] = c;
        if (bufferLen >= flags[DATA_PARSER_HTTP_FLAG_LENGTH]) {
            data = String(buffer, bufferLen);
            result = data_parser::Result(Type::HTTP, query, data);
            ok = true;
        }
    } else if (flags[DATA_PARSER_HTTP_FLAG_IN_HEADER]) {
        if (c == '\n') {
            if (flags[DATA_PARSER_HTTP_FLAG_LENGTH] == -1) {
                buffer[bufferLen++] = 0;
                flags[DATA_PARSER_HTTP_FLAG_LENGTH] = atoi(buffer);
            } else if (flags[DATA_PARSER_HTTP_FLAG_LINE_TO_LAST] == 0) { // CRLFCRLF=>进入body部分
                flags[DATA_PARSER_HTTP_FLAG_IN_HEADER] = 0;
                flags[DATA_PARSER_HTTP_FLAG_IN_BODY] = 1;
                if (flags[DATA_PARSER_HTTP_FLAG_LENGTH] == 0) {
                    result = data_parser::Result(Type::HTTP, query, "");
                    ok = true;
                }
            }
            bufferLen = 0;
            flags[DATA_PARSER_HTTP_FLAG_LINE_TO_LAST] = 0;
        } else if (c == ':') {
            static const char *CONTENT_LEN = "content-length";
            for (int j = 0, k = 0; j < bufferLen; j++) {
                if (buffer[j] >= 'A' && buffer[j] <= 'Z')
                    buffer[j] += 32;
                if (buffer[j] != CONTENT_LEN[k])
                    break;
                k++;
                if (CONTENT_LEN[k] == '\0') {
                    flags[DATA_PARSER_HTTP_FLAG_LENGTH] = -1;
                    bufferLen = 0;
                }
            }
        } else if (c != '\r') {
            buffer[bufferLen++] = c;
            if (bufferLen >= 200) {
                bufferLen = 0;
            }
            flags[DATA_PARSER_HTTP_FLAG_LINE_TO_LAST]++;
        }
    } else {
        if (c == ' ') {
            query = String(buffer, bufferLen);
        } else if (c == '\n') {
            flags[DATA_PARSER_HTTP_FLAG_LENGTH] = 0;
            flags[DATA_PARSER_HTTP_FLAG_IN_HEADER] = 1;
        } else {
            buffer[bufferLen++] = c;
        }
    }
}
void data_parser::Parser::processTypeJudge(char c) {
#ifdef DEBUG
    getDebugStream()->printf("char[%d] (%d)\r\n", (int)c, bufferLen);
#endif
    buffer[bufferLen++] = c;
    if (buffer[0] != 'P' && buffer[0] != 'G' && buffer[0] != 'D' && buffer[0] != 0x01 && (uint8_t)buffer[0] != 0xFF) {
        bufferLen = 0;
        return;
    }
    if (bufferLen == 5) {
        if (buffer[0] == 'P' && buffer[1] == 'O' && buffer[2] == 'S' && buffer[3] == 'T' && buffer[4] == ' ') {
            bufferLen = 0;
            type = Type::HTTP;
            flags[DATA_PARSER_HTTP_FLAG_IN_BODY] = 0;
            flags[DATA_PARSER_HTTP_FLAG_IN_HEADER] = 0;
            flags[DATA_PARSER_HTTP_FLAG_LENGTH] = 0;
        } else {
            bufferLen = 0;
        }
    }
    if (bufferLen == 4) {
        if (buffer[0] == 'D' && buffer[1] == 'A' && buffer[2] == 'T' && buffer[3] == 'A') {
            bufferLen = 0;
            type = Type::STRING;
            flags[DATA_PARSER_STRING_FLAG_STAGE] = 0;
        }

        if (buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T' && buffer[3] == ' ') {
            bufferLen = 0;
            type = Type::HTTP;
            flags[DATA_PARSER_HTTP_FLAG_IN_BODY] = 0;
            flags[DATA_PARSER_HTTP_FLAG_IN_HEADER] = 0;
            flags[DATA_PARSER_HTTP_FLAG_LENGTH] = 0;
        }
    }
    if (bufferLen == 2) {
        if (buffer[0] == 0x01 && buffer[1] == 0x02) {
            bufferLen = 0;
            type = Type::BINARY;
            flags[DATA_PARSER_BINARY_FLAG_STAGE] = 0;
            flags[DATA_PARSER_BINARY_FLAG_LENGTH_QUERY] = -1;
            flags[DATA_PARSER_BINARY_FLAG_LENGTH_DATA] = -1;
        }
    }
    if (bufferLen == 3) {
        if ((uint8_t)buffer[0] == 0xFF && (uint8_t)buffer[1] == 0xFF && (uint8_t)buffer[2] == 0xFF) {
            bufferLen = 0;
            type = Type::MULTI_BYTE;
        }
    }
}

void data_parser::Parser::processMultiByte(char c) {
    buffer[bufferLen++] = c;
    if (bufferLen == 3) {
        ok = true;
        result = data_parser::Result(Type::MULTI_BYTE, "mb", String(buffer, 3));
    }
}
bool data_parser::Parser::isOK() {
#ifdef DEBUG
    if (ok) {
        getDebugStream()->println("ok");
        getDebugStream()->println(result.query);
        getDebugStream()->println(result.data);
    }
#endif
    return ok;
}

void data_parser::Parser::clear() {
    ok = false;
    bufferLen = 0;
    type = Type::UNKNOWN;
}
data_parser::Result data_parser::Parser::getResult() { return result; }
void data_parser::Parser::processBinary(char c) {
    buffer[bufferLen++] = c;
#ifdef DEBUG
    getDebugStream()->printf("char[%d of %d %d %d] (%d)\r\n", (int)c, flags[DATA_PARSER_BINARY_FLAG_STAGE], flags[DATA_PARSER_BINARY_FLAG_LENGTH_QUERY],
                  flags[DATA_PARSER_BINARY_FLAG_LENGTH_DATA], bufferLen);
#endif
    if (flags[DATA_PARSER_BINARY_FLAG_STAGE] == 0) {
        if (flags[DATA_PARSER_BINARY_FLAG_LENGTH_QUERY] == -1) {
            if (bufferLen == 4) {
                flags[DATA_PARSER_BINARY_FLAG_LENGTH_QUERY] = ((u_char)buffer[0] << 24) | ((u_char)buffer[1] << 16) | ((u_char)buffer[2] << 8) | ((u_char)buffer[3]);
                bufferLen = 0;
                if (flags[DATA_PARSER_BINARY_FLAG_LENGTH_QUERY] == 0) {
                    flags[DATA_PARSER_BINARY_FLAG_STAGE] = 1;
                }
            }
        } else {
            if (bufferLen == flags[DATA_PARSER_BINARY_FLAG_LENGTH_QUERY]) {
                query = String(buffer, bufferLen);
                flags[DATA_PARSER_BINARY_FLAG_STAGE] = 1;
                bufferLen = 0;
            }
        }
    } else {
        if (flags[DATA_PARSER_BINARY_FLAG_LENGTH_DATA] == -1) {
            if (bufferLen == 4) {
                flags[DATA_PARSER_BINARY_FLAG_LENGTH_DATA] = ((u_char)buffer[0] << 24) | ((u_char)buffer[1] << 16) | ((u_char)buffer[2] << 8) | ((u_char)buffer[3]);
                bufferLen = 0;
                if (flags[DATA_PARSER_BINARY_FLAG_LENGTH_DATA] == 0) {
                    ok = true;
                    data="";
                    result = Result(BINARY, query, data);
                }
            }
        } else {
            if (bufferLen == flags[DATA_PARSER_BINARY_FLAG_LENGTH_DATA]) {
                ok = true;
                data = String(buffer, bufferLen);
                result = Result(BINARY, query, data);
                bufferLen = 0;
            }
        }
    }
}
void data_parser::Parser::processString(char c) {
    if (c == '\n') {
        if (flags[DATA_PARSER_STRING_FLAG_STAGE] == 0) {
            query = String(buffer, bufferLen);
            flags[DATA_PARSER_STRING_FLAG_STAGE] = 1;
            bufferLen = 0;
        } else {
            data = String(buffer, bufferLen);
            ok = true;
            result = Result(STRING, query, data);
        }
    } else {
        buffer[bufferLen++] = c;
    }
}
bool data_parser::Parser::nextChar(char c) {
    if (ok)
        return false;
    switch (type) {
    case Type::UNKNOWN:
        processTypeJudge(c);
        break;
    case Type::HTTP:
        processHTTP(c);
        break;
    case Type::BINARY:
        processBinary(c);
        break;
    case Type::STRING:
        processString(c);
        break;
    case Type::MULTI_BYTE:
        processMultiByte(c);
        break;
    default:
        break;
    }
    if (ok) {
        type = Type::UNKNOWN;
    }
    return true;
}
data_parser::Parser::Parser() : result(UNKNOWN, "", "") {
    type = Type::UNKNOWN;
    for (int i = 0; i < 8; i++) {
        flags[i] = 0;
    }
    query = "";
    data = "";
}

data_parser::Result::Result(Type type, String query, String data) : type(type), query(query), data(data) {}