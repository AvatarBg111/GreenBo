/**
  ******************************************************************************
  *                                 GreenBo         
  *                           The Embedded Experts
  ******************************************************************************
  * @file    controls.c
  * @brief   This file provides code for the configuration
  *          of all the requested RAW and controls of the machine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 GreenBo.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by GreenBo under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  **/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "controls.h"
#include "servo.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
uint8_t DataBuff[512];

/* Private function prototypes -----------------------------------------------*/
void Prepare_data_and_log(void)
{
  for(int i = 0; i < 512; i++)
  {
    DataBuff[i] = 0;
  }
  sprintf(DataBuff, "code=cmd_get");
}

uint8_t Calculate_CRC_Data(void)
{
  uint8_t crc = 0;

  for(int i = 0; i < sizeof(DataBuff) - 1; i++)
  {
    crc += DataBuff[i];
  }
  return crc;
}

void initiate_movement(void)
{
  HAL_TIM_PWM_Start(&tim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&tim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&tim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&tim2, TIM_CHANNEL_4);
  TIM2->CCR1 = 3450;
  TIM2->CCR2 = 3400;
  TIM2->CCR3 = 3400;
  TIM2->CCR4 = 3450;
  HAL_GPIO_WritePin(GPIOA, Servo_Grup_Drive_Pin, GPIO_PIN_RESET);
}

  //HAL_GPIO_WritePin(GPIOB, LED_3_Pin, GPIO_PIN_SET);

/* Private user code ---------------------------------------------------------*/


/************************ (C) COPYRIGHT GreenBo *****END OF FILE****/