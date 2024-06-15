#include "preferences.h"

Preferences preferences::p;
void preferences::init() { p.begin("s"); }

void preferences::set(String key, String value) { p.putString(key.c_str(), value); }

String preferences::get(String key) { return p.getString(key.c_str()); }
