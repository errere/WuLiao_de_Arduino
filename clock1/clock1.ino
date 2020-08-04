
#include <Wire.h>
#include <SFE_BMP180.h>
#include <U8glib.h>
#include <RTClib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);	// Fast I2C / TWI 
SFE_BMP180 pressure;
RTC_DS3231 rtc;

void setup()
{


}

// Add the main program code into the continuous loop() function
void loop()
{


}
