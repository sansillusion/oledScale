/*
  Load cell scale for 1kg load cell and arduino
  using oled for display in grams and onces
  push button to tare
  use serial to find proper calibration value
*/

//#include <Arduino.h>
#include <U8g2lib.h>
#include "HX711.h"
#include <Wire.h>

#define DOUT  5
#define CLK  4

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

HX711 scale(DOUT, CLK);

float calibration_factor = -1815; //ok for my 1kg load cell
const byte interruptPin = 2;
int taremoi = 1;
void setup() {
  u8g2.begin();

  Serial.begin(115200);
  Serial.println("HX711 calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  scale.set_scale();
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tare, CHANGE);
  long zero_factor = scale.read_average(); //Get a baseline reading
}

void tare() {
  taremoi = 1;
}

void loop() {
  if (taremoi != 0) {
    Serial.println("Taring");
    scale.tare(); //Reset the scale to 0
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_logisoso28_tr);
      u8g2.drawStr(24, 45, "Tare");
    } while ( u8g2.nextPage() );
    delay(1000);
    taremoi = 0;
  }
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  float poids = scale.get_units();
  if (poids < 0.09) {
    if (poids > -0.09) {
      poids = 0;
    }
  }
  Serial.print(poids, 1);
  Serial.print(" Grams");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  float onces = poids / 28.3495;
  char g[10];
  dtostrf(poids, 5, 1, g);
  char o[10];
  dtostrf(onces, 5, 2, o);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_logisoso28_tn);
    u8g2.drawStr(0, 32, g);
    u8g2.drawStr(0, 64, o);
    u8g2.setFont(u8g2_font_logisoso22_tr);
    u8g2.drawStr(110, 32, "G");
    u8g2.drawStr(90, 64, "oz");
  } while ( u8g2.nextPage() );
  delay(200);
  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
}
