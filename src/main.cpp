// Introduction to RTOS - Solution to Part 4 (FreeRTOS)
// https://www.digikey.de/en/maker/projects/introduction-to-rtos-solution-to-part-2-freertos/b3f84c9c9455439ca2dcb8ccfce9dec5

#include <Arduino.h>
#include <stdlib.h>

// USe only core 1 for demo
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Globals
String inString = ""; // string to hold input
static char *msg_ptr = NULL;
static volatile uint8_t msg_flag = 0;

//*****************************************************************
// Tasks

// Task: Blink LED at rate set by global variable
void print_to_serial(void *parameter)
{
  while (1)
  {
    // Wait for flag to be set and print message
    if (msg_flag == 1)
    {
      Serial.println(msg_ptr);

      // Give amount of free heap memory (uncomment if you'd like to see it)
      //      Serial.print("Free heap (bytes): ");
      //      Serial.println(xPortGetFreeHeapSize());

      // Free buffer, set pointer to null, and clear flag
      vPortFree(msg_ptr);
      msg_ptr = NULL;
      msg_flag = 0;
    }
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
        Serial.print("High water mark (words): ");
        Serial.println(uxTaskGetStackHighWaterMark(NULL));

        int count{0};
        for (char x : inString)
        {
          ++count;
        }
        char *message_buff = new char[count];
        int count2{0};
        for (char x : inString)
        {
          message_buff[count2] = x;
          count2++;
        }
        msg_ptr = message_buff;
        // delete[] message_buff;
        Serial.print("Buffer: ");
        for (int i = 0; i < count; ++i)
        {
          Serial.print(msg_ptr[i]);
        }
        Serial.println();

        // clear the string for new input:
        inString = "";
        msg_flag = 1;
      } // wait for data available

      inString += c;
    }
  }
}

//*****************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup()
{

  // Configue Serial (go slow so we can watch the preemption)
  Serial.begin(115200);

  // Wait a moment to start (so we dont miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Memory Management Demo");
  Serial.println("Enter a String: ");

  // Print message found in heap memory to Serial Monitor
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      print_to_serial,     // Function to be called
      "Print to Serial",   // Name of task
      1024,                // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass
      1,                   // Task priority
      NULL,                // Task handle
      app_cpu);            // Run on one core for demo purposes (ESP32 only)

  // Listen for input from Serial Monitor
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