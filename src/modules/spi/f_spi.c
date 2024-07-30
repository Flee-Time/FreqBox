#include "modules/spi/f_spi.h"

HAL_StatusTypeDef spi_trx(SPI_HandleTypeDef hal_spi, const uint8_t* tx_buffer, uint8_t* rx_buffer, size_t size, uint32_t timeout)
{
    HAL_StatusTypeDef status;
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    status = HAL_SPI_TransmitReceive(&hal_spi, tx_buffer, rx_buffer, size, timeout);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

    return status;
}

HAL_StatusTypeDef spi_write(SPI_HandleTypeDef hal_spi, uint8_t *addr, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	HAL_StatusTypeDef status;

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); //set Chip Select to Low
	status = HAL_SPI_Transmit(&hal_spi, addr, 1, timeout);
    if(status==HAL_OK && pData!=NULL)
    {
    	status = HAL_SPI_Transmit(&hal_spi, pData, size, timeout);
	}
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //set Chip Select to High
    
    return status;
}

HAL_StatusTypeDef spi_read(SPI_HandleTypeDef hal_spi, uint8_t *addr, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	HAL_StatusTypeDef status;
    
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); //set Chip Select to Low
	status = HAL_SPI_Transmit(&hal_spi, addr, 1, timeout);
	status = HAL_SPI_Receive(&hal_spi, pData, size, timeout);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //set Chip Select to High

	return status;
}