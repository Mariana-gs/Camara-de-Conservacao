#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"  // carrega informações do build
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include <driver/i2c.h>
#include <stdio.h>
#include "HD44780.h" //driver ESP-IDF universal para LCD HD44780


// Definições pré main(): pinos, variáveis, etc

#define pinoLI1 15
#define pinoLI2 2
#define pinoLI3 4
#define pinoLI4 16
#define pinoSensorRS 17

#define pinoPeltier 5
#define pinoCoolers 18

#define LCD_ADDR 0x3F
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2

void porta(void *pvParameter)
{
  while(1)
  {
    bool sensorPorta = gpio_get_level(pinoSensorRS);
    if(sensorPorta == 0){
    gpio_set_level(pinoLI1, 1);
    gpio_set_level(pinoLI2, 1);
    gpio_set_level(pinoLI3, 1);
    gpio_set_level(pinoLI4, 1);
    }else{
    gpio_set_level(pinoLI1, 0);
    gpio_set_level(pinoLI2, 0);
    gpio_set_level(pinoLI3, 0);
    gpio_set_level(pinoLI4, 0);
    }
  }
}

void app_main(void){  // Main // equivalente ao setup() do Arduino

//Pinos Iluminação Interna
gpio_pad_select_gpio(pinoLI1);
gpio_set_direction(pinoLI1, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoLI2);
gpio_set_direction(pinoLI2, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoLI3);
gpio_set_direction(pinoLI3, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoLI4);
gpio_set_direction(pinoLI4, GPIO_MODE_OUTPUT);

//Pino Reed Switch
gpio_pad_select_gpio(pinoSensorRS);
gpio_set_direction(pinoSensorRS, GPIO_MODE_INPUT);

//Pinos Relé
gpio_pad_select_gpio(pinoPeltier);
gpio_set_direction(pinoPeltier, GPIO_MODE_OUTPUT);
gpio_pad_select_gpio(pinoCoolers);
gpio_set_direction(pinoCoolers, GPIO_MODE_OUTPUT);

//LCD
LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);


//Task Porta
xTaskCreate(porta,"porta",configMINIMAL_STACK_SIZE,NULL,1,NULL);

gpio_set_level(pinoCoolers, 1); 
gpio_set_level(pinoPeltier, 0); 



    while (1) { //equivalente ao loop() do Arduino   
    }// Fim do Loop

}// Fim do Main

