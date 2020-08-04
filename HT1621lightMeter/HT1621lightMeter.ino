#include "HT1621.h"
#include <Arduino_FreeRTOS.h>

#include <semphr.h>

HT1621 H;

#define POINT 0b10000000

#define MOD_MAX 2
#define LMOD_MAX 6

const uint8_t dig[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

const uint8_t LIGHT[6] = {0, 20, 50, 100, 150, 255};

int8_t mode = 0;
int8_t lightMode = 0;

char sv[5];
char timeStr[8];

uint16_t sensorValue;
float sensorvoltage;

uint16_t battValue;
float battvoltage;

uint8_t update = 1;
uint8_t clear = 1;

void setup()
{
      H.begin(8, 6, 7);
      H.clear();

      pinMode(2, 2);
      pinMode(10, 2);
      Serial.begin(115200);

      xTaskCreate(vTaskDisplay, // Task function
                  "disp",       // Task name
                  128,          // Stack size
                  NULL,
                  0, // Priority
                  NULL);
      xTaskCreate(vTaskReadInput, // Task function
                  "disp",         // Task name
                  128,            // Stack size
                  NULL,
                  0, // Priority
                  NULL);
      xTaskCreate(vTaskReadKey, // Task function
                  "disp",       // Task name
                  128,          // Stack size
                  NULL,
                  0, // Priority
                  NULL);
}
void vTaskDisplay(void *pvParameters)
{
      for (;;)
      {
            if (clear)
            {
                  for (uint8_t i = 0; i < 17; i++)
                  {
                        H.writeFomart(i, 0x40);
                  }
                  clear = 0;
            }

            H.writeFomart(13, dig[mode] | POINT);

            sprintf(timeStr, "%07d", millis() / 100);
            for (uint8_t i = 0; i < 7; i++)
            {
                  if (i == 5)
                  {
                        H.writeFomart(i, dig[(timeStr[i] - '0')] | POINT);
                  }
                  else
                  {
                        H.writeFomart(i, dig[(timeStr[i] - '0')]);
                  }
            }

            switch (mode)
            {
            case 0:
                  sprintf(sv, "%03d", sensorValue);

                  for (uint8_t i = 0; i < 3; i++)
                  {
                        H.writeFomart(10 + i, dig[(sv[i] - '0')]);
                  }

                  float2bcd3(sensorvoltage, sv);

                  H.writeFomart(14, dig[(sv[0])]);
                  H.writeFomart(15, dig[(sv[1])] | POINT);
                  H.writeFomart(16, dig[(sv[2])]);
                  break;
            case 1:

                  sprintf(sv, "%03d", battValue);

                  for (uint8_t i = 0; i < 3; i++)
                  {
                        H.writeFomart(10 + i, dig[(sv[i] - '0')]);
                  }

                  float2bcd3(battvoltage, sv);

                  H.writeFomart(14, dig[(sv[0])]);
                  H.writeFomart(15, dig[(sv[1])] | POINT);
                  H.writeFomart(16, dig[(sv[2])]);
                  break;
            case 2:

                  H.writeFomart(7, 0x38);
                  sprintf(sv, "%02d", lightMode);
                  H.writeFomart(8, dig[(sv[0] - '0')]);
                  H.writeFomart(9, dig[(sv[1] - '0')]);

                  break;

            default:
                  break;
            }

            if (lightMode != 0)
            {
                  analogWrite(9, LIGHT[lightMode - 1]);
            }
            else
            {
                  if (sensorValue > 200)
                        analogWrite(9, 0);
                  else
                        analogWrite(9, map(sensorValue, 0, 200, 255, 0));
            }

            vTaskDelay(50 / portTICK_PERIOD_MS);
      }
}

void vTaskReadInput(void *pvParameters)
{
      for (;;)
      {
            if (update)
            {
                  sensorValue = analogRead(A0);
                  sensorvoltage = sensorValue * (10.0 / 1023.0);

                  battValue = analogRead(A1);
                  battvoltage = battValue * (5.0 / 1023.0);
            }
            Serial.println(sensorValue);
            vTaskDelay(portTICK_PERIOD_MS);
      }
}

void vTaskReadKey(void *pvParameters)
{
      for (;;)
      {

            if (mode == 2)
            {
                  if (digitalRead(2) == 0)
                  {
                        while (digitalRead(2) == 0)
                              ;
                        lightMode++;
                        
                        if (lightMode > LMOD_MAX)
                              lightMode = 0;
                  }
            }
            else
            {
                  update = digitalRead(2);
            }

            if (digitalRead(10) == 0)
            {
                  while (digitalRead(10) == 0)
                        ;
                  mode++;
                  clear = 1;
                  if (mode > MOD_MAX)
                        mode = 0;
            }
            vTaskDelay(portTICK_PERIOD_MS);
      }
}

void loop()
{
}

uint8_t float2bcd3(float dat, uint8_t *bcd)
{
      //a.b
      uint8_t temp = 0;
      bcd[0] = (int)((int)dat / 10);
      bcd[1] = (int)((int)dat % 10);
      bcd[2] = (int)((int)(dat * 10.0f) % 10);
      return 0;
}
