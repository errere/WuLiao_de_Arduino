#include <inttypes.h>
#include <string.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define GPSP_SIZE 18

volatile SemaphoreHandle_t procSemaphore;

uint8_t AllowRecv = 0;
String RawBuffer = "";
char RawBufferProc[255] = "";

char *gpsp[GPSP_SIZE];

//GPRMC,GPVTG,GPGGA,GPGSA,GPGSV,GPGLL

void TaskGnssGet(void *pvParameters);
void TaskGnssProc(void *pvParameters);

uint8_t split_by_char(char *the_src, char the_char, char **the_des, uint8_t the_siz);
uint8_t split_by_comma(char *the_src, char **the_des, uint8_t the_siz);
uint8_t delete_crlf(char *the_buf);
uint8_t hexToInt2(char *the_buf);

uint8_t gpsCalcChecksum(char *array);
uint8_t gpsReadChecksumR(char *the_buf);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 18, 19); //gnss

  Serial1.setRxBufferSize(1024);

  procSemaphore = xSemaphoreCreateBinary();

  for (uint8_t i = 0; i < GPSP_SIZE; i++)
  {
    gpsp[i] = (char *)malloc(50);
  }

  xTaskCreatePinnedToCore(TaskGnssGet, "TaskGnssGet", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskGnssProc, "TaskGnssProc", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
}

void loop()
{
  delay(10000);
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
void TaskGnssProc(void *pvParameters) // This is a task.
{
  (void)pvParameters;
  for (;;) // A Task shall never return or exit.
  {
    if (xSemaphoreTake(procSemaphore, 0) == pdTRUE)
    {
      Serial.print(RawBufferProc);
      //GPRMC,GPVTG,GPGGA,GPGSA,GPGSV,GPGLL
      if (strncmp(RawBufferProc + 1, "GPRMC", 5) == 0 || strncmp(RawBufferProc + 1, "GPVTG", 5) == 0 || strncmp(RawBufferProc + 1, "GPGGA", 5) == 0 || strncmp(RawBufferProc + 1, "GPGSA", 5) == 0 || strncmp(RawBufferProc + 1, "GPGSV", 5) == 0 || strncmp(RawBufferProc + 1, "GPGLL", 5) == 0)
      {

        delete_crlf(RawBufferProc);
        uint8_t sum = gpsCalcChecksum(RawBufferProc);
        uint8_t check_result = gpsReadChecksumR(RawBufferProc);
        uint8_t wordNum = split_by_comma(RawBufferProc, gpsp, sizeof(gpsp) / sizeof(char *));
        //uint8_t check_result = gpsReadChecksum(gpsp);

        Serial.printf("CSum = %d,Rsum = %d\r\n", sum, check_result);

        for (int i = 0; i < wordNum; i++)
        {
          Serial.print(i);
          Serial.print(": ");
          Serial.println(gpsp[i]);
        }
      } //GPRMC,GPVTG,GPGGA,GPGSA,GPGSV,GPGLL

      Serial.print("\r\n\r\n");
    }
    vTaskDelay(1);
  }
}

/*==========================================tools============================================*/
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

// check sum using xor
uint8_t gpsCalcChecksum(char *array)
{
  uint8_t sum = array[1];
  for (uint8_t i = 2; array[i] != '*'; i++)
  {
    sum ^= array[i];
  }
  return sum;
}

//get gga checksum
uint8_t gpsReadChecksumR(char *the_buf)
{
  uint8_t leng = strlen(the_buf);
  uint8_t i = 0;

  for (i = leng - 1; i < 0; i--)
  {
    if (the_buf[i] == '*')
    {
      break;
    }
  }

  if (i < 6)
    return 0;

  //Serial.printf("RSS = %s\r\n", the_buf + i - 1);

  uint8_t sum = hexToInt2(the_buf + i - 1);
  return sum;
}
