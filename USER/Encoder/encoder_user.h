/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:25
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_ENCODER_USER_H
#define MULTI_ENCODER_USER_H

#include <stdio.h>
#include "jsmn_user.h"
#include "oled_user.h"

#define ENCODER_EVENT_NUM 6

void encoder_init_user(void);

void encoder_scan_user(void);

void debounce_func(void);


#endif //MULTI_ENCODER_USER_H
