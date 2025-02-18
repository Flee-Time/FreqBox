#ifndef __DISPLAY_CONTROLLER_H
#define __DISPLAY_CONTROLLER_H

// Includes
#include "stm32f4xx_hal.h"
#include <u8g2.h>
#include <stdbool.h>

// Defines
#define screen_width 128
#define screen_height 64

// Structs
typedef struct {
    u8g2_t u8g2;
    uint8_t* buffer;
} DisplayManager;

typedef struct {
    const uint8_t* bits;
    uint16_t width;
    uint16_t height;
} Image;

// Public Functions
int DM_init(DisplayManager* dm);
void DM_deinit(DisplayManager* dm);
void clearScreen(DisplayManager *dm);
void updateScreen(DisplayManager *dm);

// Image Functions
void displayWelcomeScreen(DisplayManager* dm);
void displayUSBSDScreen(DisplayManager* dm);

// Animation Functions
void displayNoSDAnim(DisplayManager *dm, uint8_t frame);

#endif