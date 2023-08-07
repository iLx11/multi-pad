#ifndef LEDEN_JSMN_USER_H
#define LEDEN_JSMN_USER_H
#define JSMN_HEADER

#include "jsmn.h"
#include "stdio.h"
#include "string.h"

uint8_t parse_json_data(jsmn_parser *p);

void jsmn_init_user();
#endif //LEDEN_JSMN_USER_H
