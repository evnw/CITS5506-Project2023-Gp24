/*
CITS5506 Project 2023 - Group 24
Smart WiFi Based House Plant/Urban Garden Irrigation System
*/

#define BLYNK_TEMPLATE_ID "TMPL6cIO6vdaG"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#define BLYNK_AUTH_TOKEN "ksXot_p1gaBVSUSGR-lXoOCZJ5B1X7oM"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <BME280I2C.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

char auth[] = BLYNK_AUTH_TOKEN;
// Wifi Credentials
char ssid[] = "EvsPhone";
char pass[] = "123456789";

BlynkTimer timer;

// min moisture
#define LOW_MOIST 20

// Hardware Pins
#define soilPin A0
#define waterPump 12

// Virtual Pins
#define VPIN_MOIST  V0
#define VPIN_PUMP   V1 
#define VPIN_TEMP   V2
#define VPIN_HUMID  V3
#define VPIN_LIGHT  V4
#define VPIN_AUTO   V5

int moistPercentage;
long temperature;
int humidity;
int autoMode = 1;

// Setup for BME280
BME280I2C::Settings settings(
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::OSR_X1,
  BME280::Mode_Forced,
  BME280::StandbyTime_1000ms,
  BME280::Filter_Off,
  BME280::SpiEnable_False,
  BME280I2C::I2CAddr_0x77 // I2C address for BME280. Adjust if needed.
);
BME280I2C bme(settings);

// Setup for SparkFun VEML6030 Ambient Light Sensor
#define AL_ADDR 0x10
SparkFun_Ambient_Light light(AL_ADDR);
float gain = .125;
int integrationTime = 100;
long luxVal = 0;

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(VPIN_MOIST);
  Blynk.syncVirtual(VPIN_PUMP);
  Blynk.syncVirtual(VPIN_TEMP);
  Blynk.syncVirtual(VPIN_HUMID);
  Blynk.virtualWrite(VPIN_AUTO, 1);
}

// If pump turned on from dashboard
BLYNK_WRITE(VPIN_PUMP)
{
  if (param.asInt() == 1)
  {
    Serial.println("Manual pump on");
    pumpOn();
  }
}

// if auto mode changed
BLYNK_WRITE(VPIN_AUTO)
{
  autoMode = param.asInt();
  Serial.println((String)"Auto Mode: " + autoMode);
}

void readMoisture()
{
  int moistVal = analogRead(soilPin);
  moistPercentage= map(moistVal ,4095, 0, 0, 100);
  Serial.println((String)"Soil moisture: " + moistPercentage);
  Blynk.virtualWrite(V0, moistPercentage);
  delay(1000);
}

void pumpOn()
{
  // water the plant
  // 10 secs ?
  Blynk.virtualWrite(VPIN_PUMP, 1);
  Serial.println("Pump On");
  digitalWrite(waterPump, HIGH);
  delay(5000);
  digitalWrite(waterPump, LOW);
  Blynk.virtualWrite(VPIN_PUMP, 0);
  Serial.println("Pump Off");
}

void controlPump()
{
  if (moistPercentage <= (LOW_MOIST) && autoMode == 1)
  {
    Serial.println("Low moisture -> turning pump on");
    pumpOn();
  }
}

void setup()
{
  Wire.begin(); // Initialize I2C
  Serial.begin(115200);

  while (!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

    // Initialize SparkFun VEML6030 Ambient Light Sensor
  if (light.begin()) {
    Serial.println("Ready to sense some light!");
  } else {
    Serial.println("Could not communicate with the light sensor!");
  }

  light.setGain(gain);
  light.setIntegTime(integrationTime);

  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, LOW);
  delay(100);

  Blynk.begin(auth, ssid, pass);
  delay(1000);
  timer.setInterval(5000L, readMoisture);
  delay(5000);
}

void loop() {
  Blynk.run();
  timer.run();
  controlPump();

  float temp(NAN), hum(NAN), pres(NAN);
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C\tHumidity: ");
  Serial.print(hum);
  Serial.print("%\tPressure: ");
  Serial.print(pres);
  Serial.println(" Pa");

  // Reading data from SparkFun VEML6030 Ambient Light Sensor
  luxVal = light.readLight();
  Serial.print("Ambient Light Reading: ");
  Serial.print(luxVal);
  Serial.println(" Lux");
  
  delay(1000);
}
