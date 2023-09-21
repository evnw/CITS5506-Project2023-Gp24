
int RAIN_PIN = 25;
void setup() {
  Serial.begin(9600);
  pinMode(RAIN_PIN, INPUT);
}
void loop() {
  //analog output
  int value = digitalRead(RAIN_PIN);
  Serial.println(value);
  delay(1000);
}
