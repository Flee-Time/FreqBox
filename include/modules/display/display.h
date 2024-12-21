#ifndef __DISPLAY_CONTROLLER_H
#define __DISPLAY_CONTROLLER_H

// Includes
#include "stm32f4xx_hal.h"
#include <u8g2.h>
#include <stdio.h>

// Defines
#define screen_width 128
#define screen_height 64

// Public Functions
u8g2_t initDisplay();
void startScreen(u8g2_t u8g2);
void noSDFrame1(u8g2_t u8g2);
void noSDFrame2(u8g2_t u8g2);
void USBSDScreen(u8g2_t u8g2);

#endif