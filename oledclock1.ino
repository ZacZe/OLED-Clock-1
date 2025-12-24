#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPass";

String temperatureReader = "https://api.open-meteo.com/v1/forecast?latitude=51.5085&longitude=-0.1257&models=ukmo_seamless&current=temperature_2m";
String time = "http://worldtimeapi.org/api/timezone/Europe/London.txt"

#define SCREEN_WIDTH 128 // width
#define SCREEN_HEIGHT 64 // height

#define OLED_RESET -1 // reset pin
#define SCREEN_ADDRESS 0x3C // 0x3C, not 0x3D ever!!!!
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

void setup() {
  // put your setup code here, to run once:
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    drawConnecting();
  }


}

void loop() {
  // put your main code here, to run repeatedly:
}

void drawLocationTmp(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Location: "));
  display.println(F("");)

  display.display();
  delay(2000);
}

void drawConnecting(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Connecting to the internet"));
  display.println(F(".");)
  display.display();
  delay(1000)
  display.println(F(".");)
  display.display();
  delay(1000)
  display.println(F(".");)
  display.display();
}

void drawTime(void) {
  display.clearDisplay(); 
  display.setTextSize(3); 
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(4,0);
  display.println(F(time));
  display.display();
  delay(1000); 
}
