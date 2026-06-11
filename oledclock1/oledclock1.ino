#include <Wire.h>
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

  delay(1500);

  drawLogo();
}

void loop() {
  updateTime();

  display.clearDisplay();

  drawWeather();
  drawTime();

  display.display();

  delay(1000);
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

// ---------- TIME ----------
void updateTime() {
  currentTime = millis() / 1000;

  hours = (currentTime / 3600) % 24;
  minutes = (currentTime / 60) % 60;
  seconds = currentTime % 60;
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

void drawWeather() {
  display.setCursor(0, 0);
  display.println(location);
  display.println(weather);
  display.println(temperature);
}