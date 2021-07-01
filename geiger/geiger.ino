#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define MINUTE_PERIOD 60000
#define INTEGRATION_PERIOD 30000
#define CALLIBRATION_VALUE 151.0 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SoftwareSerial BlueTooth(10, 11);

unsigned long currentCounts = 0;
unsigned long previousMillis = 0;
unsigned long nextIntegrationCounter = 0;
unsigned long countsPerMinute = 0;
float microSivertsHour = 0.0f;

void setup() {
  initDisplay();
  initSerial();
  initBluetooth();
}

void loop() {
  updateDisplay();
  calculateResult();
}

void initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1.3);
  display.setTextColor(WHITE);
}

void initSerial() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), onGeigerEvent, FALLING);
}

void onGeigerEvent() {
  currentCounts++;
}

void initBluetooth() {
  BlueTooth.begin(9600);
}

void calculateResult() {
  unsigned long currentMillis = millis();
  unsigned long deltaTime = currentMillis - previousMillis;
  nextIntegrationCounter = (INTEGRATION_PERIOD - deltaTime) / 1000;

  if (deltaTime > INTEGRATION_PERIOD) {
    previousMillis = currentMillis;

    countsPerMinute = currentCounts * MINUTE_PERIOD / INTEGRATION_PERIOD;
    microSivertsHour = countsPerMinute / CALLIBRATION_VALUE;
    updateSerial();
    currentCounts = 0;
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println((String) currentCounts +  " counts");
  display.setCursor(0, 25);
  display.println((String) "Result in " +  nextIntegrationCounter  + " sec");
  display.setCursor(0, 37);
  display.println((String) countsPerMinute +  " CPM");
  display.setCursor(0, 47);
  display.println((String) microSivertsHour + " uSv/h");
  display.display();
}

void updateBluetooth() {
  BlueTooth.println((String) millis() + ";" + countsPerMinute + ";" + microSivertsHour);
}

void updateSerial() {
  Serial.println((String) millis() + ";" + countsPerMinute + ";" + microSivertsHour);
}
