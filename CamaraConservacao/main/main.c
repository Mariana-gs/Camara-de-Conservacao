#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h" 
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include <stdio.h>
#include "driver/adc.h"
#include "sdkconfig.h"
#include "esp_adc_cal.h"
//#include "HD44780.h"

#define LCD_ADDR 0x27
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2


// Definições pré main():
#define pinoLI1 2 //LED 1
#define pinoLI2 4 //LED 2
#define pinoSensorRS 17
#define pinoPeltier 5
#define pinoCoolers 18
#define TMP ADC1_CHANNEL_0 //36

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
while(1){

 int valorSensor = adc1_get_raw(TMP);

 uint32_t temp;
 int totaltemp = 0;
 temp = esp_adc_cal_raw_to_voltage(valorSensor,&adc1_chars);
  
  for(int i = 0; i < 1000; i++){
    temp = esp_adc_cal_raw_to_voltage(valorSensor,&adc1_chars);
    totaltemp += temp;
  }

  temp = (totaltemp/1000)/10;
  printf("Valortemp : %d\n", temp); 
  vTaskDelay(pdMS_TO_TICKS(1000));

  //printf("valorSensor: %d\n", valorSensor);



}
    
}// Fim do Main

