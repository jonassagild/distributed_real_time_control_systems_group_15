// method reads the value of sensorPin, and prints the value to the console. Also prints the lux value

// setup constants
const int sensorPin = 1; // connect sensor to analog input 1

void setup (){
  Serial.begin(115200); // initialize Serial
}

void loop(){
  // reads the value from the LDR
  int anRead = analogRead(sensorPin);

  // prints the value to the console
  Serial.println(anRead);

  // prints value of lux to console
  // double lux = pow(10, (-0.0021*anRead + 5.1431 - 5.7747)/(-0.7757));
  // Serial.println(lux);

  // waits a second
  delay(1000);
} 
