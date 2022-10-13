#include <freertos/FreeRTOS.h>// FreeRTOS para usar delay
#include <freertos/task.h>
#include "sdkconfig.h"  // carrega informações do build
#include "driver/gpio.h"
#include "esp_rom_gpio.h"

// Definições pré main(): pinos, variáveis, etc

//#define pinoLI1 15
#define pinoLI2 2
#define pinoLI3 4
//#define pinoLI4 16
#define pinoSensorRS 17

#define pinoPeltier 5
#define pinoCoolers 18

//
void porta(void *pvParameter)
{
  while(1)
  {
    bool sensorPorta = gpio_get_level(pinoSensorRS);
    if(sensorPorta == 0){
    //gpio_set_level(pinoLI1, 1);
    gpio_set_level(pinoLI2, 1);
    gpio_set_level(pinoLI3, 1);
    //gpio_set_level(pinoLI4, 1);
    }else{
    //gpio_set_level(pinoLI1, 0);
    gpio_set_level(pinoLI2, 0);
    gpio_set_level(pinoLI3, 0);
    //gpio_set_level(pinoLI4, 0);
    }
  }
}

void app_main(void){  // Main // equivalente ao setup() do Arduino

//Pinos Iluminação Interna
//gpio_pad_select_gpio(pinoLI1);
//gpio_set_direction(pinoLI1, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoLI2);
gpio_set_direction(pinoLI2, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoLI3);
gpio_set_direction(pinoLI3, GPIO_MODE_OUTPUT);
//gpio_pad_select_gpio(pinoLI4);
//gpio_set_direction(pinoLI4, GPIO_MODE_OUTPUT);

//Pino Reed Switch
gpio_pad_select_gpio(pinoSensorRS);
gpio_set_direction(pinoSensorRS, GPIO_MODE_INPUT);

//Pinos Relé
gpio_pad_select_gpio(pinoPeltier);
gpio_set_direction(pinoPeltier, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoCoolers);
gpio_set_direction(pinoCoolers, GPIO_MODE_OUTPUT);


//Taks Porta
xTaskCreate(porta,"porta",configMINIMAL_STACK_SIZE,NULL,1,NULL);

gpio_set_level(pinoCoolers, 1); 
gpio_set_level(pinoPeltier, 0); 


    while (1) { //equivalente ao loop() do Arduino   


  
    }// Fim do Loop

}// Fim do Main

