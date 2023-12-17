#include <LiquidCrystal.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

#define REPORTING_PERIOD_MS 1000
unsigned long previousMillis = 5000, previousMillis1;
PulseOximeter pox;
uint32_t tsLastReport = 0;

#define ThermistorPin A0  // for Arduino microcontroller
long ADC_Value;
float R1 = 10000;  // value of R1 on board
float logR2, R2, T;

//steinhart-hart coeficients for thermistor
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;
float temp_c, temp_f;

#define buzz 4
int LED = 13;
void onBeatDetected() {
  Serial.println("Beat!");
  digitalWrite(LED, !digitalRead(LED));
}
void pox_update() {
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;)
      ;
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}
void temp() {
  ADC_Value = 0;
  for (int i = 0; i < 50; i++) {
    ADC_Value = ADC_Value + analogRead(ThermistorPin);
    delay(1);
  }

  ADC_Value = ADC_Value / 50;
  R2 = R1 * (1023.0 / (float)ADC_Value - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp_c = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp_c = temp_c - 273.15;                                         //convert Kelvin to Celcius
  temp_f = (temp_c * 9.0) / 5.0 + 32.0;                             //convert Celcius to Fahrenheit
}

void setup() {
  Serial.begin(9600);
  pinMode(ThermistorPin, INPUT);  //10k thermister 1 pin + 1+10k 10k 1pin -

  pinMode(buzz, OUTPUT);  // declare Buzzer as output
  pinMode(13, OUTPUT);
  digitalWrite(buzz, HIGH);
  Serial.print("Initializing pulse oximeter..");
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  delay(3000);
  lcd.clear();

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;)
      ;
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {

  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    Serial.print("<A");
    Serial.print(pox.getHeartRate());
    Serial.print(">");
    Serial.print("<B");
    Serial.print(pox.getSpO2());
    Serial.print(">");
    Serial.print("<C");
    Serial.print(temp_f);
    Serial.print(">");
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("H: ");
    lcd.setCursor(2, 0);
    lcd.print(pox.getHeartRate());

    lcd.setCursor(9, 0);
    lcd.print("S: ");
    lcd.setCursor(11, 0);
    lcd.print(pox.getSpO2());  //919716040141
    lcd.print("%");


    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.setCursor(5, 1);
    lcd.print(temp_f, 1);
    lcd.write(0xdf);  // to display °
    lcd.print("f ");

    tsLastReport = millis();
  }
  if (pox.getHeartRate() > 110) {
    digitalWrite(buzz, LOW);
    delay(1000);
  }
    if (temp_f > 104) {
    digitalWrite(buzz, LOW);
     delay(1000);
  }
  else 
   {
    digitalWrite(buzz, HIGH);
  }


  if (millis() - previousMillis > 10000) {
    temp();
    previousMillis = millis();
    pox_update();
  }
}
