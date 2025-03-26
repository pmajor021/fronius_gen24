#include "TFT_eSPI.h" // Graphics and font library for ST7735 driver chip
#include "SPI.h"
#include "pin_config.h"
#include "Arduino.h"
#include "WiFiMulti.h"
#include "ArduinoJson.h"
#include "time.h"
#include "HTTPClient.h"

WiFiClient wifiClient;

#define MY_NTP_SERVER "pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define topbutton 0

String ssid = "MAJORhq";
String password = "Babos021Babos";
String urlMeter = "http://192.168.111.120/solar_api/v1/GetMeterRealtimeData.cgi?Scope=Device&DeviceId=0";
String urlFlow = "http://192.168.111.120/solar_api/v1/GetPowerFlowRealtimeData.fcgi";

typedef struct struct_message {
    bool State;
} struct_message;

struct_message Button;

time_t now;
struct tm tm;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

unsigned long targetTime = 0;
byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11;
uint32_t runing = 0;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(topbutton, 0);

    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);


    tft.fillScreen(TFT_RED); delay(1000);
    tft.fillScreen(TFT_GREEN); delay(1000);
    tft.fillScreen(TFT_BLUE); delay(1000);

    targetTime = millis() + 1000;

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(2000);
    Serial.print("Wait for WiFi... ");
    tft.print("Wait for WiFi... ");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        tft.print(".");
        delay(1000);
    }
    Serial.println("");
    tft.println("");
    Serial.println("WiFi connection successful");
    tft.println("WiFi connection successful");
    delay(3000);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    configTime(3600, 3600, MY_NTP_SERVER);

}

void loop()
{

HTTPClient http;
  const size_t capacityMeter = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  const size_t capacityFlow = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  
  DynamicJsonBuffer jsonBufferMeter(capacityMeter);
  DynamicJsonBuffer jsonBufferFlow(capacityFlow);

  time(&now);
  localtime_r(&now, &tm);
  Serial.print("Hour: ");
  Serial.println(tm.tm_hour);

   //URL #1 - Meter Data
  http.begin(wifiClient, urlMeter);
  int httpCodeMeter = http.GET();
  String httpResponseMeter = http.getString();
  http.end();


  Serial.print("URL:               ");
  Serial.println(urlMeter);
  Serial.print("HTTP Status:       ");
  Serial.println(httpCodeMeter);
  JsonObject& jsonMeter = jsonBufferMeter.parseObject(httpResponseMeter);
  if (!jsonMeter.success()) {
    Serial.println("JSON-Parser:       Fail");
  } else {
    Serial.println("JSON-Parser:       OK");
  }

  float l1p = (jsonMeter["Body"]["Data"]["PowerReal_P_Phase_1"] | 0);
  float l2p = (jsonMeter["Body"]["Data"]["PowerReal_P_Phase_2"] | 0);
  float l3p = (jsonMeter["Body"]["Data"]["PowerReal_P_Phase_3"] | 0);

   //URL #2 - Flow Data
  http.begin(wifiClient, urlFlow);
  int httpCodeFlow = http.GET();
  String httpResponseFlow = http.getString();
  http.end();


  Serial.print("URL:               ");
  Serial.println(urlFlow);
  Serial.print("HTTP Status:       ");
  Serial.println(httpCodeFlow);
  JsonObject& jsonFlow = jsonBufferFlow.parseObject(httpResponseFlow);
  if (!jsonFlow.success()) {
    Serial.println("JSON-Parser:       Fail");
  } else {
    Serial.println("JSON-Parser:       OK");
  }
  Serial.println();
  float soc = (jsonFlow["Body"]["Data"]["Inverters"]["1"]["SOC"] | 0);
  float p = (jsonFlow["Body"]["Data"]["Inverters"]["1"]["P"] | 0);
  float in_out = (jsonFlow["Body"]["Data"]["Site"]["P_Grid"] | 0);
  float cons = (jsonFlow["Body"]["Data"]["Site"]["P_Load"] | 0);
  float prod = (jsonFlow["Body"]["Data"]["Site"]["P_PV"] | 0);
  float autonomy = (jsonFlow["Body"]["Data"]["Site"]["rel_Autonomy"] | 0);
  float selfcons = (jsonFlow["Body"]["Data"]["Site"]["rel_SelfConsumption"] | 0);


  targetTime = millis();

  tft.setTextSize(4);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  if (in_out >= 0) {
    tft.setTextColor(TFT_RED);
    tft.print("GR: ");
    tft.print(in_out, 0);
    tft.println("W");
  } else {
    tft.setTextColor(TFT_GREEN);
    tft.print("GR: ");
    tft.print(in_out * -1, 0);
    tft.println("W");
  }
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW);
  tft.print("PV: ");
  tft.print(p, 0);
  tft.print("W / ");
  tft.print(prod, 0);
  tft.println("W");
  tft.setTextColor(TFT_LIGHTGREY);
  tft.print("LD: ");
  tft.print(cons * -1, 0);
  tft.print("W B: ");
  tft.print(soc, 0);
  tft.println("%");
  if (l1p >= 0) {
    tft.setTextColor(TFT_RED);
    tft.print("L1: ");
    tft.print(l1p, 0);
    tft.println("W");
  } else {
    tft.setTextColor(TFT_GREEN);
    tft.print("L1: ");
    tft.print(l1p * -1, 0);
    tft.println("W");
  }
  if (l2p >= 0) {
    tft.setTextColor(TFT_RED);
    tft.print("L2: ");
    tft.print(l2p, 0);
    tft.println("W");
  } else {
    tft.setTextColor(TFT_GREEN);
    tft.print("L2: ");
    tft.print(l2p * -1, 0);
    tft.println("W");
  }if (l3p >= 0) {
    tft.setTextColor(TFT_RED);
    tft.print("L3: ");
    tft.print(l3p, 0);
    tft.println("W");
  } else {
    tft.setTextColor(TFT_GREEN);
    tft.print("L3: ");
    tft.print(l3p * -1, 0);
    tft.println("W");
  }

  delay(5000);

}


// TFT Pin check
#if PIN_LCD_WR  != TFT_WR || \
    PIN_LCD_RD  != TFT_RD || \
    PIN_LCD_CS    != TFT_CS   || \
    PIN_LCD_DC    != TFT_DC   || \
    PIN_LCD_RES   != TFT_RST  || \
    PIN_LCD_D0   != TFT_D0  || \
    PIN_LCD_D1   != TFT_D1  || \
    PIN_LCD_D2   != TFT_D2  || \
    PIN_LCD_D3   != TFT_D3  || \
    PIN_LCD_D4   != TFT_D4  || \
    PIN_LCD_D5   != TFT_D5  || \
    PIN_LCD_D6   != TFT_D6  || \
    PIN_LCD_D7   != TFT_D7  || \
    PIN_LCD_BL   != TFT_BL  || \
    TFT_BACKLIGHT_ON   != HIGH  || \
    170   != TFT_WIDTH  || \
    320   != TFT_HEIGHT
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif




