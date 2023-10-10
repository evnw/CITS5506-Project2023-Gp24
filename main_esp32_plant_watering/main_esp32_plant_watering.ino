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

char auth[] = BLYNK_AUTH_TOKEN;
// Wifi Credentials
char ssid[] = "EvsPhone";
char pass[] = "123456789";

BlynkTimer timer;

// min moisture
#define LOW_MOIST 20

// Hardware Pins
#define soilPin A0
#define waterPump 2

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
  Serial.begin(115200);
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
}
