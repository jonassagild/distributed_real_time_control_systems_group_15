// setup constants
const int sensor_pin = 1;
int anread = 0;

void setup() {
  // initialize Serial
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  anread = analogRead(sensor_pin);
  Serial.println(anread);
  delay(200);
}
