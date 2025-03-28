#include "TFT_eSPI.h"
#include "SPI.h"
#include "pin_config.h"
#include "Arduino.h"
#include "WiFiMulti.h"
#include "ArduinoJson.h"
#include "time.h"
#include "HTTPClient.h"
#include "user_secrets.h"
#include "fronius_logo.h"

WiFiClient wifiClient;

#define MY_NTP_SERVER "pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"                                    // Timezone Central European Time
#define topbutton 0

String ssid = WIFI_SSID;
String password = WIFI_PASSWORD;
String urlMeter = "http://" + Fronius_IP + "/solar_api/v1/GetMeterRealtimeData.cgi?Scope=Device&DeviceId=0";
String urlFlow = "http://" + Fronius_IP + "/solar_api/v1/GetPowerFlowRealtimeData.fcgi";

typedef struct struct_message {
  bool State;
} struct_message;

struct_message Button;

time_t now;
struct tm tm;

TFT_eSPI tft = TFT_eSPI();

unsigned long targetTime = 0;
static int previousHour = -1;                                                     // Track the previous hour
float etotal_p = 0;                                                               // Variable to store etotal value at midnight

void setup() {
  Serial.begin(115200);
  Serial.println("Hello");

  pinMode(PIN_POWER_ON, OUTPUT);                                                  // Set the power pin as output
  digitalWrite(PIN_POWER_ON, HIGH);                                               // Turn on the display
  pinMode(topbutton, INPUT);                                                      // Set the button pin as input

  tft.init(); // Initialize the TFT
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 320, 170, (uint16_t *)fronius_logo);                        // Display the Fronius logo

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Wait for WiFi... ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println("\nWiFi connection successful");

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  configTzTime(MY_TZ, MY_NTP_SERVER);
}

// Fetch the data from the Fronius API
void fetchData(const String &url, DynamicJsonBuffer &jsonBuffer, JsonObject *&jsonObject) {
  HTTPClient http;
  http.begin(wifiClient, url);    
  int httpCode = http.GET();                                                      // Make the request
  String httpResponse = http.getString();                                         // Get the response
  http.end();

  Serial.print("URL: ");
  Serial.println(url);
  Serial.print("HTTP Status: ");
  Serial.println(httpCode);

  jsonObject = &jsonBuffer.parseObject(httpResponse);                             // Parse the JSON
  if (!jsonObject->success()) {
    Serial.println("JSON-Parser: Fail");
  } else {
    Serial.println("JSON-Parser: OK");
  }
}

// Display the data on the TFT
void displayData(float value, const char *label, uint16_t color, bool invert = false) { 
  tft.setTextColor(color);
  tft.print(label);
  tft.print(invert ? value * -1 : value, 0);
  tft.println("W");
}

void loop() {
  DynamicJsonBuffer jsonBufferMeter(JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60);
  DynamicJsonBuffer jsonBufferFlow(JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60);

  time(&now);
  localtime_r(&now, &tm);                                                         // Get the time
  Serial.print("Hour: ");
  Serial.println(tm.tm_hour);

  JsonObject *jsonMeter = nullptr;
  fetchData(urlMeter, jsonBufferMeter, jsonMeter);

  float l1p = (*jsonMeter)["Body"]["Data"]["PowerReal_P_Phase_1"] | 0;            // Get the power value for phase 1
  float l2p = (*jsonMeter)["Body"]["Data"]["PowerReal_P_Phase_2"] | 0;            // Get the power value for phase 2
  float l3p = (*jsonMeter)["Body"]["Data"]["PowerReal_P_Phase_3"] | 0;            // Get the power value for phase 3

  JsonObject *jsonFlow = nullptr;
  fetchData(urlFlow, jsonBufferFlow, jsonFlow);

  float etotal = (*jsonFlow)["Body"]["Data"]["Inverters"]["1"]["E_Total"] | 0;    // Get the total energy produced 
  float soc = (*jsonFlow)["Body"]["Data"]["Inverters"]["1"]["SOC"] | 0;           // Get the battery state of charge
  float p = (*jsonFlow)["Body"]["Data"]["Inverters"]["1"]["P"] | 0;               // Get the power value for the Inverter
  float in_out = (*jsonFlow)["Body"]["Data"]["Site"]["P_Grid"] | 0;               // Get the power value for the grid
  float cons = (*jsonFlow)["Body"]["Data"]["Site"]["P_Load"] | 0;                 // Get the power value for the load
  float prod = (*jsonFlow)["Body"]["Data"]["Site"]["P_PV"] | 0;                   // Get the power value for the Solar

  if (previousHour == -1 || (previousHour == 23 && tm.tm_hour == 0)) 
      etotal_p = etotal;                                                          // Store the etotal value at startup or midnight
  previousHour = tm.tm_hour;

  tft.setTextSize(3);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);

  displayData(in_out, "GR: ", in_out >= 0 ? TFT_RED : TFT_GREEN, in_out < 0);     // Display the power value for the grid
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW);
  tft.print("PV: ");
  tft.print(p, 0);                                                                // Display the power value for the Inverter
  tft.print("W / ");
  tft.print(prod, 0);
  tft.println("W");                                                               // Display the power value for the Solar
  tft.setTextColor(TFT_LIGHTGREY);
  tft.print("LD: ");
  tft.print(cons * -1, 0);                                                        // Display the power value for the load
  tft.print("W B: ");
  tft.print(soc, 0);                                                              // Display the battery state of charge
  tft.println("%");

  displayData(l1p, "L1: ", l1p >= 0 ? TFT_RED : TFT_GREEN, l1p < 0);              // Display the power value for phase 1
  displayData(l2p, "L2: ", l2p >= 0 ? TFT_RED : TFT_GREEN, l2p < 0);              // Display the power value for phase 2
  displayData(l3p, "L3: ", l3p >= 0 ? TFT_RED : TFT_GREEN, l3p < 0);              // Display the power value for phase 3

  tft.setTextColor(TFT_LIGHTGREY);
  tft.print("ET: ");
  tft.print(etotal - etotal_p, 0);                                                // Display the energy produced today
  tft.println("Wh");

  delay(5000);
}

// TFT Pin check
#if PIN_LCD_WR != TFT_WR ||     \
    PIN_LCD_RD != TFT_RD ||     \
    PIN_LCD_CS != TFT_CS ||     \
    PIN_LCD_DC != TFT_DC ||     \
    PIN_LCD_RES != TFT_RST ||   \
    PIN_LCD_D0 != TFT_D0 ||     \
    PIN_LCD_D1 != TFT_D1 ||     \
    PIN_LCD_D2 != TFT_D2 ||     \
    PIN_LCD_D3 != TFT_D3 ||     \
    PIN_LCD_D4 != TFT_D4 ||     \
    PIN_LCD_D5 != TFT_D5 ||     \
    PIN_LCD_D6 != TFT_D6 ||     \
    PIN_LCD_D7 != TFT_D7 ||     \
    PIN_LCD_BL != TFT_BL ||     \
    TFT_BACKLIGHT_ON != HIGH || \
    170 != TFT_WIDTH ||         \
    320 != TFT_HEIGHT
#error "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#endif

// ESP32 version check
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#error "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif
