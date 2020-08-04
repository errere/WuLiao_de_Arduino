#include <MapleFreeRTOS900.h>

#define MOTOR_ST0 0
#define MOTOR_ST1 1

HardwareTimer timer(1);

const uint8_t step1[4] = {1, 1, 0, 0};
const uint8_t step2[4] = {0, 1, 1, 0};
const uint8_t step3[4] = {0, 0, 1, 1};
const uint8_t step4[4] = {1, 0, 0, 1};

const uint8_t stepMap[4] = {PB12, PB14, PB13, PB15};

const uint8_t maxStep = 4;
int8_t ThisStep = 0;

//std
int16_t PuseTime = 1;
int16_t DelayTime = 2;

uint8_t enableSingelPrev = 0;
uint8_t enableSingelNext = 0;

uint8_t enablePrev = 0;
uint8_t enableNext = 0;

uint8_t endPrev = 0;
uint8_t endNext = 0;

//CNT
int16_t Stepcount = 50;
int16_t DelayCNT = 2;

uint8_t CNTFowardEn = 0;
uint8_t CNTBackEn = 0;

uint8_t bootloader();

int8_t getSerTouch();
int8_t getSerTouchNoRTOS();

inline void off();
inline void WriteStep(uint8_t step);

void nextStep(uint8_t PuseTime);
void prevStep(uint8_t PuseTime);

void nextStepNoOff(uint8_t Time);
void prevStepNoOff(uint8_t Time);

void zheng(int dd);
void fan(int dd);

inline void diSplayNumber(int16_t Pt, int16_t Ds);
inline void diSplayStat(uint8_t PR, uint8_t PG, uint8_t DR, uint8_t DG);

//CNT
inline void diSplayNumberCNT(int16_t Ct, int16_t Ds);
inline void diSplayStatCNT(uint8_t CR, uint8_t CG);

void setup()
{
      uint8_t mode;
      // put your setup code here, to run once:
      pinMode(PB12, OUTPUT);
      pinMode(PB13, OUTPUT);
      pinMode(PB14, OUTPUT);
      pinMode(PB15, OUTPUT);
      Serial2.begin(115200);
      Serial.begin(115200);
      delay(500);
      mode = bootloader();
      if (mode == 1)
      {
            delay(500);
            Serial2.print("SPG(10);\r\n");
            delay(500);
            diSplayNumber(PuseTime, DelayTime);
            diSplayStat((enableSingelPrev || enableSingelNext), !(enableSingelPrev || enableSingelNext), (enablePrev || enableNext), !(enablePrev || enableNext));

            xTaskCreate(vGUI, "Task1", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
            xTaskCreate(vMotorFowardSingel, "Task2", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
            xTaskCreate(vMotorPrevSingel, "Task3", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
            xTaskCreate(vMotorFoward, "Task4", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
            xTaskCreate(vMotorPrev, "Task5", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
      }
      else if (mode == 2)
      {
            delay(500);
            Serial2.print("SPG(13);\r\n");
            delay(500);
            diSplayNumberCNT(DelayCNT, Stepcount);
            diSplayStatCNT((CNTFowardEn, CNTBackEn), !(CNTFowardEn, CNTBackEn));

            xTaskCreate(vGUICNT, "Task-1", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
            xTaskCreate(vMotorFowardCNT, "Task-2", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
            xTaskCreate(vMotorPrevCNT, "Task-3", configMINIMAL_STACK_SIZE + 128, NULL, tskIDLE_PRIORITY + 2, NULL);
      }

      vTaskStartScheduler();
}

uint8_t bootloader()
{
      Serial2.print("SPG(1);\r\n");
      for (;;)
      {
            int8_t touch;
            touch = getSerTouchNoRTOS();
            if (touch != 0)
            {
                  Serial2.print("CLS(0);\r\n");
                  return touch;
            }
      }
      return 0;
}

static void vGUI(void *pvParameters)
{
      for (;;)
      {
            int8_t touch;
            touch = getSerTouch();
            if (touch)
            {
                  Serial.println(touch);
                  switch (touch)
                  {
                  case 1:
                        PuseTime--;
                        if (PuseTime < 0)
                              PuseTime = 0;

                        break;
                  case 2:
                        PuseTime++;
                        if (PuseTime > 999)
                              PuseTime = 999;
                        break;

                  case 3:
                        if (enableSingelPrev == 0 && enableSingelNext == 0 && enablePrev == 0 && enableNext == 0)
                        {
                              enableSingelNext = 1;
                        }
                        break;

                  case 4:
                        if (enableSingelPrev == 0 && enableSingelNext == 0 && enablePrev == 0 && enableNext == 0)
                        {
                              enableSingelPrev = 1;
                        }
                        break;

                  case 5:
                        DelayTime--;
                        if (DelayTime < 0)
                              DelayTime = 0;

                        break;

                  case 6:
                        DelayTime++;
                        if (DelayTime > 999)
                              DelayTime = 999;
                        break;

                  case 7:
                        if (enableSingelPrev == 0 && enableSingelNext == 0 && enablePrev == 0 && enableNext == 0)
                        {
                              enablePrev = 0;
                              enableNext = 1;
                        }
                        break;

                  case 8:
                        if (enableSingelPrev == 0 && enableSingelNext == 0 && enablePrev == 0 && enableNext == 0)
                        {
                              enableNext = 0;
                              enablePrev = 1;
                        }
                        break;

                  case 9:
                        enablePrev = 0;
                        enableNext = 0;
                        break;

                  default:
                        break;
                  }

                  diSplayNumber(PuseTime, DelayTime);
                  diSplayStat((enableSingelPrev || enableSingelNext), !(enableSingelPrev || enableSingelNext), (enablePrev || enableNext), !(enablePrev || enableNext));
            }
            if (enableSingelPrev == 0 && enableSingelNext == 0 && enablePrev == 0 && enableNext == 0)
                  off();
      }
} //vGUI

static void vMotorFowardSingel(void *pvParameters)
{
      for (;;)
      {
            if (enableSingelNext)
            {
                  nextStep(PuseTime);
                  vTaskDelay(100);
                  enableSingelNext = 0;
                  diSplayStat((enableSingelPrev || enableSingelNext), !(enableSingelPrev || enableSingelNext), (enablePrev || enableNext), !(enablePrev || enableNext));
            }
            else
                  vTaskDelay(1);
      }
}

static void vMotorPrevSingel(void *pvParameters)
{
      for (;;)
      {
            if (enableSingelPrev)
            {
                  prevStep(PuseTime);
                  vTaskDelay(100);
                  enableSingelPrev = 0;
                  diSplayStat((enableSingelPrev || enableSingelNext), !(enableSingelPrev || enableSingelNext), (enablePrev || enableNext), !(enablePrev || enableNext));
            }
            else
                  vTaskDelay(1);
      }
}

static void vMotorFoward(void *pvParameters)
{
      for (;;)
      {
            if (enableNext)
            {
                  endPrev = 0;
                  zheng(DelayTime);
                  endPrev = 1;
            }
            else
            {
                  vTaskDelay(1);
            }
      }
}

static void vMotorPrev(void *pvParameters)
{
      for (;;)
      {
            if (enablePrev)
            {
                  endNext = 0;
                  fan(DelayTime);
                  endNext = 1;
            }
            else
            {

                  vTaskDelay(1);
            }
      }
}

void loop()
{
}

int8_t getSerTouch()
{ //[BN:6]
      int8_t hot = 0;
      if (Serial2.available())
      {
            char ch = Serial2.read();
            String buf = "";
            if (ch == '[')
            {
                  buf += ch;
                  while (ch != ']')
                  {
                        vTaskDelay(2);
                        ch = Serial2.read();
                        buf += ch;
                  }
#ifdef DEBUG
                  Serial.println(buf);
#endif // DEBUG
                  bool passed = false;
                  for (uint8_t i = 0; i < buf.length(); i++)
                  {
                        if (buf[i] == ':')
                        {
                              passed = true;
#ifdef DEBUG
                              Serial.print("passen in ");
                              Serial.println(i);
#endif // DEBUG
                              continue;
                        }
                        if (!passed)
                              continue;
                        if (buf[i] == ']')
                              break;

                        hot *= 10;
                        int8_t tmp = buf[i] - '0';
                        hot += tmp;

                  } //for

            } //if (ch == '[')

      } //available

      return hot;
} //getSerTouch

int8_t getSerTouchNoRTOS()
{ //[BN:6]
      int8_t hot = 0;
      if (Serial2.available())
      {
            char ch = Serial2.read();
            String buf = "";
            if (ch == '[')
            {
                  buf += ch;
                  while (ch != ']')
                  {
                        delay(2);
                        ch = Serial2.read();
                        buf += ch;
                  }
                  bool passed = false;
                  for (uint8_t i = 0; i < buf.length(); i++)
                  {
                        if (buf[i] == ':')
                        {
                              passed = true;

                              continue;
                        }
                        if (!passed)
                              continue;
                        if (buf[i] == ']')
                              break;

                        hot *= 10;
                        int8_t tmp = buf[i] - '0';
                        hot += tmp;

                  } //for

            } //if (ch == '[')

      } //available

      return hot;
} //getSerTouch

inline void off()
{
      digitalWrite(PB12, MOTOR_ST0);
      digitalWrite(PB13, MOTOR_ST0);
      digitalWrite(PB14, MOTOR_ST0);
      digitalWrite(PB15, MOTOR_ST0);
}

inline void WriteStep(uint8_t step)
{

      switch (step)
      {
      case 0:
            for (uint8_t i = 0; i < 4; i++)
                  digitalWrite(stepMap[i], step1[i]);
            break;
      case 1:
            for (uint8_t i = 0; i < 4; i++)
                  digitalWrite(stepMap[i], step2[i]);
            break;
      case 2:
            for (uint8_t i = 0; i < 4; i++)
                  digitalWrite(stepMap[i], step3[i]);
            break;
      case 3:
            for (uint8_t i = 0; i < 4; i++)
                  digitalWrite(stepMap[i], step4[i]);
            break;

      default:
            break;
      }
}

void nextStep(uint8_t PuseTime)
{
      WriteStep(ThisStep);
      vTaskDelay(PuseTime);
      off();
      ThisStep++;
      ThisStep = ThisStep % 4;
}

void prevStep(uint8_t PuseTime)
{
      WriteStep(ThisStep);
      vTaskDelay(PuseTime);
      off();
      ThisStep--;
      if (ThisStep < 0)
            ThisStep = 3;
}

void nextStepNoOff(uint8_t Time)
{
      WriteStep(ThisStep);
      vTaskDelay(Time);
      ThisStep++;
      ThisStep = ThisStep % 4;
}

void prevStepNoOff(uint8_t Time)
{
      WriteStep(ThisStep);
      vTaskDelay(Time);
      ThisStep--;
      if (ThisStep < 0)
            ThisStep = 3;
}

void zheng(int dd)
{
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step1[i]);
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step2[i]);
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step3[i]);
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step4[i]);
}
void fan(int dd)
{
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step4[i]);
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step3[i]);
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step2[i]);
      vTaskDelay(dd);
      for (uint8_t i = 0; i < 4; i++)
            digitalWrite(stepMap[i], step1[i]);
}

inline void diSplayNumber(int16_t Pt, int16_t Ds)
{
      Serial2.print("DS24(121,14,'");
      Serial2.print(Pt);
      Serial2.print("    ',2,156);");

      Serial2.print("DS24(121,103,'");
      Serial2.print(Ds);
      Serial2.print("    ',41,156);");

      Serial2.print("\r\n");
}

inline void diSplayStat(uint8_t PR, uint8_t PG, uint8_t DR, uint8_t DG)
{
      if (PR)
            Serial2.print("CIRF(211,60,8,1);");
      else
            Serial2.print("CIRF(211,60,8,0);");

      if (PG)
            Serial2.print("CIRF(211,80,8,2);");
      else
            Serial2.print("CIRF(211,80,8,0);");

      if (DR)
            Serial2.print("CIRF(211,142,8,1);");
      else
            Serial2.print("CIRF(211,142,8,0);");

      if (DG)
            Serial2.print("CIRF(211,162,8,2);");
      else
            Serial2.print("CIRF(211,162,8,0);");

      Serial2.print("\r\n");
}

//==========================================CNT==========================================

inline void diSplayNumberCNT(int16_t Ct, int16_t Ds)
{
      Serial2.print("DS32(117,45,'");
      Serial2.print(Ct);
      Serial2.print("    ',2,164);");

      Serial2.print("DS32(117,0,'");
      Serial2.print(Ds);
      Serial2.print("     ',2,164);");

      Serial2.print("\r\n");
}

inline void diSplayStatCNT(uint8_t CR, uint8_t CG)
{
      if (CR)
            Serial2.print("CIRF(211,142,8,1);");
      else
            Serial2.print("CIRF(211,142,8,0);");

      if (CG)
            Serial2.print("CIRF(211,162,8,2);");
      else
            Serial2.print("CIRF(211,162,8,0);");

      Serial2.print("\r\n");
}

static void vGUICNT(void *pvParameters)
{
      for (;;)
      {
            int8_t touch;
            touch = getSerTouch();
            if (touch)
            {
                  Serial.println(touch);
                  switch (touch)
                  {
                  case 1:
                        Stepcount--;
                        if (Stepcount < 0)
                              Stepcount = 0;
                        break;
                  case 2:
                        Stepcount++;
                        if (Stepcount > 999)
                              Stepcount = 999;
                        break;

                  case 3:
                        DelayCNT--;
                        if (DelayCNT < 0)
                              DelayCNT = 0;
                        break;

                  case 4:
                        DelayCNT++;
                        if (DelayCNT > 999)
                              DelayCNT = 999;
                        break;

                  case 5:
                        if (CNTFowardEn == 0 && CNTBackEn == 0)
                        {
                              CNTFowardEn = 1;
                        }
                        break;

                  case 6:
                        if (CNTFowardEn == 0 && CNTBackEn == 0)
                        {
                              CNTBackEn = 1;
                        }
                        break;

                  case 7:
                        CNTBackEn = 0;
                        CNTFowardEn = 0;
                        off();
                        break;

                  default:
                        break;
                  }
                  diSplayNumberCNT(DelayCNT, Stepcount);
                  diSplayStatCNT((CNTFowardEn || CNTBackEn), !(CNTFowardEn || CNTBackEn));
            }
            if (CNTFowardEn == 0 && CNTBackEn == 0)
            {
                  off();
            }
      }
}
static void vMotorFowardCNT(void *pvParameters)
{
      for (;;)
      {
            if (CNTFowardEn == 1)
            {

                  for (int16_t i = 0; i < Stepcount; ++i)
                  {
                        if (CNTFowardEn)
                              nextStepNoOff(DelayCNT);
                  }
                  vTaskDelay(100);
                  CNTFowardEn = 0;
                  diSplayStatCNT((CNTFowardEn, CNTBackEn), !(CNTFowardEn, CNTBackEn));
            }
            else
            {
                  vTaskDelay(1);
            }
      }
}
static void vMotorPrevCNT(void *pvParameters)
{
      for (;;)
      {
            if (CNTBackEn == 1)
            {
                  for (int16_t i = 0; i < Stepcount; ++i)
                  {
                        if (CNTBackEn)
                              prevStepNoOff(DelayCNT);
                  }
                  vTaskDelay(100);
                  CNTBackEn = 0;
                  diSplayStatCNT((CNTFowardEn, CNTBackEn), !(CNTFowardEn, CNTBackEn));
            }
            else
            {
                  vTaskDelay(1);
            }
      }
}
