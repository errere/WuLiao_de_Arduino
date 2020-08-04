#include <WiFi.h>
#include <PubSubClient.h>
#include "AIP1650.h"

const uint8_t DIGMAP[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
const uint8_t DIG_LOST[] = {0x38, 0x5c, 0x6d, 0x78};
const uint8_t DIG_CONN[] = {0x39, 0x5c, 0x37, 0x37};
const uint8_t DIG_STBY[] = {0x6d, 0x78, 0x7c, 0x6e};
const uint8_t DIG_SERV[] = {0x6d, 0x79, 0x31, 0x3e};

const char *ssid = "HONOR_9X";             //ESP32连接的WiFi账号
const char *password = "1234567890";       //WiFi密码
const char *mqttServer = "123.56.221.173"; //要连接到的服务器IP
const int mqttPort = 1883;                 //要连接到的服务器端口号
const char *mqttUser = "admin";            //MQTT服务器账号
const char *mqttPassword = "public";       //MQTT服务器密码

const char *mqttTopic = "RGBLEDMQTT"; //MQTT订阅主题
const char *mqttHostName = "ESP32Client";

WiFiClient espClient;           // 定义wifiClient实例
PubSubClient client(espClient); // 定义PubSubClient的实例

AIP1650 A;
uint8_t Subc = 0;

void callback(char *topic, byte *payload, unsigned int length)
{
      Serial.print("Topic:"); //串口打印：来自订阅的主题:
      Serial.println(topic);  //串口打印订阅的主题

      if (strncmp(topic, mqttTopic, 10) == 0)
      {
            if (length == 8)
            {
                  if (payload[0] == 0xa0 && payload[6] == 0x0a)
                  {

                        uint8_t aa = 255 - payload[5];
                        uint8_t ww = map(aa, 0, 255, 0, payload[4]);
                        uint8_t cw = map(payload[5], 0, 255, 0, payload[4]);

                        Serial1.write(0xa0);

                        Serial1.write(ww);
                        Serial1.write(cw);

                        Serial1.write(payload[3]);
                        Serial1.write(payload[2]);
                        Serial1.write(payload[1]);

                        Serial1.write(0x0a);

                        for (int i = 0; i < length; i++) //使用循环打印接收到的信息
                        {
                              Serial.printf("0x%x,", (char)payload[i]);
                        }
                  }
            }
            Serial.println("\r\n");
      }
}

void setup()
{
      Serial1.begin(115200, SERIAL_8N1, 19, 18);
      Serial.begin(115200); //串口函数，波特率设置

      A.init(5, 17, LED_SG8); //init(uint8_t Dat, uint8_t Clk);
      Write4Dig(DIG_CONN);

      WiFi.begin(ssid, password); //接入WiFi函数（WiFi名称，密码）重新连接wifi
      WiFi.setAutoReconnect(true);

      Serial.println("conn");               //串口输出：连接wifi中
      while (WiFi.status() != WL_CONNECTED) //若WiFi接入成功WiFi.status()会返回 WL_CONNECTED
      {
            Serial.print("."); //串口输出：连接wifi中

            delay(500); //若尚未连接WiFi，则进行重连WiFi的循环
      }
      Serial.println("\r\nWifi ok"); //连接wifi成功之后会跳出循环，串口并输出：wifi连接成功

      client.setServer(mqttServer, mqttPort); //MQTT服务器连接函数（服务器IP，端口号）
      client.setCallback(callback);           //设定回调方式，当ESP32收到订阅消息时会调用此方法

      while (!client.connected()) //是否连接上MQTT服务器
      {
            Serial.println("loading Serv");                           //串口打印：连接服务器中
            if (client.connect(mqttHostName, mqttUser, mqttPassword)) //如果服务器连接成功
            {
                  Write4Dig(DIG_STBY);
                  Serial.println("Serv OK"); //串口打印：服务器连接成功
            }
            else
            {
                  Write4Dig(DIG_SERV);
                  Serial.print("Fail Serv");    //串口打印：连接服务器失败
                  Serial.print(client.state()); //重新连接函数
                  delay(2000);
            }
      }

      client.subscribe(mqttTopic); //连接MQTT服务器后订阅主题
      Subc = 1;
      Serial.print("OK"); //串口打印：已订阅主题，等待主题消息
                          //client.publish("/World","Hello from ESP32");//向服务器发送的信息(主题，内容)
}

void loop()
{
      client.loop(); //回旋接收函数  等待服务器返回的数据
      delay(1);

      Write4Dig(DIG_STBY);

      while (!client.connected()) //是否连接上MQTT服务器
      {
            if (WiFi.status() != WL_CONNECTED) //若WiFi接入成功WiFi.status()会返回 WL_CONNECTED
            {
                  Subc = 0;
                  Serial.print("."); //串口输出：连接wifi中
                  Write4Dig(DIG_LOST);
                  delay(500); //若尚未连接WiFi，则进行重连WiFi的循环
            }
            else if (client.connect(mqttHostName, mqttUser, mqttPassword)) //如果服务器连接成功
            {
                  Write4Dig(DIG_STBY);
            }
            else
            {
                  Subc = 0;
                  Write4Dig(DIG_SERV);
                  Serial.print("Fail Serv");    //串口打印：连接服务器失败
                  Serial.print(client.state()); //重新连接函数
                  delay(2000);
            }
            delay(1);
      }

      while (WiFi.status() != WL_CONNECTED) //若WiFi接入成功WiFi.status()会返回 WL_CONNECTED
      {
            Serial.print("."); //串口输出：连接wifi中
            Write4Dig(DIG_LOST);
            delay(500); //若尚未连接WiFi，则进行重连WiFi的循环
      }

      if (Subc == 0)
      {
            client.setServer(mqttServer, mqttPort); //MQTT服务器连接函数（服务器IP，端口号）
            client.setCallback(callback);           //设定回调方式，当ESP32收到订阅消息时会调用此方法
            client.subscribe(mqttTopic);            //连接MQTT服务器后订阅主题
            Subc = 1;
      }
}

void Write4Dig(const uint8_t *Buf)
{
      for (int i = 0; i < 4; i++)
      {
            A.Write(i, Buf[i]);
            delay(1);
      }
}
