#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define builtin_led 2 

#define SCREEN_WIDTH 128 // width
#define SCREEN_HEIGHT 64 // height

#define OLED_RESET -1 // reset pin
#define SCREEN_ADDRESS 0x3C // 0x3C, not 0x3D ever!!!!
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT 15
#define LOGO_WIDTH 16

static const unsigned char PROGMEM logo_bmp[] =
{ 0b11111111, 0b11111111,
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
/*
int hours = 16;
int minutes = 23;
int seconds = 30;
*/
int currentTime = seconds+(minutes*60)+(hours*3600);

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22);                  // explicitly init I2C
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 failed"));
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("BOOT OK"));
  display.setCursor(2,0);
  display.println(F("I/O OK"));
  display.display();

  drawLogo(); 
}

void loop() {
  timeTicking();
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("OLED Clock"));
  display.setCursor(0, 20);
  display.println( timeFormatter() );
  display.display();

  delay(973);
}

void drawLogo() {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(2000);
}

void timeTicking() {
  currentTime = millis() / 1000; // total seconds since start
  hours = (currentTime / 3600) % 24;
  minutes = (currentTime / 60) % 60;
  seconds = currentTime % 60;
}

String timeFormatter() {
  String h, m, s;
  if (seconds < 10) {
    s = "0"+String(seconds);
  } else {
    s = String(seconds);
  }

  if (minutes < 10) {
    m = "0"+String(minutes);
  } else {
    m = String(minutes);
  }

  if (hours < 10) {
    h = "0"+String(hours);
  } else {
    h = String(hours);
  }
  
  return h+":"+m+":"+s;
}