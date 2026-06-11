#include <Wire.h>
#include <math.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---- TIME ----
int hours = 0;
int minutes = 0;
int seconds = 0;
unsigned long currentTime = 0;

// ---- FAKE WEATHER ----
String location = "London";
String temperature = "15*C";
String weather = "Cloudy";

// ---- LOGO ----
#define LOGO_HEIGHT 15
#define LOGO_WIDTH 16

static const unsigned char PROGMEM logo_bmp[] = {
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b00000000, 0b11100111,
  0b00000001, 0b11001110,
  0b00000011, 0b10011100,
  0b00000111, 0b00111000,
  0b00001110, 0b01110000,
  0b00011100, 0b11100000,
  0b00111001, 0b11000000,
  0b01110011, 0b10000000,
  0b11100111, 0b00000000,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111
};

// MUST BE BETWEEN 0 AND 59 INCLUSIVELY. 
int hours = 18;
int minutes = 23;
int seconds = 50;

const char* WIFI_SSID = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
const char* OPENWEATHER_API_KEY = "OPENWEATHER_API_KEY";
const char* WEATHER_LOCATION = "London,GB";

const unsigned long WEATHER_UPDATE_MS = 10UL * 60UL * 1000UL;
unsigned long lastWeatherUpdate = 0;
float currentTemperature = NAN;
String currentWeather = "Loading";
String weatherError = "err";

void connectToWifi();
void updateWeather();
bool parseWeatherResponse(const String& response);

void setup() {
  Serial.begin(9600);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("BOOT OK");
  display.println("SYSTEM READY");
  display.display();

  drawLogo(); 
  connectToWifi();
  updateWeather();
}

void loop() {
  timeTicking();
  if (millis() - lastWeatherUpdate >= WEATHER_UPDATE_MS) {
    updateWeather();
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("OLED Clock"));
  display.setCursor(0, 20);
  display.println( timeFormatter() );
  display.setTextSize(1);
  display.setCursor(0, 44);
  display.print(F("London: "));
  if (weatherError.length() > 0) {
    display.println(weatherError);
  } else if (isnan(currentTemperature)) {
    display.println(F("Loading"));
  } else {
    display.print(currentTemperature, 1);
    display.println(F(" C"));
    display.setCursor(0, 56);
    display.println(currentWeather);
  }
  display.setTextSize(2);
  display.display();
  delay(990);
}

// ---------- LOGO ----------
void drawLogo() {
  display.clearDisplay();

  display.drawBitmap(
    (SCREEN_WIDTH - LOGO_WIDTH) / 2,
    (SCREEN_HEIGHT - LOGO_HEIGHT) / 2,
    logo_bmp,
    LOGO_WIDTH,
    LOGO_HEIGHT,
    SSD1306_WHITE
  );

  display.display();
  delay(2000);
}

void timeTicking() {
  seconds = seconds+1;
  if (seconds >= 60) {
    seconds = 0;
    minutes = minutes+1;
    if (minutes >= 60) {
      minutes = 0;
      hours = hours+1;
      if (hours >= 24) {
        hours = 0;
      }
    }
  }
}

String formatTime() {
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return String(buffer);
}

// ---------- UI ----------
void drawTime() {
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.println(formatTime());
  display.setTextSize(1);
}

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Connecting WiFi"));
  display.display();

  unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("WiFi connected: "));
    Serial.println(WiFi.localIP());
  } else {
    weatherError = "WiFi failed";
    Serial.println(F("WiFi connection failed"));
  }

  display.setTextSize(2);
}

void updateWeather() {
  lastWeatherUpdate = millis();

  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
    if (WiFi.status() != WL_CONNECTED) {
      return;
    }
  }

  String url = "http://api.openweathermap.org/data/2.5/weather?q=";
  url += WEATHER_LOCATION;
  url += "&appid=";
  url += OPENWEATHER_API_KEY;
  url += "&units=metric";

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    weatherError = "API error";
    Serial.print(F("OpenWeather HTTP error: "));
    Serial.println(httpCode);
    http.end();
    return;
  }

  String response = http.getString();
  http.end();

  if (parseWeatherResponse(response)) {
    weatherError = "";
    Serial.print(F("London weather: "));
    Serial.print(currentTemperature, 1);
    Serial.print(F(" C, "));
    Serial.println(currentWeather);
  } else {
    weatherError = "Parse error";
    Serial.println(F("Could not parse OpenWeather response"));
  }
}

bool parseWeatherResponse(const String& response) {
  int tempKey = response.indexOf("\"temp\":");
  if (tempKey < 0) {
    return false;
  }

  int tempStart = tempKey + 7;
  int tempEnd = response.indexOf(',', tempStart);
  if (tempEnd < 0) {
    return false;
  }

  int descKey = response.indexOf("\"description\":\"");
  if (descKey < 0) {
    return false;
  }

  int descStart = descKey + 15;
  int descEnd = response.indexOf('"', descStart);
  if (descEnd < 0) {
    return false;
  }

  currentTemperature = response.substring(tempStart, tempEnd).toFloat();
  currentWeather = response.substring(descStart, descEnd);
  if (currentWeather.length() > 0) {
    currentWeather.setCharAt(0, toupper(currentWeather.charAt(0)));
  }
  return true;
}
