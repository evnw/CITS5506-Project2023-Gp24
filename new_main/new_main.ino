#define BLYNK_TEMPLATE_ID "TMPL6cIO6vdaG"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#define BLYNK_AUTH_TOKEN "ksXot_p1gaBVSUSGR-lXoOCZJ5B1X7oM"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <BME280I2C.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN; // Replace with your Blynk auth token
char ssid[] = "EvsPhone";
char pass[] = "123456789";

BlynkTimer timer;

#define LOW_MOIST 20
#define soilPin A0
#define waterPump 2

#define VPIN_MOIST  V0
#define VPIN_PUMP   V1 
#define VPIN_TEMP   V2
#define VPIN_HUMID  V3
#define VPIN_LIGHT  V4
#define VPIN_AUTO   V5

int moistPercentage;
float temperature;
float humidity;
int autoMode = 1;

BME280I2C bme;
#define AL_ADDR 0x10
SparkFun_Ambient_Light light(AL_ADDR);
float gain = 0.125;
int integrationTime = 100;
long luxVal = 0;

void pumpOn()
{
  // Water the plant for 5 seconds
  digitalWrite(waterPump, HIGH);
  delay(5000);
  digitalWrite(waterPump, LOW);
}

void readMoisture()
{
  int moistVal = analogRead(soilPin);
  moistPercentage = map(moistVal, 0, 4095, 0, 100); // Invert the map function arguments
  Serial.println("Soil moisture: " + String(moistPercentage) + "%");
  Blynk.virtualWrite(VPIN_MOIST, moistPercentage);
}

void controlPump()
{
  if (moistPercentage <= LOW_MOIST && autoMode == 1)
  {
    Serial.println("Low moisture -> turning pump on");
    pumpOn();
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(VPIN_MOIST);
  Blynk.syncVirtual(VPIN_PUMP);
  Blynk.syncVirtual(VPIN_TEMP);
  Blynk.syncVirtual(VPIN_HUMID);
  Blynk.virtualWrite(VPIN_AUTO, 1);
}

BLYNK_WRITE(VPIN_PUMP)
{
  if (param.asInt() == 1)
  {
    Serial.println("Manual pump on");
    pumpOn();
  }
}

BLYNK_WRITE(VPIN_AUTO)
{
  autoMode = param.asInt();
  Serial.println("Auto Mode: " + String(autoMode));
}

void setup()
{
  Wire.begin(); // Initialize I2C
  Serial.begin(115200);

  if (!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    while (1)
      ; // Halt the program if the sensor is not found
  }

  if (!light.begin())
  {
    Serial.println("Could not communicate with the light sensor!");
    while (1)
      ; // Halt the program if the light sensor is not found
  }

  light.setGain(gain);
  light.setIntegTime(integrationTime);

  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, LOW);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, readMoisture);
}

void loop()
{
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

  luxVal = light.readLight();
  Serial.print("Ambient Light Reading: ");
  Serial.print(luxVal);
  Serial.println(" Lux");

  delay(1000);
}
