#ifndef preferences_h
#define preferences_h

#include <Preferences.h>
namespace preferences {
extern Preferences p;
void init();
void set(String key, String value);
String get(String key);
} // namespace preferences

#endif