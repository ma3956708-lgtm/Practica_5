
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

#define DEVICE_DISCONNECTED_C -127
#define enfriar 16 //tx
#define calentar 17 //rx
#define on_enfriar digitalWrite (enfriar, HIGH)
#define off_enfriar digitalWrite (enfriar, LOW)
#define on_calentar digitalWrite (calentar, HIGH)
#define off_calentar digitalWrite (calentar, LOW)


// LCD I2C: direccion 0x27, 16 columnas y 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Entradas analogicas de la ESP32
int POT1_PIN = 34;
int POT2_PIN = 35;
int NTC_PIN  = 32;

// Parametros del termistor NTC
float R_FIXED = 10000.0; // Resistencia fija: 10 kohm
float R0 = 10000.0;      // NTC: 10 kohm a 25 grados C
float BETA = 3950.0;     // Beta tipica del NTC
float T_0 = 298.15;       // 25 grados C en Kelvin
float VCC = 3.3;
float ADC_MAX = 4095.0;

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetPinAttenuation(POT1_PIN, ADC_11db);
  analogSetPinAttenuation(POT2_PIN, ADC_11db);
  analogSetPinAttenuation(NTC_PIN, ADC_11db);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("ESP32 + NTC");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();

  pinMode(enfriar, OUTPUT);
  pinMode(calentar, OUTPUT);
  off_enfriar;
  off_calentar;
}

float leerVoltaje(int pin) {
  int lectura = analogRead(pin);
  return lectura * VCC / ADC_MAX;
}

float leerTemperaturaNTC() {
  uint32_t mv = 0;
  for (int i = 0; i < 16; i++) mv +=analogReadMilliVolts(NTC_PIN);
  mv /= 16;
  return mv / 10.0;
}

void loop() {
  float v1 = leerVoltaje(POT1_PIN);
  float v2 = leerVoltaje(POT2_PIN);
  float temp = leerTemperaturaNTC();
  
  float x1 = 3.3;
  float y1 = 100;
  float x2 = 0;
  float y2 = 50;
  float porcentaje = ((((y2 - y1) / (x2 - x1)) * (v1 - x1)) + y1);

  float xx1 = 3.3;
  float yy1 = 40;
  float xx2 = 0;
  float yy2 = 0;
  float porcentaje2 = ((((yy2 - yy1) / (xx2 - xx1)) * (v2 - xx1)) + yy1);


  // Mostrar los voltajes en la primera fila
  lcd.setCursor(0, 0);
  lcd.print("HL:");
  lcd.print(porcentaje, 1);
  lcd.print(" LL:");
  lcd.print(porcentaje2, 1);

  // Mostrar la temperatura en la segunda fila
  lcd.setCursor(0, 1);
  lcd.print("NTC: ");

  if (isnan(temp)) {
    lcd.print("Error lectura ");
  } else {
    lcd.print(temp, 1);
    lcd.print((char)223); // Simbolo de grados
    lcd.print("C       ");
  }

  // Monitor serie
  Serial.print("High Level: ");
  Serial.print(v1, 3);
  Serial.print(" V | Low Level: ");
  Serial.print(v2, 3);
  Serial.print(" V | Temperatura NTC: ");
  Serial.print(temp, 2);
  Serial.println(" C");

  if (temp != DEVICE_DISCONNECTED_C) {
            if (temp >= porcentaje) { on_enfriar; off_calentar; }
            if (temp <= porcentaje2) { off_enfriar; on_calentar; }
        }

        Serial.print("Temperatura: ");
        if (temp == DEVICE_DISCONNECTED_C) Serial.println("Sensor desconectado");
        else { Serial.print(temp); Serial.println(" °C"); }

        
    

  delay(500);
}