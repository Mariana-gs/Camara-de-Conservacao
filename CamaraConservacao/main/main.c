#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h" 
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include <stdio.h>
#include "driver/adc.h"
#include "sdkconfig.h"
#include "esp_adc_cal.h"
//#include "mqtt.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "freertos/event_groups.h"


// Definições pré main():
#define pinoLI1 2 //LED 1
#define pinoLI2 4 //LED 2
#define pinoSensorRS 17
#define pinoPeltier 5
#define pinoCoolers 18
#define TMP ADC1_CHANNEL_0 //36



// WIFI ------------------------------------------

#define ESP_WIFI_SSID      "Mariana"
#define ESP_WIFI_PASS      "mariana06"

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
//#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

static const char *TAG = "wifi";


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
       // if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
		//{
            esp_wifi_connect();
           // s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        //} else {
        //    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        //s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


void wifi_init_sta(void){
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_STA_DISCONNECTED,
                    &event_handler,
                    NULL,
                    NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
           
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

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
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}



//---------------------------------------




static esp_adc_cal_characteristics_t adc1_chars;
//Iluminação
void porta(void *pvParameter)
{
  while(1)
  {
    bool sensorPorta = gpio_get_level(pinoSensorRS);
    if(sensorPorta == 0){
    gpio_set_level(pinoLI1, 1);
    gpio_set_level(pinoLI2, 1);
    
    }else{
    gpio_set_level(pinoLI1, 0);
    gpio_set_level(pinoLI2, 0);
    
    }
  }
}

void app_main(void){ 

//Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();




//Configura conversor AD
esp_adc_cal_characterize(ADC_UNIT_1,ADC_ATTEN_DB_11,ADC_WIDTH_BIT_DEFAULT,0,&adc1_chars);
adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
adc1_config_channel_atten(TMP, ADC_ATTEN_DB_11);

//Pinos Iluminação Interna
gpio_pad_select_gpio(pinoLI1);
gpio_set_direction(pinoLI1, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoLI2);
gpio_set_direction(pinoLI2, GPIO_MODE_OUTPUT);

//Pino Reed Switch
gpio_pad_select_gpio(pinoSensorRS);
gpio_set_direction(pinoSensorRS, GPIO_MODE_INPUT);

//Pinos Relé
gpio_pad_select_gpio(pinoPeltier);
gpio_set_direction(pinoPeltier, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoCoolers);
gpio_set_direction(pinoCoolers, GPIO_MODE_OUTPUT);

//Ligar Geladeira
gpio_set_level(pinoCoolers, 1); 
gpio_set_level(pinoPeltier, 1); 

//Task Porta
xTaskCreate(porta,"porta",configMINIMAL_STACK_SIZE,NULL,1,NULL);


int tolerancia = 2;
int tempDesejada = 10;
int totaltemp = 0;
uint32_t temp;
int valorSensor;

while(1){
 valorSensor = adc1_get_raw(TMP);
 totaltemp = 0;
 temp = esp_adc_cal_raw_to_voltage(valorSensor,&adc1_chars);
  for(int i = 0; i < 1000; i++){
    temp = esp_adc_cal_raw_to_voltage(valorSensor,&adc1_chars);
    totaltemp += temp;
  }

  temp = (totaltemp/1000)/10;
  printf("Valortemp : %d\n", temp); 
  vTaskDelay(pdMS_TO_TICKS(1000));


  if(temp <= (tempDesejada - tolerancia)){
    //desligar peltier
    //gpio_set_level(pinoCoolers, 1); 
    printf("Valortemp : %d\n", temp); 
    printf("Temperatura Desejada!\nGeladeira Desligada"); 
    gpio_set_level(pinoPeltier, 0); 
}else if(temp >= (tempDesejada + tolerancia)){
    //ligar peltier
    //gpio_set_level(pinoCoolers, 1); 
    //printf("Aumento na Temperatura!\nLigando Geladeira"); 
    gpio_set_level(pinoPeltier, 1); 
}

  
}





    
}// Fim do Main

