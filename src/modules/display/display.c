#include "modules/display/display.h"

// Graphics
#include "graphics/welcome_screen.h"
#include "graphics/usbsd_screen.h"
#include "graphics/nosd_frame1.h"
#include "graphics/nosd_frame2.h"

extern uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

u8g2_t initDisplay()
{
    u8g2_t u8g2;
    uint8_t* buf;
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_stm32_gpio_and_delay);
    buf = (uint8_t *)malloc(u8g2_GetBufferSize(&u8g2));
    u8g2_SetBufferPtr(&u8g2, buf);
    u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearBuffer(&u8g2);
    return u8g2;
}

void startScreen(u8g2_t u8g2)
{
    u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor(&u8g2,1);
    u8g2_DrawXBM(&u8g2, 0, 0, welcome_screen_width, welcome_screen_height, &welcome_screen_bits);
    u8g2_SendBuffer(&u8g2);
}

void noSDFrame1(u8g2_t u8g2)
{
    u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor(&u8g2,1);
    u8g2_DrawXBM(&u8g2, 0, 0, nosd_frame1_width, nosd_frame1_height, &nosd_frame1_bits);
    u8g2_SendBuffer(&u8g2);
}

void noSDFrame2(u8g2_t u8g2)
{
    u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor(&u8g2,1);
    u8g2_DrawXBM(&u8g2, 0, 0, nosd_frame2_width, nosd_frame2_height, &nosd_frame2_bits);
    u8g2_SendBuffer(&u8g2);
}

void USBSDScreen(u8g2_t u8g2)
{
    u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor(&u8g2,1);
    u8g2_DrawXBM(&u8g2, 0, 0, usbsd_screen_width, usbsd_screen_height, usbsd_screen_bits);
    u8g2_SendBuffer(&u8g2);
}