/**
  ******************************************************************************
  *                                 GreenBo         
  *                           The Embedded Experts
  ******************************************************************************
  * @file    main.c
  * @brief   Generic application start.
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


#include "main.h"
#include <sys/param.h>

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "esp_http_client.h"

#include "esp_spiffs.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "http.h"
#include "uart.h"

#include "esp_log.h"
#include "esp_eth.h"								// receive request
#include "esp_http_server.h"


////////////////////////////////////////////////////////////////////////////////

#define TWDT_TIMEOUT_S          3
#define TASK_RESET_PERIOD_S     1

/*
 * Macro to check the outputs of TWDT functions and trigger an abort if an
 * incorrect code is returned.
 */
#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
            }                                                          \
})

#define EXAMPLE_AP_WIFI_SSID		"GreenBo"
#define EXAMPLE_AP_WIFI_PASS		""
#define EXAMPLE_MAX_STA_CONN		4

#define EXAMPLE_SSID_Nomber_1
#if defined(EXAMPLE_SSID_Nomber_1)
	#define EXAMPLE_STA_WIFI_SSID		"Tech_D3881996"
	#define EXAMPLE_STA_WIFI_PASS		"FJHPEPJJ"
#elif defined(EXAMPLE_SSID_Nomber_2)
	#define EXAMPLE_STA_WIFI_SSID		"M-Tel_37E3"
	#define EXAMPLE_STA_WIFI_PASS		"485754434A37E34E"
#elif defined(EXAMPLE_SSID_Nomber_3)
	#define EXAMPLE_STA_WIFI_SSID		"Bluebird"
	#define EXAMPLE_STA_WIFI_PASS		"vas1905vld"
#elif defined(EXAMPLE_SSID_Nomber_4)
	#define EXAMPLE_STA_WIFI_SSID		"VASTA"
	#define EXAMPLE_STA_WIFI_PASS		"vasta12345"
#elif defined(EXAMPLE_SSID_Nomber_5)
	#define EXAMPLE_STA_WIFI_SSID		" "
	#define EXAMPLE_STA_WIFI_PASS		" "
#elif defined(EXAMPLE_SSID_Nomber_6)
	#define EXAMPLE_STA_WIFI_SSID		"JIVKO2"
	#define EXAMPLE_STA_WIFI_PASS		"77122006"
#elif defined(EXAMPLE_SSID_Nomber_7)
	#define EXAMPLE_STA_WIFI_SSID		"TiN-Incubator"
	#define EXAMPLE_STA_WIFI_PASS		""
#endif

#define EXAMPLE_STA_MAXIMUM_RETRY	5

#define MAX_EVENT_TAB 10
#define MAX_MEASUREMENT_TAB 50


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

////////////////////////////////////////////////////////////////////////////////

static const char *TAG = "main";
static int s_retry_num = 0;
uint32_t ipaddress, ipnetmask, ipgw;


static TaskHandle_t task_handles[portNUM_PROCESSORS];

//Callback for user tasks created in app_main()
void reset_task(void *arg)
{
    //Subscribe this task to TWDT, then check if it is subscribed
    CHECK_ERROR_CODE(esp_task_wdt_add(NULL), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(NULL), ESP_OK);

    while(1){
        //reset the watchdog every 2 seconds
        CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);  //Comment this line to trigger a TWDT timeout
        vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1000));
    }
}

void main_task(void *arg)
{
	char buf[200];
	uint8_t *file = NULL;

	while(1){
		if(http_ov7725_queue != 0){
			if(xQueueReceive(http_ov7725_queue, &file, 10)){
				ESP_LOGI(TAG, "Send_ov7725_by_http");
				send_data_to_API(file, 38480);
			}
		}else if(http_mlx90640_queue != 0){
			if(xQueueReceive(http_mlx90640_queue, &file, 10)){
				ESP_LOGI(TAG, "Send_mlx90640_by_http");
				send_data_to_API(file, 1024);
			}
		}else if(http_data_queue != 0){
			if(xQueueReceive(http_data_queue, &file, 10)){
				ESP_LOGI(TAG, "Send_data_by_http");
				send_data_to_API(file, 512);
			}
		}

		if(0){
			bzero(buf, 200);
			sprintf(buf, "code=cmd_get");
			send_request_for_controls((char*)&buf);
		}
		//vTaskDelay(pdMS_TO_TICKS(10));   //Delay for 10 miliseconds
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

/* Function to initialize SPIFFS */
static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_STA_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		ipaddress = event->ip_info.ip.addr;
		ipnetmask = event->ip_info.netmask.addr;
		ipgw = event->ip_info.gw.addr;
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* /////// esp_wifi_types.h ///////
 * typedef union {
 *    wifi_ap_config_t  ap;
 *    wifi_sta_config_t sta;
 * } wifi_config_t; */
esp_err_t wifi_init_apsta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = EXAMPLE_AP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_AP_WIFI_SSID),
            .password = EXAMPLE_AP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
			//.channel = AP_CHANNEL
        },
    };
    if (strlen(EXAMPLE_AP_WIFI_PASS) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    wifi_config_t sta_config = {
        .sta = {
            .ssid = EXAMPLE_STA_WIFI_SSID,
            .password = EXAMPLE_STA_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_apsta finished. SSID:%s password:%s",
             EXAMPLE_AP_WIFI_SSID, EXAMPLE_AP_WIFI_PASS);

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_STA_WIFI_SSID, EXAMPLE_STA_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_STA_WIFI_SSID, EXAMPLE_STA_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
 
	return ESP_OK;
}


char* convert_uint8_charchar(char *buf, uint8_t *number){
	uint8_t ones = 0;
    uint8_t tens= 0;
    buf[2] = 0;
  
    if(*number >= 10){
        tens = (int)(*number / 10);
        ones = *number - (tens * 10);
        buf[0] = tens + 0x30;
    }else{
		buf[0] = 0x30;
		ones = *number;
	}

	buf[1] = ones + 0x30;
	return buf;
}

char* convert_hex_charchar(char *buf, uint8_t *number){
	uint8_t k = *number;
    buf[2] = 0;

	k = (k >> 4) & 0x0f;
	if (k < 0x0a) {
		buf[0] = k + 0x30;
	} else {
		buf[0] = k + 0x37;
	}
	
	k = *number & 0x0f;
	if (k < 0x0a) {
		buf[1] = k + 0x30;
	} else {
		buf[1] = k + 0x37;
	}

	return buf;
}




////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


void app_main(void)
{
    printf("Initialize TWDT\n");
    //Initialize or reinitialize TWDT
    CHECK_ERROR_CODE(esp_task_wdt_init(TWDT_TIMEOUT_S, false), ESP_OK);

    //Subscribe Idle Tasks to TWDT if they were not subscribed at startup
#ifndef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));
#endif
#ifndef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(1));
#endif

    //Create user tasks and add them to watchdog (this is made so the watchdog doesn't say processes are "dead" or sth. like it)
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xTaskCreatePinnedToCore(reset_task, "reset task", 1024, NULL, 10, &task_handles[i], i);
    }

   /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(BLINK_GPIO, 0);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(wifi_init_apsta());

    /* Initialize file storage */
    ESP_ERROR_CHECK(init_spiffs());

    /* Start the file server */
    //ESP_ERROR_CHECK(start_file_server("/spiffs"));

	start_uart_event();
    //server = start_webserver();

	http_ov7725_queue = xQueueCreate(8, sizeof(uint8_t*));
	if(!http_ov7725_queue) ESP_LOGI(TAG, "http_ov7725_queue ERR created");

	http_mlx90640_queue = xQueueCreate(8, sizeof(uint8_t*));
	if(!http_mlx90640_queue) ESP_LOGI(TAG, "http_mlx90640_queue ERR created");

	http_data_queue = xQueueCreate(8, sizeof(uint8_t*));
	if(!http_data_queue) ESP_LOGI(TAG, "http_data_queue ERR created");

	/* Send requests */
    xTaskCreate(main_task, "main_task", 50000, NULL, 1|portPRIVILEGE_BIT, NULL);
}


/************************ (C) COPYRIGHT GreenBo *****************END OF FILE****/