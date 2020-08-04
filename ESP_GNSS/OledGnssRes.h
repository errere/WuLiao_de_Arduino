#ifndef OLEDGNSSRES_H
#define OLEDGNSSRES_H

#include <inttypes.h>

/*===================================================Res=========================================================*/
const uint8_t activeSymbol[] PROGMEM = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00};

const uint8_t inactiveSymbol[] PROGMEM = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00};

#endif
/*===================================================struct=========================================================*/
//GPRMC（推荐定位信息数据格式）eg:$GPRMC,,V,,,,,,,,,,N*53  $GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50
typedef struct
{
    //字段1：UTC时间
    uint8_t UTC_Hour = 0;
    uint8_t UTC_Minute = 0;
    uint8_t UTC_Second = 0;
    //字段2：状态，A=定位，V=未定位
    char State = 0;
    //字段3：纬度
    double Latitude = 0.0;
    //字段4：纬度N（北纬）或S（南纬）
    char Ns = 0;
    //字段5：经度
    double Longitude = 0.0;
    //字段6：经度E（东经）或W（西经）
    char Ew = 0;
    //字段7：速度，节，Knots
    double Velocity = 0.0;
    //字段8：方位角，度
    double Azimuth = 0.0;
    //字段9：UTC日期
    uint8_t UTC_Day = 0;
    uint8_t UTC_Month = 0;
    uint16_t UTC_Year = 0;
    //字段10：磁偏角
    double Declination = 0.0;
    //字段11：磁偏角方向，E=东W=西
    char Dew = 0;
    //字段12：模式，A=自动，D=差分，E=估测，N=数据无效（3.0协议内容）
    char Mode = 0;

} xxrmc_t;

//GPVTG（地面速度信息） eg:$GPVTG,,,,,,,,,N*30  $GPVTG,89.68,T,,M,0.00,N,0.0,K*5F
typedef struct
{
    //字段1：运动角度，000 - 359，（前导位数不足则补0）
    double T_Angle = 0.0;
    //字段2：T=真北参照系
    char T_Reference = 'T';
    //字段3：运动角度，000 - 359，（前导位数不足则补0）
    double M_Angle = 0.0;
    //字段4：M=磁北参照系
    char M_Reference = 'M';
    //字段5：水平运动速度（0.00）（前导位数不足则补0）
    double N_Horizontal_Velocity = 0.0;
    //字段6：N=节，Knots
    char Knots = 'N';
    //字段7：水平运动速度（0.00）（前导位数不足则补0）
    double K_Horizontal_Velocity = 0.0;
    //字段8：K=公里/时，km/h
    char Kmh = 'K';

} xxvtg_t;

//GPGGA（定位信息） eg:$GPGGA,,,,,,0,00,99.99,,,,,,*48  $GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,12.2,M,19.7,M,,0000*1F
typedef struct
{
    //字段1：UTC 时间，hhmmss.sss，时分秒格式
    uint8_t UTC_Hour = 0;
    uint8_t UTC_Minute = 0;
    uint8_t UTC_Second = 0;
    //字段2：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
    double Latitude = 0.0;
    //字段3：纬度N（北纬）或S（南纬）
    char Ns = 0;
    //字段4：经度dddmm.mmmm，度分格式（前导位数不足则补0）
    double Longitude = 0.0;
    //字段5：经度E（东经）或W（西经）
    char Ew = 0;
    //字段6：GPS状态，0=不可用(FIX NOT valid)，1=单点定位(GPS FIX)，2=差分定位(DGPS)，3=无效PPS，4=实时差分定位（RTK FIX），5=RTK FLOAT，6=正在估算
    uint8_t Fix = 0;
    //字段7：正在使用的卫星数量（00 - 12）（前导位数不足则补0）
    uint8_t S_Num = 0;
    //字段8：HDOP水平精度因子（0.5 - 99.9）
    double HDOP = 0.0;
    //字段9：海拔高度（-9999.9 - 99999.9）
    double Altitude = 0.0;
    //字段10：单位：M（米）
    char Metre = 'M';
    //字段11：地球椭球面相对大地水准面的高度 WGS84水准面划分
    double Level = 0.0;
    //字段12：WGS84水准面划分单位：M（米）
    char Level_Unit = 'M';
    //字段13：差分时间（从接收到差分信号开始的秒数，如果不是差分定位将为空）
    double Diff_Time = 0.0;
    //字段14：差分站ID号0000 - 1023（前导位数不足则补0，如果不是差分定位将为空）
    uint16_t Diff_ID = 0;

} xxgga_t;

//GPGSA（ 当前卫星信息） eg:$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30  $GPGSA,A,3,01,20,19,13,,,,,,,,,40.4,24.4,32.2*0A
typedef struct
{
    //字段1：定位模式(选择2D/3D)，A=自动选择，M=手动选择
    char Mode = 0;
    //字段2：定位类型，1=未定位，2=2D定位，3=3D定位
    uint8_t Type = 0;
    //字段(3-14)：PRN码（伪随机噪声码），(第1信道-第12信道)正在使用的卫星PRN码编号（00）（前导位数不足则补0）
    uint8_t Sno[12] = {0};
    //字段15：PDOP综合位置精度因子（0.5 - 99.9）
    double PDOP = 0.0;
    //字段16：HDOP水平精度因子（0.5 - 99.9）
    double HDOP = 0.0;
    //字段17：VDOP垂直精度因子（0.5 - 99.9）
    double VDOP = 0.0;

} xxgsa_t;

/*


$GPRMC,,V,,,,,,,,,,N*53
Type = 1
CSum = 83,Rsum = 83 --> check_result is 1
0: $GPRMC
1: 
2: V
3: 
4: 
5: 
6: 
7: 
8: 
9: 
10: 
11: 
12: N


$GPVTG,,,,,,,,,N*30
Type = 2
CSum = 48,Rsum = 48 --> check_result is 1
0: $GPVTG
1: 
2: 
3: 
4: 
5: 
6: 
7: 
8: 
9: N


$GPGGA,,,,,,0,00,99.99,,,,,,*48
Type = 3
CSum = 72,Rsum = 72 --> check_result is 1
0: $GPGGA
1: 
2: 
3: 
4: 
5: 
6: 0
7: 00
8: 99.99
9: 
10: 
11: 
12: 
13: 
14: 


$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30
Type = 4
CSum = 48,Rsum = 48 --> check_result is 1
0: $GPGSA
1: A
2: 1
3: 
4: 
5: 
6: 
7: 
8: 
9: 
10: 
11: 
12: 
13: 
14: 
15: 99.99
16: 99.99
17: 99.99

================================================================================================================

$GPRMC,123819.00,A,2001.55321,N,11018.38192,E,0.907,,170720,,,D*73
Type = 1
CSum = 115,Rsum = 115 --> check_result is 1
0: $GPRMC
1: 123819.00
2: A
3: 2001.55321
4: N
5: 11018.38192
6: E
7: 0.907
8: 
9: 170720
10: 
11: 
12: D


$GPVTG,,T,,M,0.907,N,1.679,K,D*21
Type = 2
CSum = 33,Rsum = 33 --> check_result is 1
0: $GPVTG
1: 
2: T
3: 
4: M
5: 0.907
6: N
7: 1.679
8: K
9: D


$GPGGA,123819.00,2001.55321,N,11018.38192,E,2,03,4.36,329.8,M,-12.0,M,,0000*78
Type = 3
CSum = 120,Rsum = 120 --> check_result is 1
0: $GPGGA
1: 123819.00
2: 2001.55321
3: N
4: 11018.38192
5: E
6: 2
7: 03
8: 4.36
9: 329.8
10: M
11: -12.0
12: M
13: 
14: 0000


$GPGSA,A,3,24,12,50,,,,,,,,,,6.06,4.36,4.21*04
Type = 4
CSum = 4,Rsum = 4 --> check_result is 1
0: $GPGSA
1: A
2: 3
3: 24
4: 12
5: 50
6: 
7: 
8: 
9: 
10: 
11: 
12: 
13: 
14: 
15: 6.06
16: 4.36
17: 4.21
*/