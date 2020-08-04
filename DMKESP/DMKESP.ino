#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "HONOR_9X";
const char *password = "1234567890";
const char APIAdress[] = "http://123.56.221.173/GetDanmaku.php";

HTTPClient http;

StaticJsonDocument<700> jsonBuffer;

void setup()
{
  pinMode(2,2);
      Serial.begin(115200);
      // put your setup code here, to run once:
      Serial.println("Connecting...");
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED)
      {
            delay(500);
            Serial.print(".");
      }
      WiFi.setAutoReconnect(true);
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
}

void loop()
{
      // put your main code here, to run repeatedly:
      http.begin(APIAdress); //HTTPhttps://www.arduino.cn/thread-86568-1-1.html
      //http.begin("http://123.56.221.173/index.html"); //HTTPhttps://www.arduino.cn/thread-86568-1-1.html

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK)
            {
                  String payload = http.getString();
                  Serial.println(payload);

                   
                   deserializeJson(jsonBuffer, payload);
                   JsonObject object = jsonBuffer.as<JsonObject>();

                   Serial.println(object["Msg"].as<String>());

            }
      }
      else
      {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
      delay(10000);
      delay(10000);
      delay(10000);
      delay(10000);
      delay(10000);
      delay(10000);
      delayMicroseconds(1);
}
