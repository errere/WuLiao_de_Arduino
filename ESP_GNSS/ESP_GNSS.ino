#include <inttypes.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

#include "OledGnssRes.h"
#include "HtmlDoc.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define SDA 16
#define SCL 4

#define SW_1 25
#define SW_2 33
#define SW_3 26
#define SW_4 27

#define MODE_DISP 1
#define MODE_WIFI 0

#define GPSP_SIZE 18

#define GPRMC 1
#define GPVTG 2
#define GPGGA 3
#define GPGSA 4

/*====================================================class=======================================================*/
SSD1306Wire display(0x3c, SDA, SCL);
OLEDDisplayUi ui(&display);
WiFiServer server(80);

/*======================================================var=======================================================*/
/*=====================WIFI======================*/
const char *ssid = "EspGPS";
const char *password = "12345678";
IPAddress myIP;
WiFiClient client;

/*=====================RTOS======================*/
volatile SemaphoreHandle_t procSemaphore;
volatile SemaphoreHandle_t GNSSOKSemaphore;

/*=====================GNSS======================*/
uint8_t AllowRecv = 0;
String RawBuffer = "";
char RawBufferProc[255] = "";
char *gpsp[GPSP_SIZE];

const char LocStu[3][3] = {"NO", "2D", "3D"};

xxrmc_t GPRMC_Proc;
xxvtg_t GPVTG_Proc;
xxgga_t GPGGA_Proc;
xxgsa_t GPGSA_Proc;

xxrmc_t GPRMC_Disp;
xxvtg_t GPVTG_Disp;
xxgga_t GPGGA_Disp;
xxgsa_t GPGSA_Disp;

uint16_t NGmtYear;
uint8_t NGmtMonth;
uint8_t NGmtDay;
uint8_t NGmtHour;

/*=====================disp======================*/
char DisplayTempStr[50] = "";

/*======================sys======================*/
uint8_t SysMode = MODE_DISP;
int ADCsensorValue = 0;
float BATTvoltage = 0.0f;
uint8_t sysTemp = 0;
uint8_t sysTemp2 = 0;

const uint8_t MonD[12] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uint8_t MonDL[12] PROGMEM = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*=====================================================func======================================================*/
void TaskDisplay(void *pvParameters);

void TaskGnssGet(void *pvParameters);
void TaskGnssProc(void *pvParameters);

void TaskWifi(void *pvParameters);

void DecodeRMC(xxrmc_t *Dst, char **Buf);
void DecodeVTG(xxvtg_t *Dst, char **Buf);
void DecodeGGA(xxgga_t *Dst, char **Buf);
void DecodeGSA(xxgsa_t *Dst, char **Buf);

uint8_t GetKeyMax();

void WifiClentPrint();

inline uint8_t GetNmeaType(char *tht_buf);

uint8_t split_by_char(char *the_src, char the_char, char **the_des, uint8_t the_siz);
uint8_t split_by_comma(char *the_src, char **the_des, uint8_t the_siz);
uint8_t delete_crlf(char *the_buf);
uint8_t delete_star(char *the_buf);

uint8_t decToInt2(char *the_buf);
uint16_t decToInt(char *theBuf, uint8_t theSize);
uint8_t hexToInt2(char *the_buf);

uint8_t gpsCalcChecksum(char *array);
uint8_t gpsReadChecksumR(char *the_buf);

void RmcCpy(xxrmc_t *Dst, xxrmc_t Src);
void VtgCpy(xxvtg_t *Dst, xxvtg_t Src);
void GgaCpy(xxgga_t *Dst, xxgga_t Src);
void GsaCpy(xxgsa_t *Dst, xxgsa_t Src);

uint8_t IsLeap(uint16_t Y);
void addMon(uint16_t *DstYear, uint8_t *DstMon, uint16_t SrcYear, uint8_t SrcMon);
void addDay(uint16_t *DstYear, uint8_t *DstMon, uint8_t *DstDay, uint16_t SrcYear, uint8_t SrcMon, uint8_t SrcDay);
void timeZoneChange(uint16_t *DstYear, uint8_t *DstMon, uint8_t *DstDay, uint8_t *DstHour, uint16_t SrcYear, uint8_t SrcMon, uint8_t SrcDay, uint8_t SrcHour);

void printRMC(xxrmc_t Src);
void printVTG(xxvtg_t Src);
void printGGA(xxgga_t Src);
void printGSA(xxgsa_t Src);

/*====================================================ui==========================================================*/
void gsOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{

  display->setFont(ArialMT_Plain_10);

  sprintf(DisplayTempStr, "SN:%d  STU:%s", GPGGA_Disp.S_Num, LocStu[GPGSA_Disp.Type - 1]);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 0, String(DisplayTempStr));

  sprintf(DisplayTempStr, "BAT:%.2fV", BATTvoltage);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128, 0, String(DisplayTempStr));
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  timeZoneChange(&NGmtYear, &NGmtMonth, &NGmtDay, &NGmtHour, (GPRMC_Disp.UTC_Year + 2000), GPRMC_Disp.UTC_Month, GPRMC_Disp.UTC_Day, GPRMC_Disp.UTC_Hour);

  // sprintf(DisplayTempStr, "%d - %d - %d  GMT+8: %d - %d - %d", (GPRMC_Disp.UTC_Year + 2000), GPRMC_Disp.UTC_Month, GPRMC_Disp.UTC_Day, NGmtYear, NGmtMonth, NGmtDay);
  // display->drawString(0 + x, 13 + y, String(DisplayTempStr));

  // sprintf(DisplayTempStr, "%d: %d : %d  GMT+8: %d : %d : %d", GPRMC_Disp.UTC_Hour, GPRMC_Disp.UTC_Minute, GPRMC_Disp.UTC_Second, NGmtHour, GPRMC_Disp.UTC_Minute, GPRMC_Disp.UTC_Second);
  // display->drawString(0 + x, 23 + y, String(DisplayTempStr));
  sprintf(DisplayTempStr, "GMT+8 : %d - %d - %d", NGmtYear, NGmtMonth, NGmtDay);
  display->drawString(0 + x, 13 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "GMT+8 : %d : %d : %d", NGmtHour, GPRMC_Disp.UTC_Minute, GPRMC_Disp.UTC_Second);
  display->drawString(0 + x, 23 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Lat : %f %c", GPRMC_Disp.Latitude, GPRMC_Disp.Ns);
  display->drawString(0 + x, 33 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Lon : %f %c", GPRMC_Disp.Longitude, GPRMC_Disp.Ew);
  display->drawString(0 + x, 43 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Velo : %.3f  %.2f°", GPRMC_Disp.Velocity, GPRMC_Disp.Azimuth);
  display->drawString(0 + x, 53 + y, String(DisplayTempStr));
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  sprintf(DisplayTempStr, "Velo.T : %.3f %c", GPVTG_Disp.T_Angle, GPVTG_Disp.T_Reference);
  display->drawString(0 + x, 13 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Velo.M : %.3f %c", GPVTG_Disp.M_Angle, GPVTG_Disp.M_Reference);
  display->drawString(0 + x, 23 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "H.Speed(Knot) : %.2f", GPVTG_Disp.N_Horizontal_Velocity);
  display->drawString(0 + x, 33 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "H.Speed(Km/h) : %.2f", GPVTG_Disp.K_Horizontal_Velocity);
  display->drawString(0 + x, 43 + y, String(DisplayTempStr));
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  sprintf(DisplayTempStr, "Lat : %f %c", GPGGA_Disp.Latitude, GPGGA_Disp.Ns);
  display->drawString(0 + x, 13 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Lon : %f %c", GPGGA_Disp.Longitude, GPGGA_Disp.Ew);
  display->drawString(0 + x, 23 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Alt : %f %c", GPGGA_Disp.Altitude, GPGGA_Disp.Metre);
  display->drawString(0 + x, 33 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "Lvl : %f %c", GPGGA_Disp.Level, GPGGA_Disp.Level_Unit);
  display->drawString(0 + x, 43 + y, String(DisplayTempStr));
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  sprintf(DisplayTempStr, "%d  %d  %d  %d", GPGSA_Disp.Sno[0], GPGSA_Disp.Sno[1], GPGSA_Disp.Sno[2], GPGSA_Disp.Sno[3]);
  display->drawString(0 + x, 13 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "%d  %d  %d  %d", GPGSA_Disp.Sno[4], GPGSA_Disp.Sno[5], GPGSA_Disp.Sno[6], GPGSA_Disp.Sno[7]);
  display->drawString(0 + x, 23 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "%d  %d  %d  %d", GPGSA_Disp.Sno[8], GPGSA_Disp.Sno[9], GPGSA_Disp.Sno[10], GPGSA_Disp.Sno[11]);
  display->drawString(0 + x, 33 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "PDOP: %.1f HDOP: %.1f", GPGSA_Disp.PDOP, GPGSA_Disp.HDOP);
  display->drawString(0 + x, 43 + y, String(DisplayTempStr));

  sprintf(DisplayTempStr, "       VDOP : %.1f", GPGSA_Disp.VDOP);
  display->drawString(0 + x, 53 + y, String(DisplayTempStr));
}

void DrawWiFiInfo()
{

  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Server started");
  display.drawString(0, 10, "Server IPadress:");

  display.drawString(0, 20, myIP.toString());

  sprintf(DisplayTempStr, "SSID: %s", ssid);
  display.drawString(0, 30, DisplayTempStr);

  sprintf(DisplayTempStr, "PWD: %s", password);
  display.drawString(0, 40, DisplayTempStr);

  display.display();
}
/*================================================UIconfig======================================================*/

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = {drawFrame1, drawFrame2, drawFrame3, drawFrame4};
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = {gsOverlay};
int overlaysCount = 1;

/*===================================================init========================================================*/
inline void HWinit()
{
  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);
  pinMode(SW_3, INPUT_PULLUP);
  pinMode(SW_4, INPUT_PULLUP);
  /*
  Serial init
  baud,config,rx,tx
  */
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 18, 19); //gnss
  Serial1.setRxBufferSize(1024);
}

inline void UIinit()
{
  ui.setTargetFPS(30);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();
}

inline void WiFIinit()
{
  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");
}

/*===================================================main========================================================*/
void setup()
{
  HWinit();

  delay(10);

  if (digitalRead(SW_1) == 0)
  {
    delay(50);
    if (digitalRead(SW_1) == 0)
    {
      SysMode = MODE_WIFI;
    }
  }

  if (SysMode == MODE_DISP)
  {
    UIinit();
  }
  else if (SysMode == MODE_WIFI)
  {
    WiFIinit();

    display.init();
    display.flipScreenVertically();
    DrawWiFiInfo();
  }

  procSemaphore = xSemaphoreCreateBinary();

  GNSSOKSemaphore = xSemaphoreCreateMutex();
  xSemaphoreGive(GNSSOKSemaphore);

  for (uint8_t i = 0; i < GPSP_SIZE; i++)
  {
    gpsp[i] = (char *)malloc(50);
  }

  xTaskCreatePinnedToCore(TaskGnssGet, "TaskGnssGet", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskGnssProc, "TaskGnssProc", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);

  if (SysMode == MODE_DISP)
  {
    xTaskCreatePinnedToCore(TaskDisplay, "TaskDisplay", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
  }
  else if (SysMode == MODE_WIFI)
  {
    xTaskCreatePinnedToCore(TaskWifi, "TaskWifi", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
  }
}

void loop()
{
  delay(100000);
}

/*===================================================task========================================================*/
void TaskDisplay(void *pvParameters) // This is a task.
{
  (void)pvParameters;
  for (;;) // A Task shall never return or exit.
  {
    if (xSemaphoreTake(GNSSOKSemaphore, 0) == pdTRUE)
    {
      RmcCpy(&GPRMC_Disp, GPRMC_Proc);
      VtgCpy(&GPVTG_Disp, GPVTG_Proc);
      GgaCpy(&GPGGA_Disp, GPGGA_Proc);
      GsaCpy(&GPGSA_Disp, GPGSA_Proc);

      xSemaphoreGive(GNSSOKSemaphore);
    } //xSemaphoreTake(GNSSOKSemaphore, 0) == pdTRUE

    ADCsensorValue = analogRead(14);
    BATTvoltage = ADCsensorValue * (6.6 / 3750.0);

    uint8_t inputSW = GetKeyMax();
    switch (inputSW)
    {
    case 1:
      ui.switchToFrame(0);
      break;
    case 2:
      ui.switchToFrame(1);
      break;
    case 3:
      ui.switchToFrame(2);
      break;
    case 4:
      ui.switchToFrame(3);
      break;

    default:
      break;
    }

    int remainingTimeBudget = ui.update();
    if (remainingTimeBudget > 0)
    {
      vTaskDelay(remainingTimeBudget);
    }
    vTaskDelay(1);
  }
}

void TaskGnssGet(void *pvParameters) // This is a task.
{
  char tempCh = 0;
  (void)pvParameters;
  for (;;) // A Task shall never return or exit.
  {
    if (Serial1.available())
    {
      tempCh = Serial1.read();

      if (tempCh == '$')
      {
        AllowRecv = 1;
      } //tempCh == '$'

      if (AllowRecv == 1)
      {
        RawBuffer += tempCh;
        if (tempCh == '\n')
        {
          AllowRecv = 0;

          strcpy(RawBufferProc, RawBuffer.c_str());

          RawBuffer = "";
          xSemaphoreGive(procSemaphore);
        } //tempCh == '\n'

      } //AllowRecv == 1

    } //Serial1.available
    vTaskDelay(1);
  } //for
}

void TaskGnssProc(void *pvParameters) // This is a task.
{
  (void)pvParameters;
  for (;;) // A Task shall never return or exit.
  {
    if (xSemaphoreTake(procSemaphore, 0) == pdTRUE)
    {

      //GPRMC,GPVTG,GPGGA,GPGSA,GPGSV,GPGLL
      if (strncmp(RawBufferProc + 1, "GPRMC", 5) == 0 || strncmp(RawBufferProc + 1, "GPVTG", 5) == 0 || strncmp(RawBufferProc + 1, "GPGGA", 5) == 0 || strncmp(RawBufferProc + 1, "GPGSA", 5) == 0)
      {

        //Serial.print(RawBufferProc);

        uint8_t NmeaType = GetNmeaType(RawBufferProc);
        //Serial.printf("Type = %d\r\n", NmeaType);

        delete_crlf(RawBufferProc);
        uint8_t CalcSum = gpsCalcChecksum(RawBufferProc);
        uint8_t ReadSum = gpsReadChecksumR(RawBufferProc);
        uint8_t wordNum = split_by_comma(RawBufferProc, gpsp, sizeof(gpsp) / sizeof(char *));
        bool check_result = (CalcSum == ReadSum);

        //Serial.printf("CSum = %d,Rsum = %d --> check_result is %d\r\n", CalcSum, ReadSum, check_result);

        for (int i = 0; i < wordNum; i++)
        {
          delete_star(gpsp[i]);
          //Serial.print(i);
          //Serial.print(": ");
          //Serial.println(gpsp[i]);
        }
        if (check_result == 1)
        {

          if (xSemaphoreTake(GNSSOKSemaphore, 0) == pdTRUE)
          {

            switch (NmeaType)
            {
            case GPRMC:
              DecodeRMC(&GPRMC_Proc, gpsp);
              //printRMC(GPRMC_Proc);
              break;
            case GPVTG:
              DecodeVTG(&GPVTG_Proc, gpsp);
              //printVTG(GPVTG_Proc);
              break;
            case GPGGA:
              DecodeGGA(&GPGGA_Proc, gpsp);
              //printGGA(GPGGA_Proc);
              break;
            case GPGSA:
              DecodeGSA(&GPGSA_Proc, gpsp);
              //printGSA(GPGSA_Proc);
              break;

            default:
              //Serial.println("BUF_ERR");
              break;
            } //switch

            xSemaphoreGive(GNSSOKSemaphore);

          } //xSemaphoreTake(GNSSOKSemaphore, 0) == pdTRUE

        } //check_result == 1

      } //GPRMC,GPVTG,GPGGA,GPGSA,GPGSV,GPGLL

      //Serial.print("");
    }
    vTaskDelay(1);
  }
}

void TaskWifi(void *pvParameters) // This is a task.
{

  char c;
  uint8_t DispFlag = 1;

  String currentLine = "";

  (void)pvParameters;
  for (;;) // A Task shall never return or exit.
  {

    if (digitalRead(SW_1) == 0)
    {
      delay(100);
      if (digitalRead(SW_1) == 0)
      {
        if (DispFlag == 0)
        {
          DispFlag = 1;
          DrawWiFiInfo();
        }
      }
    }

    else
    {
      if (DispFlag == 1)
      {
        DispFlag = 0;
        display.clear();
        display.display();
      }
    }

    if (xSemaphoreTake(GNSSOKSemaphore, 0) == pdTRUE)
    {
      RmcCpy(&GPRMC_Disp, GPRMC_Proc);
      VtgCpy(&GPVTG_Disp, GPVTG_Proc);
      GgaCpy(&GPGGA_Disp, GPGGA_Proc);
      GsaCpy(&GPGSA_Disp, GPGSA_Proc);

      xSemaphoreGive(GNSSOKSemaphore);
    } //xSemaphoreTake(GNSSOKSemaphore, 0) == pdTRUE

    ADCsensorValue = analogRead(14);
    BATTvoltage = ADCsensorValue * (6.6 / 3750.0);

    client = server.available(); // listen for incoming clients

    if (client)
    {                                // if you get a client,
      Serial.println("New Client."); // print a message out the serial port
      currentLine = "";              // make a String to hold incoming data from the client

      while (client.connected())
      {

        vTaskDelay(1);

        if (client.available())
        {
          c = client.read();

          if (c == '\n')
          {

            if (currentLine.length() == 0)
            {

              WifiClentPrint();

              break;
            }
            else
            { // if you got a newline, then clear currentLine:
              currentLine = "";
            }

          } //\n

        } //available

      } //while

      // close the connection:
      client.stop();
      Serial.println("Client Disconnected.");

    } //client

    vTaskDelay(1);
  } //for
}

/*==================================================logic========================================================*/
void DecodeRMC(xxrmc_t *Dst, char **Buf)
{
  double temp = 0.0;

  // GPRMC
  // （推荐定位信息数据格式）
  // 例：$GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50

  // 字段1：UTC时间，hhmmss.sss格式
  if (Buf[1][0])
  {
    Dst->UTC_Hour = decToInt2(Buf[1]);
    Dst->UTC_Minute = decToInt2(Buf[1] + 2);
    Dst->UTC_Second = decToInt2(Buf[1] + 4);
  }
  else
  {
    Dst->UTC_Hour = 0;
    Dst->UTC_Minute = 0;
    Dst->UTC_Second = 0;
  }

  // 字段2：状态，A=定位，V=未定位
  Dst->State = Buf[2][0];

  // 字段3：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
  if (Buf[3][0])
  {
    temp = (double)decToInt(gpsp[3], 2);
    Dst->Latitude = temp + atof(gpsp[3] + 2) / 60.0;
  }
  else
    Dst->Latitude = 0.0;

  // 字段4：纬度N（北纬）或S（南纬）
  if (Buf[4][0])
    Dst->Ns = Buf[4][0];
  else
    Dst->Ns = 0;

  // 字段5：经度dddmm.mmmm，度分格式（前导位数不足则补0）
  if (Buf[5][0])
  {
    temp = (double)decToInt(gpsp[5], 3);
    Dst->Longitude = temp + atof(gpsp[5] + 3) / 60.0;
  }
  else
    Dst->Longitude = 0.0;

  // 字段6：经度E（东经）或W（西经）
  if (Buf[6][0])
    Dst->Ew = Buf[6][0];
  else
    Dst->Ew = 0;

  // 字段7：速度，节，Knots
  if (Buf[7][0])
    Dst->Velocity = atof(Buf[7]);
  else
    Dst->Velocity = 0.0;

  // 字段8：方位角，度
  if (Buf[8][0])
    Dst->Azimuth = atof(Buf[8]);
  else
    Dst->Azimuth = 0.0;

  // 字段9：UTC日期，DDMMYY格式
  if (Buf[9][0])
  {
    Dst->UTC_Day = decToInt2(Buf[9]);
    Dst->UTC_Month = decToInt2(Buf[9] + 2);
    Dst->UTC_Year = decToInt2(Buf[9] + 4);
  }
  else
  {
    Dst->UTC_Day = 0;
    Dst->UTC_Month = 0;
    Dst->UTC_Year = 0;
  }

  // 字段10：磁偏角，（000 - 180）度（前导位数不足则补0）
  if (Buf[10][0])
    Dst->Declination = atof(Buf[10]);
  else
    Dst->Declination = 0;

  // 字段11：磁偏角方向，E=东W=西
  if (Buf[11][0])
    Dst->Dew = Buf[11][0];
  else
    Dst->Dew = 0;

  // 字段12：模式，A=自动，D=差分，E=估测，N=数据无效（3.0协议内容）
  Dst->Mode = Buf[12][0];
}

void DecodeVTG(xxvtg_t *Dst, char **Buf)
{

  //   GPVTG
  // （地面速度信息）
  // 例：$GPVTG,89.68,T,,M,0.00,N,0.0,K*5F

  // 字段1：运动角度，000 - 359，（前导位数不足则补0）
  if (Buf[1][0])
    Dst->T_Angle = atof(Buf[1]);
  else
    Dst->T_Angle = 0;

  // 字段2：T=真北参照系
  if (Buf[2][0])
    Dst->T_Reference = Buf[2][0];
  else
    Dst->T_Reference = 0;

  // 字段3：运动角度，000 - 359，（前导位数不足则补0）
  if (Buf[3][0])
    Dst->M_Angle = atof(Buf[3]);
  else
    Dst->M_Angle = 0;

  // 字段4：M=磁北参照系
  if (Buf[4][0])
    Dst->M_Reference = Buf[4][0];
  else
    Dst->M_Reference = 0;

  // 字段5：水平运动速度（0.00）（前导位数不足则补0）
  if (Buf[5][0])
    Dst->N_Horizontal_Velocity = atof(Buf[5]);
  else
    Dst->N_Horizontal_Velocity = 0;

  // 字段6：N=节，Knots
  if (Buf[6][0])
    Dst->Knots = Buf[6][0];
  else
    Dst->Knots = 0;

  // 字段7：水平运动速度（0.00）（前导位数不足则补0）
  if (Buf[7][0])
    Dst->K_Horizontal_Velocity = atof(Buf[7]);
  else
    Dst->K_Horizontal_Velocity = 0;

  // 字段8：K=公里/时，km/h
  if (Buf[8][0])
    Dst->Kmh = Buf[8][0];
  else
    Dst->Kmh = 0;
}

void DecodeGGA(xxgga_t *Dst, char **Buf)
{
  double temp = 0.0;

  //   GPGGA
  // （定位信息）
  // 例：$GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,12.2,M,19.7,M,,0000*1F
  // 字段0：$GPGGA，语句ID，表明该语句为Global Positioning System Fix Data（GGA）GPS定位信息

  // 字段1：UTC 时间，hhmmss.sss，时分秒格式
  if (Buf[1][0])
  {
    Dst->UTC_Hour = decToInt2(Buf[1]);
    Dst->UTC_Minute = decToInt2(Buf[1] + 2);
    Dst->UTC_Second = decToInt2(Buf[1] + 4);
  }
  else
  {
    Dst->UTC_Hour = 0;
    Dst->UTC_Minute = 0;
    Dst->UTC_Second = 0;
  }

  // 字段2：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
  if (Buf[2][0])
  {
    temp = (double)decToInt(gpsp[2], 2);
    Dst->Latitude = temp + atof(gpsp[2] + 2) / 60.0;
  }
  else
    Dst->Latitude = 0;

  // 字段3：纬度N（北纬）或S（南纬）
  if (Buf[3][0])
    Dst->Ns = Buf[3][0];
  else
    Dst->Ns = 0;

  // 字段4：经度dddmm.mmmm，度分格式（前导位数不足则补0）
  if (Buf[4][0])
  {
    temp = (double)decToInt(gpsp[4], 3);
    Dst->Longitude = temp + atof(gpsp[4] + 3) / 60.0;
  }
  else
    Dst->Longitude = 0.0;

  // 字段5：经度E（东经）或W（西经）
  if (Buf[5][0])
    Dst->Ew = Buf[5][0];
  else
    Dst->Ew = 0;

  // 字段6：GPS状态，0=不可用(FIX NOT valid)，1=单点定位(GPS FIX)，2=差分定位(DGPS)，3=无效PPS，4=实时差分定位（RTK FIX），5=RTK FLOAT，6=正在估算
  Dst->Fix = Buf[6][0] - '0';

  // 字段7：正在使用的卫星数量（00 - 12）（前导位数不足则补0）
  Dst->S_Num = atoi(Buf[7]);

  // 字段8：HDOP水平精度因子（0.5 - 99.9）
  Dst->HDOP = atof(Buf[8]);

  // 字段9：海拔高度（-9999.9 - 99999.9）
  if (Buf[9][0])
    Dst->Altitude = atof(Buf[9]);
  else
    Dst->Altitude = 0;

  // 字段10：单位：M（米）
  Dst->Metre = 'M';

  // 字段11：地球椭球面相对大地水准面的高度 WGS84水准面划分
  if (Buf[11][0])
    Dst->Level = atof(Buf[11]);
  else
    Dst->Level = 0;

  // 字段12：WGS84水准面划分单位：M（米）
  if (Buf[12][0])
    Dst->Level_Unit = Buf[12][0];
  else
    Dst->Level_Unit = 0;

  // 字段13：差分时间（从接收到差分信号开始的秒数，如果不是差分定位将为空）(dont have)
  Dst->Diff_Time = 0.0;

  // 字段14：差分站ID号0000 - 1023（前导位数不足则补0，如果不是差分定位将为空）
  Dst->Diff_ID = 0;

} //DecodeGGA

void DecodeGSA(xxgsa_t *Dst, char **Buf)
{

  //   GPGSA
  // （ 当前卫星信息）
  // 例：$GPGSA,A,3,01,20,19,13,,,,,,,,,40.4,24.4,32.2*0A

  // 字段1：定位模式(选择2D/3D)，A=自动选择，M=手动选择
  Dst->Mode = Buf[1][0];

  // 字段2：定位类型，1=未定位，2=2D定位，3=3D定位
  Dst->Type = Buf[2][0] - '0';

  // 字段3：PRN码（伪随机噪声码），第1信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
  for (uint8_t i = 3; i <= 14; i++)
  {
    if (Buf[i][0])
      Dst->Sno[i - 3] = atoi(Buf[i]);
    else
      Dst->Sno[i - 3] = 0;
  }

  //字段14：PRN码（伪随机噪声码），第12信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）

  // 字段15：PDOP综合位置精度因子（0.5 - 99.9）
  Dst->PDOP = atof(Buf[15]);

  // 字段16：HDOP水平精度因子（0.5 - 99.9）
  Dst->HDOP = atof(Buf[16]);

  // 字段17：VDOP垂直精度因子（0.5 - 99.9）
  Dst->VDOP = atof(Buf[17]);
}

uint8_t GetKeyMax()
{
  if (digitalRead(SW_4) == 0)
    return 4;
  if (digitalRead(SW_3) == 0)
    return 3;
  if (digitalRead(SW_2) == 0)
    return 2;
  if (digitalRead(SW_1) == 0)
    return 1;
  return 0;
}

void WifiClentPrint()
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("<!DOCTYPE html>");
  client.print("<html>");

  client.print("<head>");

  client.print("<meta charset=\"utf-8\">");
  client.print("<meta http-equiv=\"refresh\" content=\"1\"> "); //其中1指每隔1秒刷新一次页面
  //定位类型，1=未定位，2=2D定位，3=3D定位
  client.printf("<title>EspGPS %s</title>", HD_GPS_TYP[GPGSA_Disp.Type]);

  client.print("</head>");

  client.print("<body>");

  //模式，A=自动，D=差分，E=估测，N=数据无效（3.0协议内容）
  if (GPRMC_Disp.Mode == 'A')
  {
    sysTemp = 1;
  }
  else if (GPRMC_Disp.Mode == 'D')
  {
    sysTemp = 2;
  }
  else if (GPRMC_Disp.Mode == 'E')
  {
    sysTemp = 3;
  }
  else if (GPRMC_Disp.Mode == 'N')
  {
    sysTemp = 4;
  }
  else
  {
    sysTemp = 0;
  }
  client.printf("<h1>定位模式 : %s,%s</h1>", HD_GPS_LOC_MOD[sysTemp], HD_GPS_TYP[GPGSA_Disp.Type]);

  //GPS状态，0=不可用(FIX NOT valid)，1=单点定位(GPS FIX)，2=差分定位(DGPS)，
  // 3=无效PPS，4=实时差分定位（RTK FIX），5=RTK FLOAT，6=正在估算
  client.printf("<h2>定位状态 : %s</h2>", HD_GPS_STU[GPGGA_Disp.Fix]);

  //定位模式(选择2D/3D)，A=自动选择，M=手动选择
  if (GPGSA_Disp.Mode == 'A')
  {
    sysTemp = 1;
  }
  else if (GPGSA_Disp.Mode == 'M')
  {
    sysTemp = 2;
  }
  else
  {
    sysTemp = 0;
  }
  client.printf("<h3>定位模式选择 : %s</h3>", HD_GPS_MOD[sysTemp]);

  //UTC 时间，hhmmss.sss，时分秒格式
  //UTC日期，DDMMYY格式
  client.printf("<h3>UTC : %d - %d - %d , %d : %d : %d</h3>", (GPRMC_Disp.UTC_Year + 2000), GPRMC_Disp.UTC_Month, GPRMC_Disp.UTC_Day, GPRMC_Disp.UTC_Hour, GPRMC_Disp.UTC_Minute, GPRMC_Disp.UTC_Second);

  timeZoneChange(&NGmtYear, &NGmtMonth, &NGmtDay, &NGmtHour, (GPRMC_Disp.UTC_Year + 2000), GPRMC_Disp.UTC_Month, GPRMC_Disp.UTC_Day, GPRMC_Disp.UTC_Hour);

  client.printf("<h3>GMT+8 : %d - %d - %d , %d : %d : %d</h3>", NGmtYear, NGmtMonth, NGmtDay, NGmtHour, GPRMC_Disp.UTC_Minute, GPRMC_Disp.UTC_Second);

  //纬度ddmm.mmmm，度分格式（前导位数不足则补0）
  //纬度N（北纬）或S（南纬）
  //经度dddmm.mmmm，度分格式（前导位数不足则补0）
  //经度E（东经）或W（西经）
  if (GPRMC_Disp.Ns == 'N')
  {
    sysTemp = 1;
  }
  else if (GPRMC_Disp.Ns == 'S')
  {
    sysTemp = 2;
  }
  else
  {
    sysTemp = 0;
  }
  if (GPRMC_Disp.Ew == 'E')
  {
    sysTemp2 = 3;
  }
  else if (GPRMC_Disp.Ew == 'W')
  {
    sysTemp2 = 4;
  }
  else
  {
    sysTemp = 0;
  }
  client.printf("<p>纬度 :%s  %f° , 经度 : %s  %f°</p>", HD_GPS_LOC_LALO[sysTemp], GPRMC_Disp.Latitude, HD_GPS_LOC_LALO[sysTemp2], GPRMC_Disp.Longitude);

  //海拔高度（-9999.9 - 99999.9）
  //单位：M（米）
  client.printf("<p>海拔 : %f 米</p>", GPGGA_Disp.Altitude);

  //磁偏角，（000 - 180）度（前导位数不足则补0）
  //磁偏角方向，E=东W=西
  if (GPRMC_Disp.Dew == 'E')
  {
    sysTemp = 1;
  }
  else if (GPRMC_Disp.Dew == 'W')
  {
    sysTemp = 2;
  }
  else
  {
    sysTemp = 0;
  }
  client.printf("<p>磁偏角 : %s %f°</p>", HD_GPS_LOC_CPJ[sysTemp], GPRMC_Disp.Declination);

  //地球椭球面相对大地水准面的高度 WGS84水准面划分
  client.printf("<p>地球椭球面相对大地水准面的高度 : %f 米</p>", GPGGA_Disp.Level);

  //     运动角度，000 - 359，（前导位数不足则补0）
  //     T=真北参照系
  client.printf("<p>真北参照系的运动角度 : %f °</p>", GPVTG_Disp.T_Angle);

  //     运动角度，000 - 359，（前导位数不足则补0）
  //     M=磁北参照系
  client.printf("<p>磁北参照系的运动角度 : %f °</p>", GPVTG_Disp.M_Angle);

  //     水平运动速度（0.00）（前导位数不足则补0）
  //     N=节，Knots
  client.printf("<p>水平运动速度(节) : %f </p>", GPVTG_Disp.N_Horizontal_Velocity);

  //     水平运动速度（0.00）（前导位数不足则补0）
  //     K=公里/时，km/h
  client.printf("<p>水平运动速度(公里/时) : %f </p>", GPVTG_Disp.K_Horizontal_Velocity);

  //     PDOP综合位置精度因子（0.5 - 99.9）
  client.printf("<p>PDOP综合位置精度因子 :%f </p>", GPGSA_Disp.PDOP);

  //     HDOP水平精度因子（0.5 - 99.9）
  client.printf("<p>HDOP水平精度因子 : %f </p>", GPGSA_Disp.HDOP);

  //     VDOP垂直精度因子（0.5 - 99.9）
  client.printf("<p>VDOP垂直精度因子 : %f </p>", GPGSA_Disp.VDOP);

  client.printf("<p>正在使用的卫星数量 : %d </p>", GPGGA_Disp.S_Num);

  client.printf("<p>电池电压 : %.2f V</p>", BATTvoltage / 2.0f);

  client.print("<table border=\"2\">");
  client.print("<caption>信道卫星PRN码编号</caption>");
  client.print("<tr>");
  client.print("<th>卫星</th>");
  client.print("<th>PRN</th>");
  client.print("</tr>");

  for (uint8_t i = 0; i < GPGGA_Disp.S_Num; i++)
  {
    client.print("<tr>");
    client.printf("<td>%d</td>", i);
    client.printf("<td>%d</td>", GPGSA_Disp.Sno[i]);
    client.print("</tr>");
    /* code */
  }

  client.print("</body>");

  client.print("</html>");

  // The HTTP response ends with another blank line:
  client.println();
  // break out of the while loop:
}

/*=================================================tools=========================================================*/
inline uint8_t GetNmeaType(char *tht_buf)
{
  if (strncmp(tht_buf + 1, "GPRMC", 5) == 0)
  {
    return GPRMC;
  }
  if (strncmp(tht_buf + 1, "GPVTG", 5) == 0)
  {
    return GPVTG;
  }
  if (strncmp(tht_buf + 1, "GPGGA", 5) == 0)
  {
    return GPGGA;
  }
  if (strncmp(tht_buf + 1, "GPGSA", 5) == 0)
  {
    return GPGSA;
  }
  return 0xff;
}

uint8_t split_by_char(char *the_src, char the_char, char **the_des, uint8_t the_siz)
{
  uint8_t src_len = strlen(the_src);
  uint8_t di = 0;
  the_des[di++] = the_src;
  for (uint8_t si = 0; si < src_len && di < the_siz; si++)
  {
    if (the_src[si] == the_char)
    {
      the_des[di++] = the_src + si + 1;
      the_src[si] = '\0';
    }
  }
  return di;
}

uint8_t split_by_comma(char *the_src, char **the_des, uint8_t the_siz)
{
  return split_by_char(the_src, ',', the_des, the_siz);
}

uint8_t delete_crlf(char *the_buf)
{
  uint8_t leng = strlen(the_buf);
  for (uint8_t i = 0; i < leng - 1; i++)
  {
    if (the_buf[i] == '\r' && the_buf[i + 1] == '\n')
    {
      the_buf[i] = '\0';
      return 1;
    }
  }
  return 0;
}

uint8_t delete_star(char *the_buf)
{
  uint8_t leng = strlen(the_buf);
  for (uint8_t i = 0; i < leng - 1; i++)
  {
    if (the_buf[i] == '*')
    {
      the_buf[i] = '\0';
      return 1;
    }
  }
  return 0;
}

uint8_t decToInt2(char *the_buf)
{
  uint8_t value = 0;
  value += (the_buf[0] - '0') * 10;
  value += (the_buf[1] - '0');
  return value;
}

uint16_t decToInt(char *theBuf, uint8_t theSize)
{
  uint16_t value = 0;
  for (int i = 0; i < theSize; i++)
  {
    value *= 10;
    value += (theBuf[i] - '0');
  }
  return value;
}

uint8_t hexToInt2(char *the_buf)
{
  uint8_t value = 0;
  if (the_buf[0] >= '0' && the_buf[0] <= '9')
  {
    value += (the_buf[0] - '0') * 16;
  }
  else
  {
    value += (the_buf[0] - 'A' + 10) * 16;
  }

  if (the_buf[1] >= '0' && the_buf[1] <= '9')
  {
    value += (the_buf[1] - '0');
  }
  else
  {
    value += (the_buf[1] - 'A' + 10);
  }
  return value;
}

// check CalcSum using xor
uint8_t gpsCalcChecksum(char *array)
{
  uint8_t CalcSum = array[1];
  for (uint8_t i = 2; array[i] != '*'; i++)
  {
    CalcSum ^= array[i];
  }
  return CalcSum;
}

//get gga checksum
uint8_t gpsReadChecksumR(char *the_buf)
{
  uint8_t leng = strlen(the_buf);
  uint8_t i = 0;

  for (i = leng - 1; i < 0; i--)
  {
    if (the_buf[i] == '*')
      break;
  }

  if (i < 6)
    return 0;

  //Serial.printf("RSS = %s\r\n", the_buf + i - 1);

  uint8_t CalcSum = hexToInt2(the_buf + i - 1);
  return CalcSum;
}

void RmcCpy(xxrmc_t *Dst, xxrmc_t Src)
{
  Dst->UTC_Hour = Src.UTC_Hour;
  Dst->UTC_Minute = Src.UTC_Minute;
  Dst->UTC_Second = Src.UTC_Second;
  Dst->State = Src.State;
  Dst->Latitude = Src.Latitude;
  Dst->Ns = Src.Ns;
  Dst->Longitude = Src.Longitude;
  Dst->Ew = Src.Ew;
  Dst->Velocity = Src.Velocity;
  Dst->Azimuth = Src.Azimuth;
  Dst->UTC_Day = Src.UTC_Day;
  Dst->UTC_Month = Src.UTC_Month;
  Dst->UTC_Year = Src.UTC_Year;
  Dst->Declination = Src.Declination;
  Dst->Dew = Src.Dew;
  Dst->Mode = Src.Mode;
}

void VtgCpy(xxvtg_t *Dst, xxvtg_t Src)
{
  Dst->T_Angle = Src.T_Angle;
  Dst->T_Reference = Src.T_Reference;
  Dst->M_Angle = Src.M_Angle;
  Dst->M_Reference = Src.M_Reference;
  Dst->N_Horizontal_Velocity = Src.N_Horizontal_Velocity;
  Dst->Knots = Src.Knots;
  Dst->K_Horizontal_Velocity = Src.K_Horizontal_Velocity;
  Dst->Kmh = Src.Kmh;
}

void GgaCpy(xxgga_t *Dst, xxgga_t Src)
{
  Dst->UTC_Hour = Src.UTC_Hour;
  Dst->UTC_Minute = Src.UTC_Minute;
  Dst->UTC_Second = Src.UTC_Second;
  Dst->Latitude = Src.Latitude;
  Dst->Ns = Src.Ns;
  Dst->Longitude = Src.Longitude;
  Dst->Ew = Src.Ew;
  Dst->Fix = Src.Fix;
  Dst->S_Num = Src.S_Num;
  Dst->HDOP = Src.HDOP;
  Dst->Altitude = Src.Altitude;
  Dst->Metre = Src.Metre;
  Dst->Level = Src.Level;
  Dst->Level_Unit = Src.Level_Unit;
  Dst->Diff_Time = Src.Diff_Time;
  Dst->Diff_ID = Src.Diff_ID;
}

void GsaCpy(xxgsa_t *Dst, xxgsa_t Src)
{
  Dst->Mode = Src.Mode;
  Dst->Type = Src.Type;
  for (uint8_t i = 0; i < 12; i++)
  {
    Dst->Sno[i] = Src.Sno[i];
  }
  Dst->PDOP = Src.PDOP;
  Dst->HDOP = Src.HDOP;
  Dst->VDOP = Src.VDOP;
}

uint8_t IsLeap(uint16_t Y)
{

  if (Y % 4 == 0)
  {
    if (Y % 100 == 0)
    {
      // 这里如果被 400 整数是闰年
      if (Y % 400 == 0)
        return 1;
    }
    else
      return 1;
  }
  return 0;
}

void addMon(uint16_t *DstYear, uint8_t *DstMon, uint16_t SrcYear, uint8_t SrcMon)
{
  if (SrcMon == 12)
  {
    *DstMon = 1;
    *DstYear = SrcYear + 1;
  }
  else
  {
    *DstMon = SrcMon + 1;
    *DstYear = SrcYear;
  }
}

void addDay(uint16_t *DstYear, uint8_t *DstMon, uint8_t *DstDay, uint16_t SrcYear, uint8_t SrcMon, uint8_t SrcDay)
{
  uint8_t L = IsLeap(SrcYear);

  if (L)
  {
    if (SrcDay == MonDL[SrcMon])
    {
      *DstDay = 1;
      addMon(DstYear, DstMon, SrcYear, SrcMon);
    }
    else
    {
      *DstYear = SrcYear;
      *DstMon = SrcMon;
      *DstDay = SrcDay + 1;
    }
  }
  else
  {
    if (SrcDay == MonD[SrcMon])
    {
      *DstDay = 1;
      addMon(DstYear, DstMon, SrcYear, SrcMon);
    }
    else
    {
      *DstYear = SrcYear;
      *DstMon = SrcMon;
      *DstDay = SrcDay + 1;
    }
  }
}

void timeZoneChange(uint16_t *DstYear, uint8_t *DstMon, uint8_t *DstDay, uint8_t *DstHour, uint16_t SrcYear, uint8_t SrcMon, uint8_t SrcDay, uint8_t SrcHour)
{

  uint8_t temp = SrcHour;
  temp += 8;
  if (temp > 23)
  {
    temp -= 24;
    addDay(DstYear, DstMon, DstDay, SrcYear, SrcMon, SrcDay);
    *DstHour = temp;
  }
  else
  {
    *DstYear = SrcYear;
    *DstMon = SrcMon;
    *DstDay = SrcDay;
    *DstHour = SrcHour + 8;
  }
}

/*=================================================debug=========================================================*/
void printRMC(xxrmc_t Src)
{
  Serial.println("==========RMC==========");
  Serial.printf("UTC1 : %d : %d : %d \r\n", Src.UTC_Hour, Src.UTC_Minute, Src.UTC_Second);
  Serial.printf("State : %c\r\n", Src.State);
  Serial.printf("Latitude : %f %c\r\n", Src.Latitude, Src.Ns);
  Serial.printf("Longitude : %f %c\r\n", Src.Longitude, Src.Ew);
  Serial.printf("Velocity : %f | %f\r\n", Src.Velocity, Src.Azimuth);
  Serial.printf("UTC2 : %d - %d - %d \r\n", Src.UTC_Year, Src.UTC_Month, Src.UTC_Day);
  Serial.printf("Declination : %f %c\r\n", Src.Declination, Src.Dew);
  Serial.printf("Mode : %c\r\n", Src.Mode);
  Serial.println("\r\n\r\n");
}

void printVTG(xxvtg_t Src)
{
  Serial.println("==========VTG==========");
  Serial.printf("T : %f %c\r\n", Src.T_Angle, Src.T_Reference);
  Serial.printf("M : %f %c\r\n", Src.M_Angle, Src.M_Reference);
  Serial.printf("HV %f %c\r\n", Src.N_Horizontal_Velocity, Src.Knots);
  Serial.printf("HV %f %c\r\n", Src.K_Horizontal_Velocity, Src.Kmh);
}

void printGGA(xxgga_t Src)
{
  Serial.println("==========GGA==========");
  Serial.printf("UTC : %d : %d : %d \r\n", Src.UTC_Hour, Src.UTC_Minute, Src.UTC_Second);
  Serial.printf("Latitude : %f %c\r\n", Src.Latitude, Src.Ns);
  Serial.printf("Longitude : %f %c\r\n", Src.Longitude, Src.Ew);
  Serial.printf("Fix : %d\r\n", Src.Fix);
  Serial.printf("SNum : %d\r\n", Src.S_Num);
  Serial.printf("HDOP : %f\r\n", Src.HDOP);
  Serial.printf("Altitude : %f %c\r\n", Src.Altitude, Src.Metre);
  Serial.printf("Level : %f %c\r\n", Src.Level, Src.Level_Unit);
}

void printGSA(xxgsa_t Src)
{
  Serial.println("==========GSA==========");
  Serial.printf("Mode : %c\r\n", Src.Mode);
  Serial.printf("Type : %d\r\n", Src.Type);
  for (uint8_t i = 0; i < 12; ++i)
    Serial.printf("Sno%d : %d\r\n", i, Src.Sno[i]);
  Serial.printf("PDOP : %f\r\n", Src.PDOP);
  Serial.printf("HDOP : %f\r\n", Src.HDOP);
  Serial.printf("VDOP : %f\r\n", Src.VDOP);
}

/*=====================================================doc.======================================================*/
/*
$GPRMC,,V,,,,,,,,,,N*53
GPRMC
（推荐定位信息数据格式）
例：$GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50
字段0：$GPRMC，语句ID，表明该语句为Recommended Minimum Specific GPS/TRANSIT Data（RMC）推荐最小定位信息
字段1：UTC时间，hhmmss.sss格式
字段2：状态，A=定位，V=未定位
字段3：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
字段4：纬度N（北纬）或S（南纬）
字段5：经度dddmm.mmmm，度分格式（前导位数不足则补0）
字段6：经度E（东经）或W（西经）
字段7：速度，节，Knots
字段8：方位角，度
字段9：UTC日期，DDMMYY格式
字段10：磁偏角，（000 - 180）度（前导位数不足则补0）
字段11：磁偏角方向，E=东W=西
字段12：模式，A=自动，D=差分，E=估测，N=数据无效（3.0协议内容）
字段13：校验值（$与*之间的数异或后的值）

$GPVTG,,,,,,,,,N*30
GPVTG
（地面速度信息）
例：$GPVTG,89.68,T,,M,0.00,N,0.0,K*5F
字段0：$GPVTG，语句ID，表明该语句为Track Made Good and Ground Speed（VTG）地面速度信息
字段1：运动角度，000 - 359，（前导位数不足则补0）
字段2：T=真北参照系
字段3：运动角度，000 - 359，（前导位数不足则补0）
字段4：M=磁北参照系
字段5：水平运动速度（0.00）（前导位数不足则补0）
字段6：N=节，Knots
字段7：水平运动速度（0.00）（前导位数不足则补0）
字段8：K=公里/时，km/h
字段9：校验值（$与*之间的数异或后的值）

$GPGGA,,,,,,0,00,99.99,,,,,,*48
GPGGA
（定位信息）
例：$GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,12.2,M,19.7,M,,0000*1F
字段0：$GPGGA，语句ID，表明该语句为Global Positioning System Fix Data（GGA）GPS定位信息
字段1：UTC 时间，hhmmss.sss，时分秒格式
字段2：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
字段3：纬度N（北纬）或S（南纬）
字段4：经度dddmm.mmmm，度分格式（前导位数不足则补0）
字段5：经度E（东经）或W（西经）
字段6：GPS状态，0=不可用(FIX NOT valid)，1=单点定位(GPS FIX)，2=差分定位(DGPS)，3=无效PPS，4=实时差分定位（RTK FIX），5=RTK FLOAT，6=正在估算
字段7：正在使用的卫星数量（00 - 12）（前导位数不足则补0）
字段8：HDOP水平精度因子（0.5 - 99.9）
字段9：海拔高度（-9999.9 - 99999.9）
字段10：单位：M（米）
字段11：地球椭球面相对大地水准面的高度 WGS84水准面划分
字段12：WGS84水准面划分单位：M（米）
字段13：差分时间（从接收到差分信号开始的秒数，如果不是差分定位将为空）
字段14：差分站ID号0000 - 1023（前导位数不足则补0，如果不是差分定位将为空）
字段15：校验值（$与*之间的数异或后的值）

$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30
GPGSA
（ 当前卫星信息）
例：$GPGSA,A,3,01,20,19,13,,,,,,,,,40.4,24.4,32.2*0A
字段0：$GPGSA，语句ID，表明该语句为GPS DOP and Active Satellites（GSA）当前卫星信息
字段1：定位模式(选择2D/3D)，A=自动选择，M=手动选择
字段2：定位类型，1=未定位，2=2D定位，3=3D定位
字段3：PRN码（伪随机噪声码），第1信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段4：PRN码（伪随机噪声码），第2信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段5：PRN码（伪随机噪声码），第3信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段6：PRN码（伪随机噪声码），第4信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段7：PRN码（伪随机噪声码），第5信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段8：PRN码（伪随机噪声码），第6信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段9：PRN码（伪随机噪声码），第7信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段10：PRN码（伪随机噪声码），第8信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段11：PRN码（伪随机噪声码），第9信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段12：PRN码（伪随机噪声码），第10信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段13：PRN码（伪随机噪声码），第11信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段14：PRN码（伪随机噪声码），第12信道正在使用的卫星PRN码编号（00）（前导位数不足则补0）
字段15：PDOP综合位置精度因子（0.5 - 99.9）
字段16：HDOP水平精度因子（0.5 - 99.9）
字段17：VDOP垂直精度因子（0.5 - 99.9）
字段18：校验值（$与*之间的数异或后的值）

$GPGSV,1,1,01,19,,,13*72
GPGSV
(可见卫星信息)
例：$GPGSV,3,1,10,20,78,331,45,01,59,235,47,22,41,069,,13,32,252,45*70
字段0：$GPGSV，语句ID，表明该语句为GPS Satellites in View（GSV）可见卫星信息
字段1：本次GSV语句的总数目（1 - 3）
字段2：本条GSV语句是本次GSV语句的第几条（1 - 3）
字段3：当前可见卫星总数（00 - 12）（前导位数不足则补0）
字段4：PRN 码（伪随机噪声码）（01 - 32）（前导位数不足则补0）
字段5：卫星仰角（00 - 90）度（前导位数不足则补0）
字段6：卫星方位角（00 - 359）度（前导位数不足则补0）
字段7：信噪比（00－99）dbHz
字段8：PRN 码（伪随机噪声码）（01 - 32）（前导位数不足则补0）
字段9：卫星仰角（00 - 90）度（前导位数不足则补0）
字段10：卫星方位角（00 - 359）度（前导位数不足则补0）
字段11：信噪比（00－99）dbHz
字段12：PRN 码（伪随机噪声码）（01 - 32）（前导位数不足则补0）
字段13：卫星仰角（00 - 90）度（前导位数不足则补0）
字段14：卫星方位角（00 - 359）度（前导位数不足则补0）
字段15：信噪比（00－99）dbHz
字段16：校验值（$与*之间的数异或后的值）

$GPGLL,,,,,,V,N*64
GPGLL
（地理定位信息）
例：$GPGLL,4250.5589,S,14718.5084,E,092204.999,A*2D
字段0：$GPGLL，语句ID，表明该语句为Geographic Position（GLL）地理定位信息
字段1：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
字段2：纬度N（北纬）或S（南纬）
字段3：经度dddmm.mmmm，度分格式（前导位数不足则补0）
字段4：经度E（东经）或W（西经）
字段5：UTC时间，hhmmss.sss格式
字段6：状态，A=定位，V=未定位
字段7：校验值（$与*之间的数异或后的值）
*/
