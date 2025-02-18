#ifndef __INPUT_H
#define __INPUT_H

#include "stm32f4xx_hal.h"
#include "buttons.h"
#include <stdbool.h>

// Define button states
typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSED
} ButtonState;

// Define possible buttons
typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_SELECT,
    BUTTON_BACK,
    BUTTON_KEY,
    BUTTON_COUNT // Total number of buttons
} Button;

// Structure to hold button information
typedef struct {
    uint32_t lastDebounceTime;
    uint32_t lastActionTime;
    ButtonState currentState;
    ButtonState previousState;
} ButtonInfo;

// Function prototypes
void initializeButtons();
void updateButtonStates(uint32_t currentTime);
void handleButtonActions(uint32_t currentTime);

// Function to get button states from GPIO
bool readButtonHardware(Button button);

#endif