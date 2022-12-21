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

// Settings
static const uint8_t buf_len = 255;

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
  char buf[buf_len];
  uint8_t idx = 0;

  // clear whole buffer
  memset(buf, 0, buf_len);

  // Loop forever
  while (1)
  {
    if (Serial.available() > 0)
    {
      c = Serial.read();
      Serial.print(c);

      if (idx < buf_len - 1)
      {
        buf[idx] = c;
        ++idx;
      }

      // if you get a newline, print the string, then the string's value:
      if (c == '\n')
      {
        // The last character in the string is '\n', so we need to replace
        // it with '\0' to make it null-terminated
        buf[idx - 1] = '\0';

        // Try to allocate memory and copy over message. If message buffer is
        // still in use, ignore the entire message.
        if (msg_flag == 0)
        {
          msg_ptr = (char *)pvPortMalloc(idx * sizeof(char));

          // If malloc returns 0 (out of memory), throw an error and reset
          configASSERT(msg_ptr);

          // Copy message
          memcpy(msg_ptr, buf, idx);

          // Notify other task that message is ready
          msg_flag = 1;
        }

        // Reset receive buffer and index counter
        memset(buf, 0, buf_len);
        idx = 0;
      } // wait for data available
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
  Serial.println();
  Serial.println("---FreeRTOS Heap Demo---");
  Serial.println("Enter a string");

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