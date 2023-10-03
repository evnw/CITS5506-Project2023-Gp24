#define relayPin 14

void setup() {
  // put your setup code here, to run once:
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(relayPin, HIGH);
  Serial.println("on");
  delay(5000);
  digitalWrite(relayPin, LOW);
  Serial.println("off");
  delay(5000);
}
