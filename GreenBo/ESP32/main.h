#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>


/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

#define Data_OK 0

////////////////////////////////////////////////////////////////////////////////

/** 
  * @brief  RTC Time structure definition  
  */
typedef struct{
	uint8_t RTC_Hours;  /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */
	uint8_t RTC_Minutes;/*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
	uint8_t RTC_Seconds;/*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */
	uint8_t RTC_H12;    /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef; 

/** 
  * @brief  RTC Date structure definition  
  */
typedef struct{
	uint8_t RTC_WeekDay;/*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */
	uint8_t RTC_Month;  /*!< Specifies the RTC Date Month (in BCD format).
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */
	uint8_t RTC_Date;   /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */
	uint8_t RTC_Year;   /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

typedef struct event_measurement_struct_s {
    uint16_t serial_num;
    uint16_t id_consumer;
    RTC_DateTypeDef start_date;
    RTC_TimeTypeDef start_time;
    uint32_t power;
    uint8_t state;
} event_measurement_struct_t;

typedef struct programData_s {
    uint8_t		startHour;
    uint8_t		startMinute;
	char		separator1;
    uint8_t		endHour;
    uint8_t		endMinute;
	char		separator2;
    struct {
        uint8_t		b_monday		: 1;
        uint8_t		b_tuesday		: 1;
        uint8_t		b_wednesday		: 1;
        uint8_t		b_thursday		: 1;
        uint8_t		b_friday		: 1;
        uint8_t		b_saturday		: 1;
        uint8_t		b_sunday		: 1;
        uint8_t		b_hollidays		: 1;
    } days_of_the_week;
	char		separator3;
	uint8_t		temperature;		
	char		separator4;
	uint8_t		crc;
	char		separator5;
} programData_t;

typedef struct event_table_struct{
	char serial_num[5];
	char consumer_id[5];
}event_table;

typedef struct measurement_table_struct{
	char serial_num[5];
	char consumer_id[5];
	char time_stamp[20];
	char power[10];
	uint8_t state;
}measurement_table;

enum command{
	ov7725 = 0x77,
	mlx90640 = 0x90,
	SEND_MEASUREMENT = 0,
    SEND_SHOT = 1
};

enum consumption_status{
    consumption_off = 0,
	consumption_on = 1,
	first_consumption = 9
};

enum ack{
	OK_ACK = 0x06,
    NAK = 0x15
};

////////////////////////////////////////////////////////////////////////////////

extern programData_t programs[];
extern event_measurement_struct_t event_measurement_for_Rx;
extern event_table event_struct[];
extern measurement_table measurement_struct[];
extern uint8_t index_write_measurement_struct, index_write_event_struct;
extern uint8_t index_send_measurement_struct, index_send_event_struct;
extern uint8_t flag_send_measurement, flag_send_event;

void init_programs_data (programData_t *programs);
int make_file_data (char *buf, uint8_t mode);
char* convert_uint8_charchar(char *buf, uint8_t *number);
char* convert_hex_charchar(char *buf, uint8_t *number);

////////////////////////////////////////////////////////////////////////////////
#endif /* ! _MAIN_H_ */
