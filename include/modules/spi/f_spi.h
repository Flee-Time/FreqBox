#ifndef __F_SPI_H
#define __F_SPI_H

#ifndef CS_PINS
    #define CS_PINS
    #define CS_GPIO_Port GPIOA
    #define CS_Pin GPIO_PIN_4
#endif

#include "stm32f4xx_hal.h"

/** Transmit&Recieve command to&from the device
 *
 * @param      handle     - pointer to the SPI_HandleTypeDef
 * @param      tx_buffer  - the transmitt buffer you want to transmit
 * @param      rx_buffer  - the recieve buffer for recieving response from spi
 * @param      size       - the size of the transmit and recieve buffer
 * @param      timeout    - the timeout period of the spi transaction
 * 
 * @return     device status
 */
HAL_StatusTypeDef spi_trx(SPI_HandleTypeDef hal_spi, const uint8_t* tx_buffer, uint8_t* rx_buffer, size_t size, uint32_t timeout);

/** Write to the device register using spi
 *
 * @param      handle     - pointer to the SPI_HandleTypeDef
 * @param      addr       - the transmitt buffer you want to transmit
 * @param      pData      - the recieve buffer for recieving response from spi
 * @param      size       - the size of the transmit and recieve buffer
 * @param      timeout    - the timeout period of the spi transaction
 * 
 * @return     device status
 */
HAL_StatusTypeDef spi_write(SPI_HandleTypeDef hal_spi, uint8_t *addr, uint8_t *pData, uint16_t size, uint32_t timeout);

/** Read an spi device's register
 *
 * @param      handle     - pointer to the SPI_HandleTypeDef
 * @param      addr       - the address of the register you want to read
 * @param      pData      - the recieved data from the register
 * @param      size       - the size of the transmit and recieve buffer
 * @param      timeout    - the timeout period of the spi transaction
 * 
 * @return     device status
 */
HAL_StatusTypeDef spi_read(SPI_HandleTypeDef hal_spi, uint8_t *addr, uint8_t *pData, uint16_t size, uint32_t timeout);

#endif