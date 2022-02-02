#include "globals.h"
#include <cstdio>
#include <string.h>

Settings gSettings("res/settings/settings.json");
Mset gM;

void gLog(const char* msg) {
    printf(msg);
    printf("\n");
}
