/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Program will wake up, attempt to measure internal temperature, turn on a tcxo,
 * broadcast a packet on LoRa, and then return to deep sleep for ie 5 minutes.
 *
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#ifndef STM32L071xx
#define STM32L071xx
#endif
#include "sx126x.h"
#include "stddef.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifndef STM32L071xx
#define STM32L071xx
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  // TODO: measure power usage, turn on/off gpios (set as inputs?) to decrease current draw
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //Ensure radio starts up with a full reset
  HAL_GPIO_WritePin(NRESET_GPIO_PORT, NRESET_PIN, GPIO_PIN_RESET);
  HAL_Delay(150);
  HAL_GPIO_WritePin(NRESET_GPIO_PORT, NRESET_PIN, GPIO_PIN_SET);

  // CS pin should default high
  HAL_GPIO_WritePin(NS_GPIO_PORT, NS_PIN, GPIO_PIN_RESET);
  while (1)
  {
    clear_spi_rx(&hspi1);
    //volatile uint8_t msgrx = 0;
    volatile sx126x_chip_status_t radioStatus;
    radioStatus.chip_mode = SX126X_CHIP_MODE_UNUSED;
    radioStatus.cmd_status = SX126X_CMD_STATUS_RESERVED;
    volatile sx126x_errors_mask_t errorMask = 0;
    volatile sx126x_irq_mask_t irq_status = 0;
    
    // CS pin should default high
    HAL_GPIO_WritePin(NS_GPIO_PORT, NS_PIN, GPIO_PIN_SET);
    ProcessCommands();
    sx126x_clear_device_errors(&hspi1);
    uint8_t cfg = STDBY_RC;
    uint8_t packet_type = SX126X_PKT_TYPE_LORA;
    uint32_t freqInHertz = 918000000; // 918Mhz
    sx126x_pa_cfg_params_t PA_Config;
    PA_Config.pa_duty_cycle = 0x02; //For SX1262, paDutyCycle should not be higher than 0x04.
    PA_Config.hp_max = 0x02;        //Do not set higher than 0x07 (early aging of device)
    PA_Config.device_sel = 0x00;    //0x00 to select the sx2162
    PA_Config.pa_lut = 0x01;        //reserved, always set to 0x01
    sx126x_mod_params_lora_t LoraParams;
    LoraParams.bw = SX126X_LORA_BW_125;
    LoraParams.sf = SX126X_LORA_SF7;
    LoraParams.cr = SX126X_LORA_CR_4_5;
    LoraParams.ldro = false;

    sx126x_pkt_params_lora_t PacketParams;
    PacketParams.crc_is_on = true;
    PacketParams.header_type = SX126X_LORA_PKT_IMPLICIT;
    PacketParams.preamble_len_in_symb = 8;
    PacketParams.invert_iq_is_on = false;
    PacketParams.pld_len_in_bytes = MAX_PACKET_LENGTH;
    uint8_t timeout_ms = 100;

    // Delay duration = Delay(23:0) *15.625 μs
    uint32_t tcxo_delay = TCXO_BOOT_TIME_MS * 1000000 / 15625; // magic number comes from 13.3.5 "SetDIO2AsRfSwitchCtrl", page 84 of DS_SX1261-2_v2.1.pdf
    sx126x_tcxo_ctrl_voltages_t tcxo_voltage = SX126X_TCXO_CTRL_1_8V;

    uint8_t TxPower = 5; // In dbM
    uint8_t buffer[MAX_PACKET_LENGTH] = {0};
    buffer[0] = 's';
    buffer[1] = 'h';
    buffer[2] = 'i';
    buffer[3] = 't';
    buffer[4] = 's';
    buffer[5] = 'h';
    buffer[6] = 'i';
    buffer[7] = 't';

    sx126x_set_standby(&hspi1, cfg);
    ProcessCommands();
    sx126x_set_pkt_type(&hspi1, packet_type);
    ProcessCommands();
    sx126x_set_rf_freq(&hspi1, freqInHertz);
    ProcessCommands();
    sx126x_set_pa_cfg(&hspi1, &PA_Config);
    ProcessCommands();
    sx126x_set_dio2_as_rf_sw_ctrl(&hspi1, false); // although we use DIO2 as an rf switch, also need to use
    ProcessCommands();
    sx126x_set_dio3_as_tcxo_ctrl(&hspi1, tcxo_voltage, tcxo_delay);
    ProcessCommands();
    sx126x_set_tx_params(&hspi1, TxPower, SX126X_RAMP_80_US);
    ProcessCommands();
    sx126x_set_dio_irq_params(&hspi1, SX126X_IRQ_ALL, SX126X_IRQ_ALL, 0, 0);
    ProcessCommands();
    sx126x_set_buffer_base_address(&hspi1, 0, 128); // give each 128 length in a shared buffer; may be nicer to allow for the full 8byte buffer to be shared between the two?
    ProcessCommands();
    sx126x_write_buffer(&hspi1, 0, buffer, MAX_PACKET_LENGTH);
    ProcessCommands();
    sx126x_set_lora_mod_params(&hspi1, &LoraParams);
    ProcessCommands();
    sx126x_set_lora_pkt_params(&hspi1, &PacketParams);
    ProcessCommands();
    // sx126x_set_dio_irq_params(&hspi1, ); //unclear on what the right way to go is here
    sx126x_set_lora_sync_word(&hspi1, (uint8_t)0x1424); // value from google, nothing special
    ProcessCommands();
    sx126x_clear_device_errors(&hspi1);
    ProcessCommands();
    //1000 0001 0111 0100
    ReadyForTx(true);
    ProcessCommands();
    sx126x_set_tx(&hspi1, timeout_ms);
    ProcessCommands();
    ReadyForTx(false);
    ProcessCommands();
    sx126x_get_and_clear_irq_status(&hspi1, &irq_status);
    ProcessCommands();
    errorMask = 0;
    sx126x_get_device_errors(&hspi1, &errorMask);
    errorMask = errorMask & 0x17F; //removes all the bits that are RFU
    ProcessCommands();
    sx126x_get_status(&hspi1, &radioStatus);
    // pause half a second
    HAL_GPIO_WritePin(NS_GPIO_PORT, NS_PIN, GPIO_PIN_SET); // set NS high; radio should sleep
    HAL_Delay(250);
    ProcessCommands();
    // TODO irq tx done or timeout & clear tx_done flag

    /*
    *TODO: create functions for my code to interface with the sx1262:
      - sx126x_hal_reset
      - sx126x_hal_wakeup

    */
    /*
     * 1. If not in STDBY_RC mode, then go to this mode with the command SetStandby(...)
     * 2. Define the protocol (LoRa® or FSK) with the command SetPacketType(...)
     * 3. Define the RF frequency with the command SetRfFrequency(...)
     * 4. Define the Power Amplifier configuration with the command SetPaConfig(...)
     * 5. Define output power and ramping time with the command SetTxParams(...)
     * 6. Define where the data payload will be stored with the command SetBufferBaseAddress(...)
     * 7. Send the payload to the data buffer with the command WriteBuffer(...)
     * 8. Define the modulation parameter according to the chosen protocol with the command SetModulationParams(...)1
     * 9. Define the frame format to be used with the command SetPacketParams(...)2
     * 10. Configure DIO and IRQ: use the command SetDioIrqParams(...) to select TxDone IRQ and map this IRQ to a DIO (DIO1,
     * DIO2 or DIO3)
     * 11. Define Sync Word value: use the command WriteReg(...) to write the value of the register via direct register access
     * 12. Set the circuit in transmitter mode to start transmission with the command SetTx(). Use the parameter to enable
     * Timeout
     * 13. Wait for the IRQ TxDone or Timeout: once the packet has been sent the chip goes automatically to STDBY_RC mode
     * 14. Clear the IRQ TxDone flag
     */
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // TODO some form of state machine?

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void ProcessCommands(void)
{
    bool RadioBusy = true;
    while (RadioBusy) // wait for modem to turn on
    {
      bool PinStatus = HAL_GPIO_ReadPin(BUSY_GPIO_Port, BUSY_Pin);
      RadioBusy = PinStatus;
    }
}

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
