#ifndef ID_H_
#define ID_H_
#include <MD5Builder.h>
namespace ID{
    void init();
    void setPassword(String password);
    extern MD5Builder md5;
    extern String id;
    extern String username;
    extern String password;
}

#endif