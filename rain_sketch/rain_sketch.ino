
#define RAIN_PIN A0
void setup() {
  Serial.begin(9600);
  pinMode(RAIN_PIN, INPUT);
}


void loop() {
  //analog output
  int value = analogRead(RAIN_PIN);
  int percentage = map(value ,4095, 0, 0, 100);
  Serial.println((String)"Analog: " + value);
  Serial.println((String)"Percentage: " + percentage);
  delay(1000);
}
