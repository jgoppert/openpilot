/**
 ******************************************************************************
 * @addtogroup OpenPilotSystem OpenPilot System
 * @{
 * @addtogroup OpenPilotCore OpenPilot Core
 * @{
 * @file       pios_board.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Defines board hardware for the OpenPilot Version 1.1 hardware.
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#ifndef STM3210E_OP_H_
#define STM3210E_OP_H_



//------------------------
// Timers and Channels Used
//------------------------
/*
Timer | Channel 1 | Channel 2 | Channel 3 | Channel 4
------+-----------+-----------+-----------+----------
TIM1  | RC In 3   | RC In 6   | RC In 5   |
TIM2  | --------------- PIOS_DELAY -----------------
TIM3  | RC In 7   | RC In 8   | RC In 1   | RC In 2
TIM4  | Servo 1   | Servo 2   | Servo 3   | Servo 4
TIM5  | RC In 4   |           |           |
TIM6  | ----------- PIOS_PWM (Supervisor) ----------
TIM7  |           |           |           |
TIM8  | Servo 5   | Servo 6   | Servo 7   | Servo 8
------+-----------+-----------+-----------+----------
*/

//------------------------
// DMA Channels Used
//------------------------
/* Channel 1  - ADC                             */
/* Channel 2  - SPI1 RX                         */
/* Channel 3  - SPI1 TX                         */
/* Channel 4  - SPI2 RX                         */
/* Channel 5  - SPI2 TX                         */
/* Channel 6  -                                 */
/* Channel 7  -                                 */
/* Channel 8  -                                 */
/* Channel 9  -                                 */
/* Channel 10 -                                 */
/* Channel 11 -                                 */
/* Channel 12 -                                 */

//------------------------
// BOOTLOADER_SETTINGS
//------------------------

//#define FUNC_ID				1
//#define HW_VERSION			01

#define BOOTLOADER_VERSION	0
#define BOARD_TYPE		0x01  // OpenPilot board
#define BOARD_REVISION		0x01  // Beta version
//#define HW_VERSION	(BOARD_TYPE << 8) | BOARD_REVISION

#define MEM_SIZE			524288 //512K
#define SIZE_OF_DESCRIPTION	(uint8_t) 100
#define START_OF_USER_CODE	(uint32_t)0x08005000//REMEMBER SET ALSO IN link_stm32f10x_HD_BL.ld
#define SIZE_OF_CODE		(uint32_t) (MEM_SIZE-(START_OF_USER_CODE-0x08000000)-SIZE_OF_DESCRIPTION)

#ifdef STM32F10X_HD
		#define HW_TYPE			0 //0=high_density 1=medium_density;
#elif STM32F10X_MD
		#define HW_TYPE			1 //0=high_density 1=medium_density;
#endif
#define BOARD_READABLE	TRUE
#define BOARD_WRITABLA	TRUE
#define MAX_DEL_RETRYS	3

//------------------------
// WATCHDOG_SETTINGS
//------------------------
#define PIOS_WATCHDOG_TIMEOUT 250
#define PIOS_WDG_REGISTER BKP_DR4
#define PIOS_WDG_ACTUATOR        0x0001
#define PIOS_WDG_STABILIZATION   0x0002
#define PIOS_WDG_AHRS            0x0004
#define PIOS_WDG_MANUAL          0x0008

//------------------------
// PIOS_LED
//------------------------
#define PIOS_LED_LED1_GPIO_PORT                 GPIOC
#define PIOS_LED_LED1_GPIO_PIN                  GPIO_Pin_12
#define PIOS_LED_LED1_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define PIOS_LED_LED2_GPIO_PORT                 GPIOC
#define PIOS_LED_LED2_GPIO_PIN                  GPIO_Pin_13
#define PIOS_LED_LED2_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define PIOS_LED_NUM                            2
#define PIOS_LED_PORTS                          { PIOS_LED_LED1_GPIO_PORT, PIOS_LED_LED2_GPIO_PORT }
#define PIOS_LED_PINS                           { PIOS_LED_LED1_GPIO_PIN, PIOS_LED_LED2_GPIO_PIN }
#define PIOS_LED_CLKS                           { PIOS_LED_LED1_GPIO_CLK, PIOS_LED_LED2_GPIO_CLK }

//------------------------
// PIOS_I2C
// See also pios_board.c
//------------------------
#define PIOS_I2C_MAIN_ADAPTER                   0

//------------------------
// PIOS_BMP085
//------------------------
#define PIOS_BMP085_EOC_GPIO_PORT               GPIOC
#define PIOS_BMP085_EOC_GPIO_PIN                GPIO_Pin_15
#define PIOS_BMP085_EOC_PORT_SOURCE             GPIO_PortSourceGPIOC
#define PIOS_BMP085_EOC_PIN_SOURCE              GPIO_PinSource15
#define PIOS_BMP085_EOC_CLK                     RCC_APB2Periph_GPIOC
#define PIOS_BMP085_EOC_EXTI_LINE               EXTI_Line15
#define PIOS_BMP085_EOC_IRQn                    EXTI15_10_IRQn
#define PIOS_BMP085_EOC_PRIO                    PIOS_IRQ_PRIO_LOW
//#define PIOS_BMP085_OVERSAMPLING                2
#define PIOS_BMP085_OVERSAMPLING                3

//-------------------------
// USART
//
// See also pios_board.c
//-------------------------
#define PIOS_USART_RX_BUFFER_SIZE       512
#define PIOS_USART_TX_BUFFER_SIZE       512

#define PIOS_COM_TELEM_BAUDRATE         57600
#define PIOS_COM_GPS_BAUDRATE           57600

#ifdef PIOS_NO_GPS
	#define PIOS_COM_TELEM_RF               0
	#define PIOS_COM_TELEM_USB              1
#else
	#define PIOS_COM_TELEM_RF               0
	#define PIOS_COM_GPS                    1
	#define PIOS_COM_TELEM_USB              2
#endif

#ifdef PIOS_ENABLE_AUX_UART
    #define PIOS_COM_AUX_BAUDRATE       57600
    #define PIOS_COM_AUX                3
	#define PIOS_COM_DEBUG              PIOS_COM_AUX
#endif

#ifdef PIOS_INCLUDE_SPEKTRUM
    #define PIOS_COM_SPEKTRUM_BAUDRATE  115200
    #define PIOS_COM_SPEKTRUM           3
#endif

//-------------------------
// SPI
//
// See also pios_board.c
//-------------------------
#define PIOS_SDCARD_SPI                         0
#define PIOS_OPAHRS_SPI                         1

//-------------------------
// Delay Timer
//-------------------------
#define PIOS_DELAY_TIMER                        TIM2
#define PIOS_DELAY_TIMER_RCC_FUNC               RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE)

//-------------------------
// System Settings
//-------------------------
#define PIOS_MASTER_CLOCK                       72000000
#define PIOS_PERIPHERAL_CLOCK                   (PIOS_MASTER_CLOCK / 2)
#if defined(USE_BOOTLOADER)
#define PIOS_NVIC_VECTTAB_FLASH                 (START_OF_USER_CODE)
#else
#define PIOS_NVIC_VECTTAB_FLASH                 ((uint32_t)0x08000000)
#endif

//-------------------------
// Interrupt Priorities
//-------------------------
#define PIOS_IRQ_PRIO_LOW                       12              // lower than RTOS
#define PIOS_IRQ_PRIO_MID                       8               // higher than RTOS
#define PIOS_IRQ_PRIO_HIGH                      5               // for SPI, ADC, I2C etc...
#define PIOS_IRQ_PRIO_HIGHEST                   4               // for USART etc...

//-------------------------
// Receiver PWM inputs
//-------------------------
/*#define PIOS_PWM_SUPV_ENABLED                   1
#define PIOS_PWM_SUPV_TIMER                     TIM6
#define PIOS_PWM_SUPV_TIMER_RCC_FUNC            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
#define PIOS_PWM_SUPV_HZ                        25
#define PIOS_PWM_SUPV_IRQ_CHANNEL               TIM6_IRQn
#define PIOS_PWM_SUPV_IRQ_FUNC                  void TIM6_IRQHandler(void)*/

//-------------------------
// Receiver PPM input
//-------------------------
#define PIOS_PPM_GPIO_PORT                      PIOS_PWM_CH1_GPIO_PORT
#define PIOS_PPM_GPIO_PIN                       PIOS_PWM_CH1_PIN
#define PIOS_PPM_TIM_PORT                       PIOS_PWM_CH1_TIM_PORT
#define PIOS_PPM_TIM_CHANNEL                    PIOS_PWM_CH1_CH
#define PIOS_PPM_TIM_CCR                        PIOS_PWM_CH1_CCR
#define PIOS_PPM_TIM                            TIM1
#define PIOS_PPM_TIM_IRQ                        TIM1_CC_IRQn
#define PIOS_PPM_NUM_INPUTS                     8  //Could be more if needed
#define PIOS_PPM_SUPV_ENABLED                   1
#define PIOS_PPM_SUPV_TIMER                     TIM6
#define PIOS_PPM_SUPV_TIMER_RCC_FUNC            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
#define PIOS_PPM_SUPV_HZ                        25
#define PIOS_PPM_SUPV_IRQ_CHANNEL               TIM6_IRQn
#define PIOS_PPM_SUPV_IRQ_FUNC                  void TIM6_IRQHandler(void)

//-------------------------
// SPEKTRUM input
//-------------------------
//#define PIOS_SPEKTRUM_SUPV_ENABLED                   1
//#define PIOS_SPEKTRUM_SUPV_TIMER                     TIM6
//#define PIOS_SPEKTRUM_SUPV_TIMER_RCC_FUNC            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
//#define PIOS_SPEKTRUM_SUPV_HZ                        60 // 1/22ms
//#define PIOS_SPEKTRUM_SUPV_IRQ_CHANNEL               TIM6_IRQn
//#define PIOS_SPEKTRUM_SUPV_IRQ_FUNC                  void TIM6_IRQHandler(void)

//-------------------------
// Servo outputs
//-------------------------
#define PIOS_SERVO_UPDATE_HZ                    50
#define PIOS_SERVOS_INITIAL_POSITION            0 /* dont want to start motors, have no pulse till settings loaded */
#define PIOS_PWM_MAX_INPUTS                     8

//-------------------------
// ADC
// PIOS_ADC_PinGet(0) = Temperature Sensor (On-board)
// PIOS_ADC_PinGet(1) = Power Sensor (Current)
// PIOS_ADC_PinGet(2) = Power Sensor (Voltage)
// PIOS_ADC_PinGet(3) = On-board 5v Rail Sensor
// PIOS_ADC_PinGet(4) = Auxiliary Input 1
// PIOS_ADC_PinGet(5) = Auxiliary Input 2
// PIOS_ADC_PinGet(6) = Auxiliary Input 3
//-------------------------
//#define PIOS_ADC_OVERSAMPLING_RATE            1
#define PIOS_ADC_USE_TEMP_SENSOR                1
#define PIOS_ADC_TEMP_SENSOR_ADC                ADC1
#define PIOS_ADC_TEMP_SENSOR_ADC_CHANNEL        1
#define PIOS_ADC_PIN1_GPIO_PORT                 GPIOA                   // PA1 (Power Sense - Voltage)
#define PIOS_ADC_PIN1_GPIO_PIN                  GPIO_Pin_1              // ADC123_IN1
#define PIOS_ADC_PIN1_GPIO_CHANNEL              ADC_Channel_1
#define PIOS_ADC_PIN1_ADC                       ADC1
#define PIOS_ADC_PIN1_ADC_NUMBER                2
#define PIOS_ADC_PIN2_GPIO_PORT                 GPIOC                   // PC3 (Power Sense - Current)
#define PIOS_ADC_PIN2_GPIO_PIN                  GPIO_Pin_3              // ADC123_IN13
#define PIOS_ADC_PIN2_GPIO_CHANNEL              ADC_Channel_13
#define PIOS_ADC_PIN2_ADC                       ADC2
#define PIOS_ADC_PIN2_ADC_NUMBER                1
#define PIOS_ADC_PIN3_GPIO_PORT                 GPIOC                   // PC5 (Onboard 5v Sensor) PC5
#define PIOS_ADC_PIN3_GPIO_PIN                  GPIO_Pin_5              // ADC12_IN15
#define PIOS_ADC_PIN3_GPIO_CHANNEL              ADC_Channel_15
#define PIOS_ADC_PIN3_ADC                       ADC2
#define PIOS_ADC_PIN3_ADC_NUMBER                2
#define PIOS_ADC_PIN4_GPIO_PORT                 GPIOC                   // PC0 (AUX 1)
#define PIOS_ADC_PIN4_GPIO_PIN                  GPIO_Pin_0              // ADC123_IN10
#define PIOS_ADC_PIN4_GPIO_CHANNEL              ADC_Channel_10
#define PIOS_ADC_PIN4_ADC                       ADC1
#define PIOS_ADC_PIN4_ADC_NUMBER                3
#define PIOS_ADC_PIN5_GPIO_PORT                 GPIOC                   // PC1 (AUX 2)
#define PIOS_ADC_PIN5_GPIO_PIN                  GPIO_Pin_1              // ADC123_IN11
#define PIOS_ADC_PIN5_GPIO_CHANNEL              ADC_Channel_11
#define PIOS_ADC_PIN5_ADC                       ADC2
#define PIOS_ADC_PIN5_ADC_NUMBER                3
#define PIOS_ADC_PIN6_GPIO_PORT                 GPIOC                   // PC2 (AUX 3)
#define PIOS_ADC_PIN6_GPIO_PIN                  GPIO_Pin_2              // ADC123_IN12
#define PIOS_ADC_PIN6_GPIO_CHANNEL              ADC_Channel_12
#define PIOS_ADC_PIN6_ADC                       ADC1
#define PIOS_ADC_PIN6_ADC_NUMBER                4
#define PIOS_ADC_NUM_PINS                       6
#define PIOS_ADC_PORTS                          { PIOS_ADC_PIN1_GPIO_PORT, PIOS_ADC_PIN2_GPIO_PORT, PIOS_ADC_PIN3_GPIO_PORT, PIOS_ADC_PIN4_GPIO_PORT, PIOS_ADC_PIN5_GPIO_PORT, PIOS_ADC_PIN6_GPIO_PORT }
#define PIOS_ADC_PINS                           { PIOS_ADC_PIN1_GPIO_PIN, PIOS_ADC_PIN2_GPIO_PIN, PIOS_ADC_PIN3_GPIO_PIN, PIOS_ADC_PIN4_GPIO_PIN, PIOS_ADC_PIN5_GPIO_PIN, PIOS_ADC_PIN6_GPIO_PIN }
#define PIOS_ADC_CHANNELS                       { PIOS_ADC_PIN1_GPIO_CHANNEL, PIOS_ADC_PIN2_GPIO_CHANNEL, PIOS_ADC_PIN3_GPIO_CHANNEL, PIOS_ADC_PIN4_GPIO_CHANNEL, PIOS_ADC_PIN5_GPIO_CHANNEL, PIOS_ADC_PIN6_GPIO_CHANNEL }
#define PIOS_ADC_MAPPING                        { PIOS_ADC_PIN1_ADC, PIOS_ADC_PIN2_ADC, PIOS_ADC_PIN3_ADC, PIOS_ADC_PIN4_ADC, PIOS_ADC_PIN5_ADC, PIOS_ADC_PIN6_ADC }
#define PIOS_ADC_CHANNEL_MAPPING                { PIOS_ADC_PIN1_ADC_NUMBER, PIOS_ADC_PIN2_ADC_NUMBER, PIOS_ADC_PIN3_ADC_NUMBER, PIOS_ADC_PIN4_ADC_NUMBER, PIOS_ADC_PIN5_ADC_NUMBER, PIOS_ADC_PIN6_ADC_NUMBER }
#define PIOS_ADC_NUM_CHANNELS                   (PIOS_ADC_NUM_PINS + PIOS_ADC_USE_TEMP_SENSOR)
#define PIOS_ADC_NUM_ADC_CHANNELS               2
#define PIOS_ADC_USE_ADC2                       1
#define PIOS_ADC_CLOCK_FUNCTION                 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE)
#define PIOS_ADC_ADCCLK                         RCC_PCLK2_Div8
                                                /* RCC_PCLK2_Div2: ADC clock = PCLK2/2 */
                                                /* RCC_PCLK2_Div4: ADC clock = PCLK2/4 */
                                                /* RCC_PCLK2_Div6: ADC clock = PCLK2/6 */
                                                /* RCC_PCLK2_Div8: ADC clock = PCLK2/8 */
#define PIOS_ADC_SAMPLE_TIME                    ADC_SampleTime_239Cycles5
                                                /* Sample time: */
                                                /* With an ADCCLK = 14 MHz and a sampling time of 293.5 cycles: */
                                                /* Tconv = 239.5 + 12.5 = 252 cycles = 18?s */
                                                /* (1 / (ADCCLK / CYCLES)) = Sample Time (?S) */
#define PIOS_ADC_IRQ_PRIO                       PIOS_IRQ_PRIO_LOW
#define PIOS_ADC_MAX_OVERSAMPLING               10
#define PIOS_ADC_RATE                           (72.0e6 / 1 / 8 / 252 / (PIOS_ADC_NUM_ADC_CHANNELS >> PIOS_ADC_USE_ADC2))

//-------------------------
// GPIO
//-------------------------
#define PIOS_GPIO_1_PORT                        GPIOC
#define PIOS_GPIO_1_PIN                         GPIO_Pin_0
#define PIOS_GPIO_1_GPIO_CLK                    RCC_APB2Periph_GPIOC
#define PIOS_GPIO_2_PORT                        GPIOC
#define PIOS_GPIO_2_PIN                         GPIO_Pin_1
#define PIOS_GPIO_2_GPIO_CLK                    RCC_APB2Periph_GPIOC
#define PIOS_GPIO_3_PORT                        GPIOC
#define PIOS_GPIO_3_PIN                         GPIO_Pin_2
#define PIOS_GPIO_3_GPIO_CLK                    RCC_APB2Periph_GPIOC
#define PIOS_GPIO_4_PORT                        GPIOD
#define PIOS_GPIO_4_PIN                         GPIO_Pin_2
#define PIOS_GPIO_4_GPIO_CLK                    RCC_APB2Periph_GPIOD
#define PIOS_GPIO_PORTS                         { PIOS_GPIO_1_PORT, PIOS_GPIO_2_PORT, PIOS_GPIO_3_PORT, PIOS_GPIO_4_PORT }
#define PIOS_GPIO_PINS                          { PIOS_GPIO_1_PIN, PIOS_GPIO_2_PIN, PIOS_GPIO_3_PIN, PIOS_GPIO_4_PIN }
#define PIOS_GPIO_CLKS                          { PIOS_GPIO_1_GPIO_CLK, PIOS_GPIO_2_GPIO_CLK, PIOS_GPIO_3_GPIO_CLK, PIOS_GPIO_4_GPIO_CLK }
#define PIOS_GPIO_NUM                           4

//-------------------------
// USB
//-------------------------
#define PIOS_USB_ENABLED                        1
#define PIOS_USB_DETECT_GPIO_PORT               GPIOC
#define PIOS_USB_DETECT_GPIO_PIN                GPIO_Pin_4
#define PIOS_USB_DETECT_EXTI_LINE               EXTI_Line4
#define PIOS_IRQ_USB_PRIORITY                   PIOS_IRQ_PRIO_MID
#define PIOS_USB_RX_BUFFER_SIZE                 512
#define PIOS_USB_TX_BUFFER_SIZE                 512

/**
 * glue macros for file IO
 * STM32 uses DOSFS for file IO
 */
#define PIOS_FOPEN_READ(filename,file)  DFS_OpenFile(&PIOS_SDCARD_VolInfo, (uint8_t *)filename, DFS_READ, PIOS_SDCARD_Sector, &file) != DFS_OK

#define PIOS_FOPEN_WRITE(filename,file) DFS_OpenFile(&PIOS_SDCARD_VolInfo, (uint8_t *)filename, DFS_WRITE, PIOS_SDCARD_Sector, &file) != DFS_OK

#define PIOS_FREAD(file,bufferadr,length,resultadr)     DFS_ReadFile(file, PIOS_SDCARD_Sector, (uint8_t*)bufferadr, resultadr, length) != DFS_OK

#define PIOS_FWRITE(file,bufferadr,length,resultadr)    DFS_WriteFile(file, PIOS_SDCARD_Sector, (uint8_t*)bufferadr, resultadr, length)

#define PIOS_FCLOSE(file)               DFS_Close(&file)

#define PIOS_FUNLINK(filename)          DFS_UnlinkFile(&PIOS_SDCARD_VolInfo, (uint8_t *)filename, PIOS_SDCARD_Sector)



#endif /* STM3210E_OP_H_ */
/**
 * @}
 * @}
 */
