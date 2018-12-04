// implements the feedforward control. Uses the static gain to calculate lux.
// records both initial and end states, and prints them to console. 

// setup constants
const int ledPin = 6; // LED connected to digital pin 10 
const int sensorPin = 1; // connect sensor to analog input 1 
float initial_lux_set_point = 35; // starts LED-light at 0. 
float lux_set_point = 15;
int anRead = 0; // value received from LDR 
int i = 0;

void setup() {
  pinMode(ledPin, OUTPUT); // enable output on the led pin
  Serial.begin(9600); // initialize Serial
  setPwmFrequency(6,1);
}

void loop() {
  if (i == 1) {
      delay(10000); // have to wait for transfer to finish
      exit(0);
  }
  i = 1;
  // put your main code here, to run repeatedly:
  analogWrite(ledPin, (initial_lux_set_point-0.42)/0.1846);
  delay(60000); // waits for steady-state
  anRead = analogRead(sensorPin);
  double initial_lux = pow(10, (-0.0021*anRead + 5.1431 - 5.7747)/(-0.7757));
  
  analogWrite(ledPin, (lux_set_point-0.42)/0.1846);
  delay(60000); // waits for steady-state
  anRead = analogRead(sensorPin);
  double end_lux = pow(10, (-0.0021*anRead + 5.1431 - 5.7747)/(-0.7757));
  
  // prints to console
  Serial.print("Start lux was set to: ");
  Serial.println(initial_lux_set_point);
  Serial.print("Actual initial lux was: ");
  
  Serial.println(initial_lux);
  
  Serial.print("End lux was set to: ");
  Serial.println(lux_set_point);
  Serial.print("Actual end lux was: ");
  Serial.println(end_lux);

}
