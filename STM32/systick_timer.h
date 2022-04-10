/**
  ******************************************************************************
  *                                 GreenBo         
  *                           The Embedded Experts
  ******************************************************************************
  * @file           : systick_timer.h
  * @brief          : Header for systick_timer.c file.
  *                   This file contains the common defines of the application.
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
  /

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTICK_TIMER_H
#define __SYSTICK_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
typedef enum waitmsChannel_e{
    channelBtnMatrix = 0,
    channelTime1 = 1,
    channelTime2 = 2,
    channelOv7725 = 3,
    channelMlx90640 = 4,
    channelData = 5
} waitmsChannel_t;

/* Exported constants --------------------------------------------------------/
//extern volatile uint32_t ticks_1ms; // This variable has to be incremented periodically in other timer function (1ms)

/* Exported functions prototypes ---------------------------------------------*/
uint32_t GetSysTickTimer_ms(void);

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

void clear_wait_ms();
uint8_t wait_ms(uint32_t time);

void clear_wait_ms_ch(uint8_t channel);
void reset_wait_ms_ch(uint8_t channel);
uint8_t wait_ms_ch(uint8_t channel, uint32_t time);


#ifdef __cplusplus
}
#endif

#endif /* __SYSTICK_TIMER_H */

/************************ (C) COPYRIGHT GreenBo ****************END OF FILE****/