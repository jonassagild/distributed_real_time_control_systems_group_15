// method performs a step in in LED actuation, and collects the LDR result of the acutation. 
// approximates the response to a first order transfer function. 

// constants
const int ledPin = 6; // LED connected to digital pin 10 
const int sensorPin = 1; // connect sensor to analog input 1 
const int length_list = 300; 

// variables
float initial_lux_value = 30; // starts LED-light at 0. 
float end_lux_value = 5; // end LED-light 
int anRead = 0; // value received from LDR 
int i = 0; 
float lux_values[length_list]; 

// variables to implement sampling interval
unsigned long start_time; // contains start time of iteration
unsigned long end_time; // contains the end time of iteration
unsigned long delay_time; // contains the amount of time to wait
unsigned long iteration_time;
unsigned long interval_time = 30; // the amount of time used on each iteration


void setup() {
  pinMode(ledPin, OUTPUT); // enable output on the led pin
  Serial.begin(9600); // initialize Serial
  
  setPwmFrequency(6, 1);
  // changes frequency of timer 1
  //TCCR1B = (TCCR1B & mask) | prescale;
}

void loop() {
  // put your main code here, to run repeatedly:
  // runs only the first iteration
  if (i == 0) { // saves the initial lux
    // set LED voltage
    int initial_pwm_value = (initial_lux_value-0.42)/0.1846;
    if(initial_pwm_value >255) {
      initial_pwm_value = 255;
    } else if (initial_pwm_value < 0) {
      initial_pwm_value = 0;
    }
    analogWrite(ledPin, initial_pwm_value);
    delay(50000);
    // reads value from LDR
    anRead = analogRead(sensorPin); 
//    // prints the lux value to the console
//    double lux = pow(10, (-0.0021*anRead + 5.1431 - 5.7747)/(-0.7757));
    // not the actual lux_value
    lux_values[0] = anRead;
    // set LED voltage
    int end_pwm_value = (end_lux_value-0.42)/0.1846;
    if(end_pwm_value >255) {
      end_pwm_value = 255;
    } else if (end_pwm_value < 0 ) {
      end_pwm_value = 0;
    }
    analogWrite(ledPin, end_pwm_value);
  }

  // runs each iteration
  start_time = millis();
  anRead = analogRead(sensorPin);
  // not in use anymore
  // double lux = pow(10, (-0.0021*anRead + 5.1431 - 5.7747)/(-0.7757));
  
  i += 1;
  if(i < length_list){
    // this is not real lux values. The lux value is calculated in matlab
    lux_values[i] = anRead;
    end_time = millis();
    iteration_time = end_time - start_time;
    delay_time = interval_time - iteration_time;

    // if iteration_time is too small; print error
    if (iteration_time > interval_time) {
      Serial.print("Delay time: ");
      Serial.println(delay_time);
      Serial.println("Too long calculation time, exiting");
      Serial.print("Time used on last iteration: ");
      Serial.println((end_time - start_time));
      Serial.print("end_time: ");
      Serial.println(end_time);
      Serial.print("start_time: ");
      Serial.println(start_time);
      Serial.print("Iteration number: ");
      Serial.println(i);
      delay(10000);
      exit(0);
    }
    
    delay(delay_time);

   // only the last iteration
   } else {
      Serial.println("Her kommer lista:");
      Serial.print("values = [");
      for(int j=0; j<length_list; j++){
        Serial.print(lux_values[j]);
        Serial.print(" ");
      }
      Serial.print("];");
      delay(10000);
      exit(0);
    }
}
