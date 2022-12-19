// Introduction to RTOS - Solution to Part 2 (FreeRTOS)
// https://www.digikey.de/en/maker/projects/introduction-to-rtos-solution-to-part-2-freertos/b3f84c9c9455439ca2dcb8ccfce9dec5

#include <Arduino.h>

// USe only core 1 for demo
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif


// LED rates
static const int rate_1 = 500;  // ms
static const int rate_2 = 323;  // ms

// Pins
static const int led_pin = 5;

// Our task: blink an LED
void toggleLED_1(void *parameter) {
  while(1){
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
  }
}

void toggleLED_2(void *parameter) {
  while(1){
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
  }
}


void setup() {

  // Configure pin
  pinMode(led_pin, OUTPUT);

  // Task to run forever
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              toggleLED_1,    // Function to be called
              "Toggle LED 1", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreRTOS)
              NULL,         // Parameter to pass to function
              1,            // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,         // Task handle
              app_cpu);     // Run on one core for demo (ESP32 only)

              // IF this was vanilla FreeRTOS, you'd want to cal vTaskStartScheduler() in
              // ain after setting up your tasks.

                xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              toggleLED_2,    // Function to be called
              "Toggle LED 2", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreRTOS)
              NULL,         // Parameter to pass to function
              2,            // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,         // Task handle
              app_cpu);     // Run on one core for demo (ESP32 only)

}

void loop() {
  // put your main code here, to run repeatedly:

}