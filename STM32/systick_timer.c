#include "stm32h7xx_hal.h"
#include "systick_timer.h"

/* Private variables ---------------------------------------------------------*/

//volatile uint32_t ticks_1ms; // This variable has to be incremented periodically in other timer function (1ms)
uint8_t  msResetFlag = 0;
uint32_t msWaitTime;

uint8_t msResetFlag_ch[16] = {0};
uint32_t msWaitTime_ch[16] = {0};


/* Private function prototypes -----------------------------------------------*/

// return the systicks as milliseconds
inline uint32_t GetSysTickTimer_ms(void)
{
    return uwTick;
}

void clear_wait_ms() {
    msResetFlag = 1;
}

void reset_wait_ms() {
    msWaitTime = GetSysTickTimer_ms();
}

void set_wait_ms(uint32_t time) {
    msWaitTime = time;
}

uint8_t wait_ms(uint32_t time) {
    static uint32_t start, end;

    if (msResetFlag) {
        msResetFlag = 0;
        reset_wait_ms();
    }

    start = msWaitTime;
    end = start + time;
    if (start < end) { 
        if (GetSysTickTimer_ms() >= end) {
            reset_wait_ms();
            return (1);
        } 
    } else { 
        if ((GetSysTickTimer_ms() < start) && (GetSysTickTimer_ms() > end)) {
            reset_wait_ms();
            return (1);
        }
    }
    return (0);
}

/*****************************************************************/
void clear_wait_ms_ch(uint8_t channel){
  
    msResetFlag_ch[channel] = 1;
}

void reset_wait_ms_ch(uint8_t channel){
  
  msWaitTime_ch[channel] = GetSysTickTimer_ms();
}

void set_wait_ms_ch(uint8_t channel, uint32_t time){
  
    msWaitTime_ch[channel] = time;
}

uint8_t wait_ms_ch(uint8_t channel, uint32_t time){
    static uint32_t start, end;

    if(msResetFlag_ch[channel]){

      msResetFlag_ch[channel] = 0;
      reset_wait_ms_ch(channel);
    }

    start = msWaitTime_ch[channel];
    end = start + time;
    if(start < end){
      if(GetSysTickTimer_ms() >= end){
        reset_wait_ms_ch(channel);
        return (1);
      }
    }else{
      if((GetSysTickTimer_ms() < start) && (GetSysTickTimer_ms() > end)){
        reset_wait_ms_ch(channel);
        return (1);
      }
    }
    return (0);
}

/************************ (C) COPYRIGHT GreenBo *****END OF FILE****/