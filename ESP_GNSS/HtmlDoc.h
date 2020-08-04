/*=====================================================doc.======================================================*/


// ======================日期时间======================
//     UTC 时间，hhmmss.sss，时分秒格式

//     UTC日期，DDMMYY格式

// ======================定位======================

//     模式，A=自动，D=差分，E=估测，N=数据无效（3.0协议内容）
const char HD_GPS_LOC_MOD_A[] PROGMEM = "自动";
const char HD_GPS_LOC_MOD_D[] PROGMEM = "差分";
const char HD_GPS_LOC_MOD_E[] PROGMEM = "估测";
const char HD_GPS_LOC_MOD_N[] PROGMEM = "数据无效";
const char *HD_GPS_LOC_MOD[] = {"N/A", HD_GPS_LOC_MOD_A, HD_GPS_LOC_MOD_D, HD_GPS_LOC_MOD_E, HD_GPS_LOC_MOD_N};

//     纬度ddmm.mmmm，度分格式（前导位数不足则补0）
//     纬度N（北纬）或S（南纬）
//     经度dddmm.mmmm，度分格式（前导位数不足则补0）
//     经度E（东经）或W（西经）
const char HD_GPS_LOC_LAT_N[] PROGMEM = "北纬";
const char HD_GPS_LOC_LAT_S[] PROGMEM = "南纬";
const char HD_GPS_LOC_LON_E[] PROGMEM = "东经";
const char HD_GPS_LOC_LON_W[] PROGMEM = "西经";
const char *HD_GPS_LOC_LALO[] = {"N/A", HD_GPS_LOC_LAT_N, HD_GPS_LOC_LAT_S, HD_GPS_LOC_LON_E, HD_GPS_LOC_LON_W};

//     海拔高度（-9999.9 - 99999.9）
//     单位：M（米）

//     磁偏角，（000 - 180）度（前导位数不足则补0）
//     磁偏角方向，E=东W=西
const char HD_GPS_LOC_CPJ_E[] PROGMEM = "东";
const char HD_GPS_LOC_CPJ_W[] PROGMEM = "西";
const char *HD_GPS_LOC_CPJ[] = {"N/A", HD_GPS_LOC_CPJ_E, HD_GPS_LOC_CPJ_W};

//     地球椭球面相对大地水准面的高度 WGS84水准面划分
//     WGS84水准面划分单位：M（米）

// ======================运动======================

//     运动角度，000 - 359，（前导位数不足则补0）
//     T=真北参照系

//     运动角度，000 - 359，（前导位数不足则补0）
//     M=磁北参照系

//     水平运动速度（0.00）（前导位数不足则补0）
//     N=节，Knots

//     水平运动速度（0.00）（前导位数不足则补0）
//     K=公里/时，km/h


// ======================卫星======================

//     正在使用的卫星数量（00 - 12）（前导位数不足则补0）


//     PRN码（伪随机噪声码），信道正在使用的卫星PRN码编号（前导位数不足则补0）


// ======================状态模式======================

//     GPS状态，0=不可用(FIX NOT valid)，1=单点定位(GPS FIX)，2=差分定位(DGPS)，
// 3=无效PPS，4=实时差分定位（RTK FIX），5=RTK FLOAT，6=正在估算
const char HD_GPS_STU_0[] PROGMEM = "不可用";
const char HD_GPS_STU_1[] PROGMEM = "单点定位";
const char HD_GPS_STU_2[] PROGMEM = "差分定位";
const char HD_GPS_STU_3[] PROGMEM = "无效PPS";
const char HD_GPS_STU_4[] PROGMEM = "实时差分定位";
const char HD_GPS_STU_5[] PROGMEM = "RTK FLOAT";
const char HD_GPS_STU_6[] PROGMEM = "正在估算";
const char *HD_GPS_STU[] = {HD_GPS_STU_0, HD_GPS_STU_1, HD_GPS_STU_2, HD_GPS_STU_3, HD_GPS_STU_4, HD_GPS_STU_5, HD_GPS_STU_6};

//     定位模式(选择2D/3D)，A=自动选择，M=手动选择

const char HD_GPS_MOD_A[] PROGMEM = "自动";
const char HD_GPS_MOD_M[] PROGMEM = "手动";
const char *HD_GPS_MOD[] = {"N/A", HD_GPS_MOD_A, HD_GPS_MOD_M};

//     定位类型，1=未定位，2=2D定位，3=3D定位
const char HD_GPS_TYP_NO[] PROGMEM = "未定位";
const char HD_GPS_TYP_2D[] PROGMEM = "2D定位";
const char HD_GPS_TYP_3D[] PROGMEM = "3D定位";
const char *HD_GPS_TYP[] = {"N/A", HD_GPS_TYP_NO, HD_GPS_TYP_2D, HD_GPS_TYP_3D};

// ======================精度因子======================

//     PDOP综合位置精度因子（0.5 - 99.9）

//     HDOP水平精度因子（0.5 - 99.9）

//     VDOP垂直精度因子（0.5 - 99.9）
