// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       gnss_test.ino
    Created:	2018/9/5 21:23:29
    Author:     DESKTOP-GDLUU96\Administrator
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
char* dat = "";
#include <U8glib.h>
#include <SoftwareSerial.h>

SoftwareSerial Mod(12, 13);
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_FAST);
char gps_buffer[101];
char *gpsp[15];
int wordNum;

void setup()
{
	
	Mod.begin(9600);
	Serial.begin(9600);
}

// Add the main program code into the continuous loop() function
void loop()
{
	int res = read_gnss();
	if (res) {
		if (gpsp[6][0]) {

		}
	}
}

uint8_t read_gnss() {
	if (Mod.available()) {
		if (Mod.read() == '$') {
			gps_buffer[0] = '$';
			while (Mod.available() == 0);
			Mod.readBytes(gps_buffer + 1, 5);
			if (strncmp(gps_buffer + 1, "GPGGA", 5) == 0 || strncmp(gps_buffer + 1, "GNGGA", 5) == 0 || strncmp(gps_buffer + 1, "BDGGA", 5) == 0) {
				while (Mod.available() == 0);
				Mod.readBytesUntil('$', gps_buffer + 6, 100 - 6);
				parse();
				return 1;
			}
		}
	}
	return 0;
}

int parse() {
	//delete_crlf(gps_buffer);

	uint8_t leng = strlen(gps_buffer);
	for (uint8_t i = 0; i < leng - 1; i++) {
		if (gps_buffer[i] == '\r' && gps_buffer[i + 1] == '\n') {
			gps_buffer[i] = '\0';
		}
	}

	//uint8_t sum = gpsCalcChecksum(gps_buffer);
	uint8_t sum = gps_buffer[1];
	for (uint8_t i = 2; gps_buffer[i] != '*'; i++) {
		sum ^= gps_buffer[i];
	}

	uint8_t src_len = strlen(gps_buffer);
	uint8_t wordNum = 0;
	gpsp[wordNum++] = gps_buffer;
	for (uint8_t si = 0; si < src_len && wordNum < sizeof(gpsp) / sizeof(char*); si++) {
		if (gps_buffer[si] == ',') {
			gpsp[wordNum++] = gps_buffer + si + 1;
			gps_buffer[si] = '\0';
		}
	}

	char *temp = gpsp[14];
	uint8_t check_result;
	if (temp[0] != '*') {
		//printf ("error no *");
		check_result = 0;
	}
	check_result = hexToInt2(temp + 1);

	if (check_result != sum) {
		return 0;
	}
	else
		return 1;
}

uint8_t hexToInt2(char *the_buf) {
	uint8_t value = 0;
	if (the_buf[0] >= '0' && the_buf[0] <= '9') {
		value += (the_buf[0] - '0') * 16;
	}
	else {
		value += (the_buf[0] - 'A' + 10) * 16;
	}

	if (the_buf[1] >= '0' && the_buf[1] <= '9') {
		value += (the_buf[1] - '0');
	}
	else {
		value += (the_buf[1] - 'A' + 10);
	}
	return value;
}

uint8_t decToInt2(char *the_buf) {
	uint8_t value = 0;
	value += (the_buf[0] - '0') * 10;
	value += (the_buf[1] - '0');
	return value;
}

//char DFGPS::fixc () {
//	return gpsp[6][0];
//}
//
//uint8_t DFGPS::fix () {
//	if (gpsp[6][0])
//		return gpsp[6][0]-'0';
//	else
//		return 0;
//}
//
//boolean DFGPS::gpsAvailable () {
//	if (fixc () == '1')
//		   return true;
//	else
//	return false;
//}
//
//boolean DFGPS::timeAvailable () {
//	if (gpsp[1] != '\0')
//		return true;
//	else
//		return false;
//}
//
//uint8_t DFGPS::getHour () {
//	if (gpsp[1][0])
//		return decToInt2 (gpsp[1]);
//	else
//		return 0;
//}
//
//char *DFGPS::getTime () {
//	return gpsp[1];
//}
//
//uint8_t DFGPS::getMinute () {
//	if (gpsp[1][0])
//		return decToInt2 (gpsp[1]+2);
//	else
//		return 0;
//}
//
//
//uint8_t DFGPS::getSecond () {
//	if (gpsp[1][0])
//		return decToInt2 (gpsp[1]+4);
//	else
//		return 0;
//}
//
//
//double DFGPS::getLatitude () {
//	double lat = (double)decToInt (gpsp[2], 2);
//	return lat + atof (gpsp[2]+2) / 60.0;
//}
//
//char *DFGPS::getLatitudeStr () {
//	return gpsp[2];
//}
//
//double DFGPS::getLongitude () {
//	double lon = (double)decToInt (gpsp[4], 3);
//	return lon + atof (gpsp[4]+3) / 60.0;
//}
//
//char *DFGPS::getLongitudeStr () {
//	return gpsp[4];
//}
//
//char DFGPS::getNS () {
//	return gpsp[3][0];
//}
//
//char DFGPS::getEW () {
//	return gpsp[5][0];
//}
//
//uint8_t DFGPS::getNum () {
//	return atoi (gpsp[7]);
//}
//
//char *DFGPS::getNumStr () {
//	return gpsp[7];
//}
//
//char *DFGPS::getHDOPStr () {
//	return gpsp[8];
//}
//
//double DFGPS::getHDOP () {
//	return atof (gpsp[8]);
//}
//
//char *DFGPS::getAltidudeStr () {
//	return gpsp[9];
//}
//
//double DFGPS::getAltidude () {
//	return atof (gpsp[9]);
//}
//
//char DFGPS::getAunits () {
//	return gpsp[10][0];
//}
//
//char *DFGPS::getLevelStr () {
//	return gpsp[11];
//}
//
//double DFGPS::getLevel () {
//	return atof (gpsp[11]);
//}
//
//char DFGPS::getLunits () {
//	return gpsp[12][0];
//}

//typedef struct {
//	//int id;
//	struct utc{	//1:UTC time
//		int hour;
//		int minutes;
//		int second;
//	}utc;
//	double latitude;	//2:
//	char ns;		//3:
//	double longtitude;	//4:
//	char ew;		//5:
//	int fix;	//6:gps status 0 1 2 3 4 5 6 7 8 9
//	int num;	//7:
//	double hdop;	//8:水平精度
//	double altitude;	//9:海拔高度
//	char a_units;	//10:单位M
//	double level;	//11:大地水准面高度
//	char l_units;	//12:单位M
//	double diff_time;	13
//	int diff_id;	//14
//} gpgga_s;
