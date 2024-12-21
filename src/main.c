/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
#include "modules/display/display.h"
#include "graphics/menu_graphics.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  uint32_t lastUpTick;
  uint32_t lastDownTick;
  uint32_t lastConfirmTick;
  uint32_t lastBackTick;
} InputDebounceTimes;

typedef struct {
  u8g2_t u8g2;
  uint8_t *buffer;
  uint8_t selectedMenu;
  uint8_t selectedItem;
  InputDebounceTimes debounceTimes;
} DisplayContext;

typedef struct {
  char title[32];
  char *itemIcon;
  void (*action)();
  char filename[32];
} MenuItem;

typedef struct {
  char *title;
  MenuItem *items;
  size_t itemCount;
} Menu;

/* Private define ------------------------------------------------------------*/
#define MENU_HIGHLIGHT_THICKNESS 3
#define MENU_ITEM_SPACING 18
#define DEBOUNCE_DELAY 300
#define SD_DETECTION_DELAY 500

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
RTC_HandleTypeDef hrtc;
SD_HandleTypeDef hsd;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static DisplayContext displayContext;
static Menu mainMenu;
static Menu settingsMenu;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void gotoSettings(void);
static void exitSubmenu(void);

/* USER CODE BEGIN 0 */

// Function Prototypes for Menu Operations
void displayMenu(const Menu *menu, uint8_t selection);
uint8_t handleInput(const Menu *menu);
void drawScreen(const char time[7], uint8_t batteryLevel);
void displaySDError(void);

/* Initialize Menus */
void initializeMenus() {
  static MenuItem mainMenuItems[16] = {
      {"Subghz Test", NULL, NULL, ""},
      {"Option 2", NULL, NULL, ""},
      {"System Settings", settings_icon, gotoSettings, ""},
      {"Option 4", NULL, NULL, ""},
      {"Option 5", NULL, NULL, ""}};
  
  mainMenu = (Menu){"Main Menu", mainMenuItems, sizeof(mainMenuItems) / sizeof(MenuItem)};
  
  static MenuItem settingsMenuItems[8] = {
      {"Set DATE&TIME", clock_icon, NULL, ""},
      {"Exit", NULL, exitSubmenu, ""},
      {"Test 1", NULL, NULL, ""},
      {"Test 2", NULL, NULL, ""}};
  
  settingsMenu = (Menu){"Settings", settingsMenuItems, sizeof(settingsMenuItems) / sizeof(MenuItem)};
}

// Menu Navigation
void gotoSettings()
{
  displayContext.selectedMenu = 1;
  displayContext.selectedItem = 0;
}

void exitSubmenu()
{
  displayContext.selectedMenu = -1;
  displayContext.selectedItem = 0;
}

// Display Error if SD card fails
void displaySDError() {
  u8g2_ClearBuffer(&displayContext.u8g2);
  u8g2_SetFont(&displayContext.u8g2, u8g2_font_4x6_mf);
  u8g2_DrawStr(&displayContext.u8g2, 0, 10, "SD Init Failed");
  u8g2_SendBuffer(&displayContext.u8g2);
}

// Main Drawing Function
void drawScreen(const char time[7], uint8_t batteryLevel) {
  Menu *menus[] = {&mainMenu, &settingsMenu};

  u8g2_ClearBuffer(&displayContext.u8g2);
  u8g2_SetBitmapMode(&displayContext.u8g2, 1);
  u8g2_SetDrawColor(&displayContext.u8g2, 1);
  u8g2_DrawXBM(&displayContext.u8g2, 0, 0, screen_width, screen_height, m_back);
  u8g2_DrawXBM(&displayContext.u8g2, 45, 1, bicon_width, bicon_height, battery[batteryLevel]);
  u8g2_SetFont(&displayContext.u8g2, u8g2_font_4x6_mf);
  u8g2_DrawStr(&displayContext.u8g2, 1, 7, time);

  uint8_t selection = handleInput(menus[displayContext.selectedMenu]);
  displayMenu(menus[displayContext.selectedMenu], selection);
  u8g2_SendBuffer(&displayContext.u8g2);
}

/* USER CODE END 0 */

int main(void) {
  /* MCU Configuration */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  
  /* USER CODE BEGIN Init */
  initializeMenus();
  displayContext.u8g2 = initDisplay();
  HAL_Delay(100);
  startScreen(displayContext.u8g2);
  HAL_Delay(750);

  /* Check SD Card */
  while (!BSP_SD_IsDetected()) {
    displaySDError();
    HAL_Delay(SD_DETECTION_DELAY);
  }
  MX_FATFS_Init();

  if (!HAL_GPIO_ReadPin(BUTTON_KEY_GPIO_Port, BUTTON_KEY_Pin)) {
    MX_USB_DEVICE_Init();
    while (1) {
      USBSDScreen(displayContext.u8g2);
      if (!BSP_SD_IsDetected() || HAL_GPIO_ReadPin(BUTTON_CANCEL_GPIO_Port, BUTTON_CANCEL_Pin)) {
        HAL_NVIC_SystemReset();
      }
    }
  }

  uint8_t SDChange = 0;

  /* Infinite loop */
  while (1) {
    while (!BSP_SD_IsDetected()) {
      displaySDError();
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
      SDChange = 1;
    }

    if (SDChange) {
      u8g2_ClearBuffer(&displayContext.u8g2);
      u8g2_SetFont(&displayContext.u8g2, u8g2_font_4x6_mf);
      u8g2_DrawStr(&displayContext.u8g2, 10, 10, "SD Card Change Detected");
      u8g2_DrawStr(&displayContext.u8g2, 10, 25, "Restarting");
      u8g2_SendBuffer(&displayContext.u8g2);

      HAL_Delay(1000);
      HAL_NVIC_SystemReset();
    }
    // drawScreen(, 2); // Uncomment and complete as needed
  }
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
   */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */
}

/**
 * @brief SDIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : BOARD_LED_Pin */
  GPIO_InitStruct.Pin = BOARD_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BOARD_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_KEY_Pin */
  GPIO_InitStruct.Pin = BUTTON_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON_KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_UP_Pin BUTTON_RIGHT_Pin BUTTON_DOWN_Pin */
  GPIO_InitStruct.Pin = BUTTON_UP_Pin|BUTTON_RIGHT_Pin|BUTTON_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_LEFT_Pin BUTTON_CANCEL_Pin BUTTON_CONFIRM_Pin */
  GPIO_InitStruct.Pin = BUTTON_LEFT_Pin|BUTTON_CANCEL_Pin|BUTTON_CONFIRM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SDIO_DETECT_Pin */
  GPIO_InitStruct.Pin = SDIO_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SDIO_DETECT_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
