#ifndef LEDEN_ENCODER_USER_H
#define LEDEN_ENCODER_USER_H

#include "EC11.h"
#include <stdio.h>
#include "jsmn_user.h"
#include "oled_96.h"
#include "oled_user.h"

void encoder_init_user();
void debounce_func(uint8_t encoder_index);


#endif //LEDEN_ENCODER_USER_H
