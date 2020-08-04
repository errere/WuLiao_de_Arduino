#include "PT6311B.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <stdio.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

PT6311B P(4, 16, 17, 5); //uint8_t stby, uint8_t clk, uint8_t din, uint8_t dout

HTTPClient httpGetDMK;
HTTPClient httpNextDMK;

StaticJsonDocument<700> jsonBuffer;

const uint8_t regMap[8] = {0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x00, 0x15};

//config
const char *ssid = "HONOR_9X";
const char *password = "1234567890";
const char GetAdress[] = "http://123.56.221.173/GetDanmaku.php";
//const char GetAdress[] = "http://123.56.221.173/GetDanmakuErr.php";//error
const char NextAdress[] = "http://123.56.221.173/NextDanmaku.php";

//lcd Tips
const char loadStr[8] = "CONN-->";
const char OKStr[8] = "WIFI OK";
const char OSboot[8] = "OS BOOT";

const char DataBaseError[8] = "DB ERR "; //$e->Msg = "Error";
const char NODMK[8] = "NO DMK ";         //$e->Msg = "NoDanmaku";
const char DMKOKHOST[8] = "Get OK";      //$e->Msg = "OK";
const char DMKOK[8] = "DMK->  ";         //$e->Msg = "OK";

const char debugSTR[8] = "DEBUG->";

//Http Payload
//stringOne.length()
String DMK_Msg;
String DMK_Count;
String DMK_Host;
String DMK_Data;

//Display Buffer

char DMK_DispalyHOST[8][7] = {{0}};  //page,disp [0][x]is info//host max 42 Letter
char DMK_DispalyDATA[11][7] = {{0}}; //page,disp [0][x]is info//info max 70 Letter

char DMK_DispalyOnce[8] = {0};

uint8_t DMK_HostPages = 0;
uint8_t DMK_DATAPages = 0;

//Logic
volatile uint8_t haveWifi = 0;
volatile SemaphoreHandle_t Network_OK_Semaphore;
volatile SemaphoreHandle_t Danmaku_OK_Semaphore;

int16_t HttpGetResPublic = 0;
uint8_t ServErr = 0;

String TempStr;

char TempCh[80] = {0};

//func
void TaskLogicVFD(void *pvParameters);
void TaskNetWorkDMK(void *pvParameters);
void TaskNetWorkstatus(void *pvParameters);

int16_t GetServ();
void ResToVFD();

uint8_t SplitStringBySeven(String &str, char Dst[][7], uint8_t maxlen);

void VFD_PrintChar(char *ch, uint8_t len);
void VFD_PrintChar(const char *ch, uint8_t len);

void setup()
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  P.init(GR8SG20);

  delay(10);

  VFD_PrintChar(loadStr, 7);

  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoReconnect(true);
  haveWifi = 1;

  VFD_PrintChar(OKStr, 7);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
  VFD_PrintChar(OSboot, 7);

  Network_OK_Semaphore = xSemaphoreCreateBinary();
  Danmaku_OK_Semaphore = xSemaphoreCreateBinary();

  xTaskCreatePinnedToCore(TaskLogicVFD, "L", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskNetWorkDMK, "DMK", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskNetWorkstatus, "status", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
}

void TaskLogicVFD(void *pvParameters)
{
  (void)pvParameters;
  for (;;)
  {
    if (haveWifi)
    {
      if (xSemaphoreTake(Danmaku_OK_Semaphore, 0) == pdTRUE)
      {
        ResToVFD();
      } //xSemaphoreTake

      if (ServErr == 1)
      {
        snprintf(DMK_DispalyOnce, 8, "ERR %d", HttpGetResPublic);
        VFD_PrintChar(DMK_DispalyOnce, 7);
      } //ServErr
      else
      {
        /*===========================display Info start===========================*/
        if (DMK_HostPages != 0)
        {
          for (uint8_t i = 0; i <= DMK_HostPages; i++)
          {
            VFD_PrintChar(DMK_DispalyHOST[i], 7);
            vTaskDelay(1000);
          } //for

        } //DMK_HostPages != 0

        if (DMK_DATAPages != 0)
        {
          for (uint8_t i = 0; i <= DMK_DATAPages; i++)
          {
            VFD_PrintChar(DMK_DispalyDATA[i], 7);
            vTaskDelay(1000);
          } //for

        } //DMK_DATAPages != 0

        /*===========================display Info end===========================*/

      } //ServOK

    } //have wifi
    else
    {
      VFD_PrintChar(loadStr, 7);
    }

    vTaskDelay(100);
  } //for
}

void TaskNetWorkDMK(void *pvParameters)
{
  (void)pvParameters;

  for (;;)
  {
    if (xSemaphoreTake(Network_OK_Semaphore, 0) == pdTRUE)
    {

      if (haveWifi)
      {
        HttpGetResPublic = GetServ();
        if (HttpGetResPublic == 200)
        {
          ServErr = 0;
          Serial.print("DMK_Msg:");
          Serial.println(DMK_Msg);
          Serial.print("DMK_Count:");
          Serial.println(DMK_Count);
          Serial.print("DMK_Host:");
          Serial.println(DMK_Host);
          Serial.print("DMK_Data:");
          Serial.println(DMK_Data);

        } //200
        else
        {
          ServErr = 1;
        }

        xSemaphoreGive(Danmaku_OK_Semaphore);

      }               //haveWifi
    }                 //xSemaphoreTake
    vTaskDelay(5000); //5s
    xSemaphoreGive(Danmaku_OK_Semaphore);
    vTaskDelay(5000); //5s
    xSemaphoreGive(Danmaku_OK_Semaphore);
    vTaskDelay(5000); //5s
    xSemaphoreGive(Danmaku_OK_Semaphore);
    vTaskDelay(5000); //5s
    xSemaphoreGive(Danmaku_OK_Semaphore);
    vTaskDelay(5000); //5s
    xSemaphoreGive(Danmaku_OK_Semaphore);
    vTaskDelay(5000); //5s

  } //for
}

void TaskNetWorkstatus(void *pvParameters)
{
  (void)pvParameters;
  for (;;)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi Disconnected");
      xSemaphoreTake(Network_OK_Semaphore, ( TickType_t ) 1);
      haveWifi = 0;
    }
    else
    {
      xSemaphoreGive(Network_OK_Semaphore);
      vTaskDelay(100);
      haveWifi = 1;
    }
    vTaskDelay(1000);
  } //for
}

void loop()
{
  delay(1000);
}

int16_t GetServ()
{
  int16_t httpCode = 0;
  int16_t tempHttpCode = 0;

  httpGetDMK.begin(GetAdress);
  httpNextDMK.begin(NextAdress);
  httpCode = httpGetDMK.GET();
  if (httpCode > 0)
  {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK)
    {
      String payload = httpGetDMK.getString();
      Serial.println(payload);

      deserializeJson(jsonBuffer, payload);
      JsonObject object = jsonBuffer.as<JsonObject>();

      DMK_Msg = object["Msg"].as<String>();
      DMK_Count = object["Count"].as<String>();
      DMK_Host = object["Host"].as<String>();
      DMK_Data = object["Data"].as<String>();

    } //HTTP_CODE_OK
  }   //httpCode > 0
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", httpGetDMK.errorToString(httpCode).c_str());
  }

  httpGetDMK.end();

  //next dmk
  do
  {
    delay(10);
    tempHttpCode = httpNextDMK.GET();
  } while (tempHttpCode != HTTP_CODE_OK);
  httpNextDMK.end();

  return httpCode;
}

void ResToVFD()
{

  TempStr = "OK";
  if (DMK_Msg == TempStr)
  {
    for (uint8_t i = 0; i < 7; i++)
    {
      DMK_DispalyHOST[0][i] = DMKOKHOST[i];
    }
    for (uint8_t i = 0; i < 7; i++)
    {
      DMK_DispalyDATA[0][i] = DMKOK[i];
    }

    DMK_HostPages = SplitStringBySeven(DMK_Host, DMK_DispalyHOST, 42);
    DMK_DATAPages = SplitStringBySeven(DMK_Data, DMK_DispalyDATA, 70);
  } //ok

  TempStr = "NoDanmaku";
  if (DMK_Msg == TempStr)
  {
    for (uint8_t i = 0; i < 7; i++)
    {
      DMK_DispalyHOST[0][i] = NODMK[i];
      DMK_DispalyHOST[1][i] = 0x00;
    }
    
    DMK_HostPages = 1;
    DMK_DATAPages = 0;
  } //NoDanmaku

  TempStr = "Error";
  if (DMK_Msg == TempStr)
  {
    for (uint8_t i = 0; i < 7; i++)
    {
      DMK_DispalyHOST[0][i] = DataBaseError[i];
      DMK_DispalyHOST[1][i] = 0x00;
    }

    DMK_HostPages = 1;
    DMK_DATAPages = 0;
  } //Error

  // Serial.printf("MSglen:%d\n", DMK_Msg.length());
  // Serial.printf("Countlen:%d\n", DMK_Count.length());
  // Serial.printf("Hostlen:%d\n", DMK_Host.length());
  // Serial.printf("Datalen:%d\n\n\n", DMK_Data.length());
}

uint8_t SplitStringBySeven(String &str, char Dst[][7], uint8_t maxlen)
{
  uint16_t Len = 0;
  uint16_t NoFullPage = 0;
  uint16_t chCounter = 0;
  uint8_t Page = 0;

  Len = str.length();
  if (Len > maxlen)
    Len = maxlen;

  strncpy(TempCh, str.c_str(), Len);

  Page = Len / 7;
  NoFullPage = Len % 7;

  //full page
  for (uint8_t i = 1; i <= Page; i++)
  {
    for (uint8_t j = 0; j < 7; j++)
    {
      Dst[i][j] = TempCh[chCounter];
      chCounter++;

    } //j = 0; j < 7

  } //i = 0; i < Len

  //no full page
  if (NoFullPage != 0)
  {
    Page += 1;

    for (uint8_t i = 0; i < 7; i++)
    {
      Dst[Page][i] = 0;
    }

    for (uint8_t i = 0; i < NoFullPage; i++)
    {
      Dst[Page][i] = TempCh[chCounter];
      chCounter++;
    }

  } //NoFullPage != 0

  return Page;
}

void VFD_PrintChar(char *ch, uint8_t len)
{
  if (len > 7)
    len = 7;
  for (uint8_t i = 0; i < len; i++)
  {
    P.Send20Bits(regMap[i], P.AsciiToSegChar(ch[i]));
  }
}

void VFD_PrintChar(const char *ch, uint8_t len)
{
  if (len > 7)
    len = 7;
  for (uint8_t i = 0; i < len; i++)
  {
    P.Send20Bits(regMap[i], P.AsciiToSegChar(ch[i]));
  }
}
