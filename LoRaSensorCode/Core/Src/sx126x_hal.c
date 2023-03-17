/*
 * sx126x_hal.c
 *
 *  Created on: 13 Jan. 2023
 *      Author: Scott H
 *      contains functions for my code to interface with the sx1262:
 */
#include "sx126x_hal.h"
#include "stm32l0xx_hal.h"
#include "main.h"

/*
 *TODO:
  - sx126x_hal_reset
  - sx126x_hal_wakeup
  - sx126x_hal_write
  - sx126x_hal_read
 */

/**
 * Radio data transfer - write
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context          Radio implementation parameters
 * @param [in] command          Pointer to the buffer to be transmitted
 * @param [in] command_length   Buffer size to be transmitted
 * @param [in] data             Pointer to the buffer to be transmitted
 * @param [in] data_length      Buffer size to be transmitted
 *
 * @returns Operation status
 */
sx126x_hal_status_t sx126x_hal_write(const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )
{
    //uint32_t context_ptr = context;
    uint8_t spi_tx_buf[SPI_BUFFER_SIZE];
    uint8_t spi_rx_buf[SPI_BUFFER_SIZE];
    uint8_t retval = SX126X_HAL_STATUS_ERROR;
    HAL_GPIO_WritePin(NS_GPIO_Port, NS_Pin, GPIO_PIN_RESET);
    //delay_ns(32); //needs at least 32ns between ns going high and the clk starting
    uint16_t pkt_len = command_length + data_length;
    memset(spi_rx_buf, 0, SPI_BUFFER_SIZE);                 //set to 0
    memset(spi_tx_buf, 0, pkt_len);
    memcpy(spi_tx_buf, command, command_length);
    memcpy(&spi_tx_buf[command_length], data, data_length);
	if (HAL_SPI_TransmitReceive(context, spi_tx_buf, spi_rx_buf, pkt_len, SPI_TIMEOUT_MS) == HAL_OK)
    {
        retval = SX126X_HAL_STATUS_OK;
    }
    delay_ns(32);
    wait_spi_tx(context);
    HAL_GPIO_WritePin(NS_GPIO_Port, NS_Pin, GPIO_PIN_SET);
    //clear_spi_rx(context_ptr);
    return retval;
}

/**
 * Radio data transfer - read
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context          Radio implementation parameters
 * @param [in] command          Pointer to the buffer to be transmitted
 * @param [in] command_length   Buffer size to be transmitted
 * @param [in] data             Pointer to the buffer to be received
 * @param [in] data_length      Buffer size to be received
 *
 * @returns Operation status
 */
sx126x_hal_status_t sx126x_hal_read(const void* context, const uint8_t* command, const uint16_t command_length,
                                    uint8_t* data, const uint16_t data_length )
{
    uint8_t spi_tx_buf[SPI_BUFFER_SIZE];
    uint8_t retval = SX126X_HAL_STATUS_ERROR;
    volatile uint8_t messageReceived = false;
    uint16_t pkt_len =  command_length + data_length;
    HAL_GPIO_WritePin(NS_GPIO_Port, NS_Pin, GPIO_PIN_RESET);
    memset(spi_tx_buf, 0, SPI_BUFFER_SIZE);
    memset(data, 0, data_length);                   //set to 0
    memcpy(spi_tx_buf, command, command_length);   //to ensure only 0s are sent after the command packet

    messageReceived = HAL_SPI_TransmitReceive(context, spi_tx_buf, data, pkt_len, SPI_TIMEOUT_MS);
    if (messageReceived == HAL_OK)
    {
        memcpy( data, &data[command_length], data_length);
        retval = SX126X_HAL_STATUS_OK;
    }

    wait_spi_tx(context);
    HAL_GPIO_WritePin(NS_GPIO_Port, NS_Pin, GPIO_PIN_SET);
    //clear_spi_rx(context_ptr);
    return retval;
}


/**
 * Reset the radio
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context Radio implementation parameters
 *
 * @returns Operation status
 */
sx126x_hal_status_t sx126x_hal_reset( const void* context )
{
    HAL_GPIO_WritePin(NRESET_GPIO_Port, NRESET_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(NRESET_GPIO_Port, NRESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    return SX126X_HAL_STATUS_OK;
}

/**
 * Wake the radio up.
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context Radio implementation parameters
 *
 * @returns Operation status
 */
sx126x_hal_status_t sx126x_hal_wakeup( const void* context )
{
    HAL_GPIO_WritePin(NS_GPIO_Port, NS_Pin, GPIO_PIN_RESET);
    return SX126X_HAL_STATUS_OK;
}



bool ReadyForTx(bool send)//set ANTSW low, DIO2 high to send; DIO2 low if not sending
{
/*
from https://docs.rakwireless.com/Product-Categories/WisDuo/RAK4270-Module/Datasheet/#hardware
RF Switch Control Logic table
ANT_SW  DIO2    Condition
1       0	    RX mode
0	    1	    TX mode
*/
    bool retval = false;

    if (send)
    {
        HAL_GPIO_WritePin(ANT_SW_GPIO_Port, ANT_SW_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DIO2_GPIO_Port, DIO2_Pin, GPIO_PIN_SET);
        retval = true;
    }
    else
    {
        HAL_GPIO_WritePin(ANT_SW_GPIO_Port, ANT_SW_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DIO2_GPIO_Port,DIO2_Pin, GPIO_PIN_RESET);
        retval = true;
    }
    return retval;
}

bool ReadyForRx(bool receive)
{
/*
from https://docs.rakwireless.com/Product-Categories/WisDuo/RAK4270-Module/Datasheet/#hardware
RF Switch Control Logic table
ANT_SW  DIO2    Condition
1       0	    RX mode
0	    1	    TX mode
*/
    bool retval = false;
    if (receive)
    {
        HAL_GPIO_WritePin(ANT_SW_GPIO_Port, ANT_SW_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DIO2_GPIO_Port,DIO2_Pin, GPIO_PIN_RESET);
        retval = true;
    }
    else
    {
        HAL_GPIO_WritePin(ANT_SW_GPIO_Port, ANT_SW_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DIO2_GPIO_Port,DIO2_Pin, GPIO_PIN_RESET);
        retval = true;
    }
    return retval;
}

void delay_ns(uint16_t nanoseconds) //This function will pause for a minimum of 4 operations = 125ns
{
    float loops = nanoseconds / 31.25;
    if (loops > 3)
    {    
        for (uint16_t i = 0; i < nanoseconds; i++)
            asm("NOP");
    }
}
/*
void clear_spi_rx(SPI_HandleTypeDef* hspi)
{
    volatile uint32_t data_to_clear = 0;
    volatile uint8_t clearedbytes = 0;
    while (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE))
    {
        data_to_clear = hspi->Instance->DR;
        clearedbytes++;
    }
    asm("NOP");
    //hspi->Instance->DR = 0;
}
*/

void wait_spi_tx(SPI_HandleTypeDef* hspi)
{
    uint8_t flagstatus = 0;
    while (!flagstatus)
        flagstatus = __HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE);
}