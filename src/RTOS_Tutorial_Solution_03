// Introduction to RTOS - Solution to Part 3 (FreeRTOS)
// https://www.digikey.de/en/maker/projects/introduction-to-rtos-solution-to-part-2-freertos/b3f84c9c9455439ca2dcb8ccfce9dec5

#include <Arduino.h>
#include <stdlib.h>

// USe only core 1 for demo
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 20;

// Pins
static const int led_pin = 5;

// Globals
static int led_delay = 500; // ms
String inString = "";       // string to hold input

int input;
String buff;

//*****************************************************************
// Tasks

// Task: Blink LED at rate set by global variable
void toggleLED(void *parameter)
{

  while (1)
  {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
  }
}

// Task: Read from serial terminal
// Feel free to use Serial.readString() or Serial.parseInt(). I'm going to show
// it with atoi() in case you're doing this in a non-Arduino environment. You'd
// also need to replace Serial with your own UART code for non-Arduino.
void readSerial(void *parameters)
{
  char c;

  // Loop forever
  while (1)
  {
    if (Serial.available() > 0)
    {
      c = Serial.read();
      Serial.print(c);

      // if you get a newline, print the string, then the string's value:
      if (c == '\n')
      {
        // Serial.println(inString.toInt());
        Serial.print("You entered: ");
        Serial.println(inString);
        led_delay = inString.toInt();
        // clear the string for new input:
        inString = "";

      } // wait for data available

      if (isDigit(c))
      {
        // convert the incoming byte to a char and add it to the string:
        inString += c;
      }
    }
  }
}

//*****************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup()
{

  // Configue Serial (go slow so we can watch the preemption)
  Serial.begin(115200);

  pinMode(led_pin, OUTPUT);

  // Wait a moment to start (so we dont miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Multi-task LED Demo");
  Serial.println("Enter a number in milliseconds to change the LED delay: ");

  // Start blink task
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      toggleLED,           // Function to be called
      "Toggle LED",        // Name of task
      1024,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass
      1,                   // Task priority
      NULL,                // Task handle
      app_cpu);            // Run on one core for demo purposes (ESP32 only)

  // Start serial read task
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      readSerial,          // Function to be called
      "Read Serial",       // Name of task
      1024,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass
      1,                   // Task priority (must be same to prevent lockup)
      NULL,                // Task handle
      app_cpu);            // Run on one core for demo purposes (ESP32 only)

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop()
{
}