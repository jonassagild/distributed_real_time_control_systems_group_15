// method increments the voltage across LED light, and reads value from LDR. 

// With the box closed, show that, in steady state, identical increments in the LED 
// actuation correspond to identical increments in LUX (the system has a constant DC gain).

// setup constants
const int ledPin = 6; // LED connected to digital pin 13
const int sensorPin = 1; // connect sensor to analog input 0
float pwm_value = 0; // starts LED-light at 0. 
int anRead = 0; // value received from LDR

void setup() {
  pinMode(ledPin, OUTPUT); // enable output on the led pin
  Serial.begin(9600); // initialize Serial

  // set timer frequency
  // pin = 6, important!
  set_timer_frequency(6, 1);
}

void loop() {
//  // increment PWM
//  if (pwm_value > 255) {
//    exit(0);
//  } else {
//    pwm_value = pwm_value + 20; // increments the PWM 
//  }
  
  // set LED voltage
  analogWrite(ledPin, pwm_value);

  // waits a second
  delay(4000);
  
  // reads value from LDR
  anRead = analogRead(sensorPin);

  // prints the lux value to the console
  double lux = pow(10, (-0.0021*anRead + 5.1431 - 5.7747)/(-0.7757));
  Serial.println(lux);
  
  // prints voltage
  float voltage = 5 * pwm_value/255;
  Serial.println(voltage);
  


 
  
}
