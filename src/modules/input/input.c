#include "modules/input/input.h"

// Define debounce delay in milliseconds
#define DEBOUNCE_DELAY_MS 25
#define REPEAT_INTERVAL_MS 250

// Array to hold the state of all buttons
static ButtonInfo buttonInfo[BUTTON_COUNT];

// GPIO Port and Pins for Buttons
GPIO_TypeDef *buttonPorts[BUTTON_COUNT] = {BUTTON_UP_GPIO_Port, BUTTON_DOWN_GPIO_Port, BUTTON_LEFT_GPIO_Port, BUTTON_RIGHT_GPIO_Port, BUTTON_SELECT_GPIO_Port, BUTTON_BACK_GPIO_Port, BUTTON_KEY_GPIO_Port};
uint16_t buttonPins[BUTTON_COUNT] = {BUTTON_UP_Pin, BUTTON_DOWN_Pin, BUTTON_LEFT_Pin, BUTTON_RIGHT_Pin, BUTTON_SELECT_Pin, BUTTON_BACK_Pin, BUTTON_KEY_Pin};

// Initialize button states
void initializeButtons()
{
    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        buttonInfo[i].lastDebounceTime = 0;
        buttonInfo[i].lastActionTime = 0;
        buttonInfo[i].currentState = BUTTON_STATE_RELEASED;
        buttonInfo[i].previousState = BUTTON_STATE_RELEASED;
    }
}

// Read the current state of a button using GPIO
bool readButtonHardware(Button button)
{
    if (button == BUTTON_KEY)
    {
        // Invert the logic for BUTTON_KEY
        return HAL_GPIO_ReadPin(buttonPorts[button], buttonPins[button]) == GPIO_PIN_RESET;
    }
    else
    {
        return HAL_GPIO_ReadPin(buttonPorts[button], buttonPins[button]) == GPIO_PIN_SET;
    }
}

// Update button states with debouncing logic
void updateButtonStates(uint32_t currentTime)
{
    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        bool rawState = readButtonHardware((Button)i);

        if ((currentTime - buttonInfo[i].lastDebounceTime) > DEBOUNCE_DELAY_MS)
        {
            buttonInfo[i].currentState = rawState ? BUTTON_STATE_PRESSED : BUTTON_STATE_RELEASED;
        }

        if (rawState != (buttonInfo[i].previousState == BUTTON_STATE_PRESSED))
        {
            buttonInfo[i].lastDebounceTime = currentTime;
        }

        buttonInfo[i].previousState = rawState ? BUTTON_STATE_PRESSED : BUTTON_STATE_RELEASED;
    }
}

// Example action methods for each button
void handleButtonUpAction();
void handleButtonDownAction();
void handleButtonLeftAction();
void handleButtonRightAction();
void handleButtonSelectAction();
void handleButtonBackAction();
void handleButtonKeyAction();

// Handle button actions based on their states
void handleButtonActions(uint32_t currentTime)
{
    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        if (buttonInfo[i].currentState == BUTTON_STATE_PRESSED)
        {
            // Check if enough time has passed to trigger another action
            if (buttonInfo[i].previousState == BUTTON_STATE_RELEASED ||
                (currentTime - buttonInfo[i].lastActionTime) >= REPEAT_INTERVAL_MS)
            {
                buttonInfo[i].lastActionTime = currentTime;

                switch ((Button)i)
                {
                case BUTTON_UP:
                    handleButtonUpAction();
                    break;
                case BUTTON_DOWN:
                    handleButtonDownAction();
                    break;
                case BUTTON_LEFT:
                    handleButtonLeftAction();
                    break;
                case BUTTON_RIGHT:
                    handleButtonRightAction();
                    break;
                case BUTTON_SELECT:
                    handleButtonSelectAction();
                    break;
                case BUTTON_BACK:
                    handleButtonBackAction();
                    break;
                case BUTTON_KEY:
                    handleButtonKeyAction();
                    break;
                default:
                    break;
                }
            }
        }
    }
}
