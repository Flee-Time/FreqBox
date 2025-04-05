#include "modules/display/display.h"

// Graphics
#include "graphics/welcome_screen.h"
#include "graphics/usbsd_screen.h"
#include "graphics/anims/nosd_frame0.h"
#include "graphics/anims/nosd_frame1.h"
#include "graphics/menu_graphics.h"

// Create instances of the image structs with friendly names
Image welcomeScreen = {welcome_screen_bits, welcome_screen_width, welcome_screen_height};
Image noSDFrame0 = {nosd_frame0_bits, nosd_frame0_width, nosd_frame0_height};
Image noSDFrame1 = {nosd_frame1_bits, nosd_frame1_width, nosd_frame1_height};
Image usbSDScreen = {usbsd_screen_bits, usbsd_screen_width, usbsd_screen_height};

// Helper functions from another script
extern uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

MenuItem menuItems[] = {
    {"Subghz Test", NULL, NULL},
    {"Option 2", clock_icon, NULL},
    {"System Settings", settings_icon, NULL},
    {"Option 4", NULL, NULL},
    {"Option 5", NULL, NULL}};

Menu mainMenu = {
    "Main Menu",
    menuItems,
    sizeof(menuItems) / sizeof(MenuItem)};

// Initialize the display manager
int DM_init(DisplayManager *dm)
{
    if (dm == NULL)
        return -1; // Check for null pointers

    u8g2_Setup_sh1106_i2c_128x64_noname_f(&dm->u8g2, U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_stm32_gpio_and_delay);

    dm->buffer = (uint8_t *)malloc(u8g2_GetBufferSize(&dm->u8g2));
    if (dm->buffer == NULL)
    {
        // Write code to debug to serial port once serial port stuff is implemented.
        return -1;
    }

    u8g2_SetBufferPtr(&dm->u8g2, dm->buffer);
    u8g2_InitDisplay(&dm->u8g2);
    u8g2_SetPowerSave(&dm->u8g2, 0);
    u8g2_ClearBuffer(&dm->u8g2);

    return 0;
}

// Clean up resources used by the display manager
void DM_deinit(DisplayManager *dm)
{
    if (dm != NULL && dm->buffer != NULL)
    {
        free(dm->buffer);
        dm->buffer = NULL;
    }
}

// Helper function to draw an image on the display at a specified position
void drawImage(DisplayManager *dm, const Image *img, int x, int y)
{
    u8g2_ClearBuffer(&dm->u8g2);
    u8g2_SetBitmapMode(&dm->u8g2, 1);
    u8g2_SetDrawColor(&dm->u8g2, 1);
    u8g2_DrawXBM(&dm->u8g2, x, y, img->width, img->height, img->bits);
    u8g2_SendBuffer(&dm->u8g2);
}

// To clear the buffer (and also the screen)
void clearScreen(DisplayManager *dm)
{
    u8g2_ClearBuffer(&dm->u8g2);
    u8g2_SetDrawColor(&dm->u8g2, 1);
    u8g2_SendBuffer(&dm->u8g2);
}

void updateScreen(DisplayManager *dm)
{
    u8g2_SendBuffer(&dm->u8g2);
}

// Start screen display function
void displayWelcomeScreen(DisplayManager *dm)
{
    drawImage(dm, &welcomeScreen, 0, 0);
}

// USB/SD screen display function
void displayUSBSDScreen(DisplayManager *dm)
{
    drawImage(dm, &usbSDScreen, 0, 0);
}

// No SD card animation display function
void displayNoSDAnim(DisplayManager *dm, uint8_t frame)
{
    switch (frame)
    {
    case 0:
        drawImage(dm, &noSDFrame0, 0, 0);
        break;
    case 1:
        drawImage(dm, &noSDFrame1, 0, 0);
        break;
    default:
        break;
    }
}

void displayMenu(DisplayManager *dm, const Menu *menu, uint8_t selection)
{
    extern uint8_t batteryLevel;
    extern char timeChar[8];
    uint8_t y_offset = 0;

    u8g2_ClearBuffer(&dm->u8g2);
    u8g2_SetDrawColor(&dm->u8g2, 1);

    // Main menu backdrop stuff
    u8g2_DrawXBM(&dm->u8g2, 0, 0, 128, 64, m_back);
    u8g2_DrawXBM(&dm->u8g2, 45, 1, bicon_width, bicon_height, battery[batteryLevel]);
    u8g2_SetFont(&dm->u8g2, u8g2_font_4x6_mf);
    u8g2_DrawStr(&dm->u8g2, 1, 7, timeChar);
    // -------------------------------------

    u8g2_SetFont(&dm->u8g2, u8g2_font_6x10_tr);
    u8g2_DrawStr(&dm->u8g2, 65, 9, menu->title);
    u8g2_DrawBox(&dm->u8g2, 124, 5 + 55 / menu->num_items * selection, 3, 54 / menu->num_items);

    for (int i = (selection < (menu->num_items - 1) ? (selection < 1 ? 0 : (selection - 1)) : (selection - 2)); i < menu->num_items; i++)
    {
        u8g2_SetDrawColor(&dm->u8g2, 1);
        u8g2_DrawXBM(&dm->u8g2, 1, 12 + y_offset, menu_item_width, menu_item_height, selection == i ? menu_item_highlighted : menu_item);

        if (menu->items[i].menuIcon != NULL)
        {
            u8g2_SetDrawColor(&dm->u8g2, selection == i ? 0 : 1);
            u8g2_DrawXBM(&dm->u8g2, 5, 14 + y_offset, menu_icon_width, menu_icon_height, menu->items[i].menuIcon);
        }

        u8g2_SetDrawColor(&dm->u8g2, selection == i ? 0 : 1);
        u8g2_SetFont(&dm->u8g2, u8g2_font_6x10_tr);
        u8g2_DrawStr(&dm->u8g2, 20, 23 + y_offset, menu->items[i].label);
        u8g2_SetDrawColor(&dm->u8g2, 0);
        y_offset += 18;
    }

    u8g2_SendBuffer(&dm->u8g2);
}