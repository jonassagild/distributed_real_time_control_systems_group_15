const int ledPin = 3; // LED connected to digital pin 13
const int sensorPin = 5; // connect sensor to analog input 0
const int toggleSensorPin = 1; // toggle sensor
// the next two lines set the min and max delay between blinks
const int minDuration = 100; // minimum wait between blinks
const int maxDuration = 1000; // maximum wait between blinks

void setup (){
  pinMode(ledPin, OUTPUT); // enable output on the led pin
  Serial.begin(9600); // initialize Serial
}

void loop(){
  int rate = analogRead(sensorPin); // read the analog input

  //scales the blink rate between the min and max   values
  rate = map(rate, 200, 800, minDuration, maxDuration);  
  rate = constrain(rate, minDuration,maxDuration); // saturate
  // Serial.println(rate); // print rate to serial monitor

  int rate_2 = analogRead(toggleSensorPin);
  Serial.println(rate_2);

  if(rate_2 > 1000) {
    // if high -> toggle
    if (digitalRead(ledPin) == HIGH ){
      digitalWrite(ledPin, LOW); 
    } else {
      digitalWrite(ledPin, HIGH);
    }
    delay(1000);
  }

  
  digitalWrite(ledPin, HIGH); // set the LED on
  delay(rate); // wait duration dependent on light level
  digitalWrite(ledPin, LOW); // set the LED off
  delay(rate);
} 
