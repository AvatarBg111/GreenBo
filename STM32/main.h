/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_dma.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
#define PE3_Pin GPIO_PIN_3
#define PE3_GPIO_Port GPIOE
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define LCD_WR_RS_Pin GPIO_PIN_13
#define LCD_WR_RS_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#ifdef TFT96
// QQVGA
#define FrameWidth 160
#define FrameHeight 120
#elif TFT18
// QQVGA2
#define FrameWidth 128
#define FrameHeight 160
#endif

#define ADC_Batt_Pin GPIO_PIN_3 // PC3
#define ADC_Solar_Pin GPIO_PIN_4 // PC4

#define LED_2_Pin GPIO_PIN_12 // PB12
#define LED_3_Pin GPIO_PIN_13 // PB13

#define Servo_Motor1_Pin GPIO_PIN_0 // PA0
#define Servo_Motor2_Pin GPIO_PIN_1 // PA1
#define Servo_Motor3_Pin GPIO_PIN_2 // PA2
#define Servo_Motor4_Pin GPIO_PIN_3 // PA3
#define Servo_Grup_Drive_Pin GPIO_PIN_5 // PA5

#define Servo_Motor5_Pin GPIO_PIN_7 // PA7
#define Servo_Motor6_Pin GPIO_PIN_9 // PA9
#define Servo_Motor7_Pin GPIO_PIN_10 // PA10
#define Servo_Motor8_Pin GPIO_PIN_11 // PA11
#define Servo_Grup_Turn_Pin GPIO_PIN_12 // PA12

#define Servo_Motor9_Pin GPIO_PIN_0 // PB0
#define Servo_Motor10_Pin GPIO_PIN_1 // PB1
#define Servo_Motor11_Pin GPIO_PIN_5 // PB5
#define Servo_Motor12_Pin GPIO_PIN_14 // PD14
#define Servo_Grup_Robot_Pin GPIO_PIN_10 // PB10
/* USER CODE END Private defines */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint16_t pic[FrameWidth][FrameHeight];
extern uint8_t sendBuf[80 + (FrameWidth * FrameHeight * 2)];
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#ifndef __MAIN_H
#define __MAIN_H

/** INCLUDES **/
#include "stm32h750_init.h"
#include "esp32_Rx_Tx.h"

/** FUNCTIONS **/

/** VARIABLES **/

#endif /* __MAIN_H */

