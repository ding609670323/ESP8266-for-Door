 
#include <ESP8266WiFi.h>
#include <Time.h>
#include <Timezone.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "NTP.h"
 
#define WIFI_SSID "Home_IOT"       // 使用时请修改为当前你的 wifi ssid
#define WIFI_PASS "txxh2016"       // 使用时请修改为当前你的 wifi 密码

//根据液晶接口类型来配置端口 
//七针模块有cs脚；直接连接就好了；如果是用六针的CS默认已经接地；不接就可以了
#define OLED_MOSI  0               // 0   => D1(MOSI)
#define OLED_CLK   2               // 2   => D0(CLK)
#define OLED_DC    5               // 5   => DC
#define OLED_CS    13              // 13  => CS
#define OLED_RESET 4               // 4   => RES
#define DOOR       14              //门窗传感器接口
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
 
// 北京时间时区
#define STD_TIMEZONE_OFFSET +8    // Standard Time offset (-7 is mountain time)

//中文：门
 static const unsigned char PROGMEM str_1[] =
{ 
0x20,0x00,0x13,0xFC,0x10,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,
0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x04,0x40,0x14,0x40,0x08
  };

//中文：窗
static const unsigned char PROGMEM str_2[] =
{ 
0x02,0x00,0x01,0x00,0x7F,0xFE,0x48,0x22,0x92,0x14,0x04,0x00,0x3F,0xF8,0x22,0x08,
0x23,0xC8,0x24,0x48,0x2A,0x88,0x21,0x08,0x22,0x88,0x24,0x08,0x3F,0xF8,0x20,0x08
  };

//中文：状
static const unsigned char PROGMEM str_3[] =
{ 
0x08,0x40,0x08,0x48,0x08,0x44,0x48,0x44,0x28,0x40,0x2F,0xFE,0x08,0x40,0x08,0x40,
0x18,0x40,0x28,0xA0,0xC8,0xA0,0x08,0x90,0x09,0x10,0x09,0x08,0x0A,0x04,0x0C,0x02
  };

//中文：态
static const unsigned char PROGMEM str_4[] =
{ 
0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x02,0x80,0x04,0x40,0x0A,0x20,0x31,0x18,
0xC0,0x06,0x01,0x00,0x08,0x88,0x48,0x84,0x48,0x12,0x48,0x12,0x87,0xF0,0x00,0x00
  };

//中文：开
static const unsigned char PROGMEM str_5[] =
{ 
0x00,0x00,0x7F,0xFC,0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0xFF,0xFE,
0x08,0x20,0x08,0x20,0x08,0x20,0x08,0x20,0x10,0x20,0x10,0x20,0x20,0x20,0x40,0x20
  };
//中文：关
static const unsigned char PROGMEM str_6[] =
{ 
0x10,0x10,0x08,0x10,0x08,0x20,0x00,0x00,0x3F,0xF8,0x01,0x00,0x01,0x00,0x01,0x00,
0xFF,0xFE,0x01,0x00,0x02,0x80,0x02,0x80,0x04,0x40,0x08,0x20,0x30,0x18,0xC0,0x06
  };
//中文：：
static const unsigned char PROGMEM str_7[] =
{ 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x30,0x00,0x30,0x00,0x00,0x00,0x30,0x00,0x30,0x00,0x00,0x00,0x00,0x00
  };  
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// ***************************************************************
// 时区和夏令时规则
// ***************************************************************
 
// Define daylight savings time rules for the China
//定义为中国
TimeChangeRule mySTD = {"", First,  Sun, Jan, 0, STD_TIMEZONE_OFFSET * 60};
Timezone myTZ(mySTD, mySTD);
 
WiFiClient client;
 
// This function is called once a second
//这个函数一秒钟调用一次
void updateDisplay(void) {
  
  TimeChangeRule *tcr;        //指向时间变化规则的指针
 
  // 从NTP提供者读取当前UTC时间
  time_t utc = now();
 
  // 将DST转换为本地时间
  time_t localTime = myTZ.toLocal(utc, &tcr);
 
  // 将时间转换为年月日时分秒
  int weekdays=   weekday(localTime);
  int days    =   day(localTime);
  int months  =   month(localTime);
  int years   =   year(localTime);
  int seconds =   second(localTime);
  int minutes =   minute(localTime);
  int hours   =   hour(localTime) ;   //采用12小时格式则使用 hourFormat12(localTime) isPM()/isAM()

  int n =digitalRead(DOOR);           //取出端口值
  
  display.setTextSize(1);             //设置字体大小
  display.setTextColor(WHITE);        //设置字体颜色白色
  display.setCursor(8,56);            //设置字体的起始位置
  display.print(years);
  display.print("/");
  display.print(months);
  display.print("/");
  display.print(days);
  display.print(" * ");
  display.print(hours);
  display.print(":");
  display.print(minutes);
  display.print(":");
  display.print(seconds);
  
  //对值判断，高则显示开|低则关
  if( n == HIGH){                      
    display.drawBitmap(96, 32, str_5, 16, 16, 1);
  }
  if( n == LOW){
    display.drawBitmap(96, 32, str_6, 16, 16, 1); 
  }
  display.display();                  //显示以上信息
  display.clearDisplay();             //清屏
}
 
void setup() {
  //连接网络
  initNTP(WIFI_SSID, WIFI_PASS);
  //设置端口为输入模式
  pinMode(DOOR,INPUT);                       
  //初始化液晶
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  delay(10);
}
 
// Previous seconds value
time_t previousSecond = 0;
 
void loop() {
  display.setTextSize(2);             //设置字体大小
  display.setTextColor(WHITE); 
  display.setCursor(14,0);
  display.print("-ESP8266-");
  
  display.drawBitmap(16, 32, str_1, 16, 16, 1); //在坐标X:16  Y:16的位置显示中文字符凌
  display.drawBitmap(32, 32, str_2, 16, 16, 1); //在坐标X:32  Y:16的位置显示中文字符顺
  display.drawBitmap(48, 32, str_3, 16, 16, 1);
  display.drawBitmap(64, 32, str_4, 16, 16, 1);
  display.drawBitmap(80, 32, str_7, 16, 16, 1);
  display.display();
  
  //  仅当时间改变时才更新显示
  if (timeStatus() != timeNotSet) {
    if (second() != previousSecond) {
      previousSecond = second();
      // 更新显示
      updateDisplay();
    }
  }
  delay(1000);
}
