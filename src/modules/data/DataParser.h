#ifndef DATA_PARSER_H
#define DATA_PARSER_H
#include <Arduino.h>
#include "DataType.h"
constexpr int DATA_PARSER_HTTP_FLAG_IN_HEADER = 1;
constexpr int DATA_PARSER_HTTP_FLAG_IN_BODY = 2;
constexpr int DATA_PARSER_HTTP_FLAG_LENGTH = 3;
constexpr int DATA_PARSER_HTTP_FLAG_LINE_TO_LAST = 4;

constexpr int DATA_PARSER_BINARY_FLAG_LENGTH_QUERY = 1;
constexpr int DATA_PARSER_BINARY_FLAG_LENGTH_DATA = 2;
constexpr int DATA_PARSER_BINARY_FLAG_STAGE = 3;

constexpr int DATA_PARSER_STRING_FLAG_STAGE = 1;

namespace data_parser
{
    /**
     * @brief 解析结果
    */
    class Result{
        public:
        String query;
        String data;
        Type type;
        Result(Type type,String query,String data);
    };
    /**
     * 通用数据解析
     * 支持格式如下：
     * 二进制数据：0x01 0x02开头，后4个字节为数据长度，后数据为数据内容
     * 字符串数据：DATA开头，后接请求内容，换行结束，随后为后为数据内容，换行符结束，期间不允许换行
     * HTTP GET数据：GET开头
     * HTTP POST数据：POST开头
    */
    class Parser{
        public:
        /**
         * @brief 解析下一个字符
         * @param c 下一个输入字符
         * @return 字符是否完成消耗
        */
        bool nextChar(char c);
        /**
         * @brief 获取解析结果
         * @return 上次的解析结果
        */
        Result getResult();
        /**
         * @brief 是否解析完成
         * @return 解析是否完成
        */
        bool isOK();
        /**
         * @brief 清除解析结果，开始下一次输入
        */
        void clear();
        Parser();
        private:
        void processTypeJudge(char c);
        void processHTTP(char c);
        void processBinary(char c);
        void processString(char c);
        void processMultiByte(char c);
        Type type;
        char buffer[302];
        int bufferLen;
        int flags[8];
        bool ok;
        String query,data;
        Result result;
    };
}

#endif