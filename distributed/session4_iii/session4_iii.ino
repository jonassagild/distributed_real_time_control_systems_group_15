// Combines the feedback and the feedforward controller

// setup constants
const int ledPin = 6; // LED connected to digital pin 10 
const int sensorPin = 1; // connect sensor to analog input 1 
int i = 0; 
const int length_list = 150; // 300 if measure_actuation is false, 150 if not
float anRead_values[length_list]; 
float pwm_values[length_list];
int time_between_lux_measurement = 40;
bool measure_actuation = true;

//Variables for forward
float pwm_forward_duty;
float pwm_total_duty;

//Variables for feedback
const float Kp = 1.5;    // The value for Proportional gain
const float Ki = 0.001;    // The value for Integral gain
const float Kd = 0;  // The value for Differential gain
float a = 10; // from slides: a typically between 5 and 20. 
float initial_lux_set_point = 25; // initial value
float lux_set_point = 15; // The lux reference point we are aiming to regulate to
float anread_set_point; // calculated by lux_set_point
float iMax = 500;     // Used to prevent integral wind-up
float iMin = -500;    // Used to prevent integral wind-up
float err_value;      // Holds the calculated Error value
float P_Term, I_Term, D_Term;     // Holds the calculated values
float pwm_backward_duty = 0; // Holds the new PWM value
float pwm_total_old = 0; 
float I_Term_old = 0; //Store old integral value
float D_Term_old = 0; //Store old differential value
float err_value_old = 0; //Store old error value
float measured_anRead = 0;
float measured_anRead_old = 0; // Holds the old lux value 

///Variables for clock
unsigned long start_time; 
unsigned long end_time; 
unsigned long delay_time; 
unsigned long iteration_time;
unsigned long sampInterval = 30;  // milliseconds 
float T = 30; //Sampling time

//Optimalization
float K_integral = Kp*Ki*T*0.5;
  
void setup() {
  pinMode(ledPin, OUTPUT); // enable output on the led pin
  Serial.begin(9600); // initialize Serial
  setPwmFrequency(6,1); 

  // set initial value 
  analogWrite(ledPin, (initial_lux_set_point-0.42)/0.1846); // initial value 
  delay(10000);
  
  // calculate anread_set_point
  anread_set_point = (log10(lux_set_point)*(-0.7757)+0.6316)/(-0.0021);

  // for testing purposes
  Serial.println("The process will start now"); 
}

void loop() {
  start_time = millis();
  
  // gets measured lux
  measured_anRead = analogRead(sensorPin); // gets point

  /* FEED-FORWARD CONTROLLER*/
  pwm_forward_duty = ((lux_set_point-0.42)/0.1846);


  /* FEEDBACK CONTROLLER*/
  err_value = anread_set_point - measured_anRead; // calculates error 
  P_Term = Kp * err_value;// This calculates Proportional value, Kp is multiplied with Err_Value and the result is assigned to P_Term
  D_Term = (Kd/(Kd + a*T)) * D_Term_old - ((Kp*Kd*a)/(Kd+a*T))*(measured_anRead-measured_anRead_old);   // Calculates Differential value, BACKWARD
  I_Term = I_Term_old + K_integral*(err_value + err_value_old); // Calculates the Integral value with TUSTIN

  // Prevents integral wind-up
  if (I_Term > iMax){
    I_Term  = iMax;
  } else if (I_Term  < iMin){
    I_Term  = iMin;
  }

  pwm_backward_duty = P_Term + I_Term + D_Term; // Now we have the P_Term, I_Term and D_Term 

  pwm_total_duty = pwm_backward_duty + pwm_forward_duty; // Total duty
  
  // PWM overflow prevention. 
  if (pwm_total_duty > 255){
    pwm_total_duty = 255;
  } else if (pwm_total_duty < 0){
    pwm_total_duty = 0;
  }
  
  I_Term_old=I_Term; //Store old integral value
  D_Term_old=D_Term; //Store old differential value
  err_value_old=err_value; //Store old error value
  measured_anRead_old=measured_anRead; // Holds the old lux value

  if (i == time_between_lux_measurement*length_list){
    // prints the lux value to the console
    Serial.print("data = [");
    delay(10000);
    for(int j=0; j<length_list; j++){
      Serial.print(anRead_values[j]);
      Serial.print(" ");
    }
    delay(10000);
    Serial.print("];");
    Serial.println( );

    // prints pwm values if measure_actuation
    if (measure_actuation) {
      Serial.println();
      Serial.print("pwm_values = [");
      for(int j=0; j<length_list; j++){
        Serial.print(pwm_values[j]);
        Serial.print(" ");
      }
      delay(100);
      Serial.print("];");
    }

    // matlab code
    Serial.println( );
    Serial.print("plot_session_4(data, pwm_values, ");
    Serial.print(sampInterval);
    Serial.print(");");
    Serial.println( );
    
    delay(10000);
    exit(0);  
  }
  else if (i % time_between_lux_measurement == 0) {
    anRead_values[i/time_between_lux_measurement] = measured_anRead;
    if (measure_actuation) {
      pwm_values[i/time_between_lux_measurement] = pwm_total_duty; 
    }
  } 
  
  end_time = millis();
  iteration_time = (end_time - start_time);
  if (iteration_time > sampInterval) {
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

  // calculate delay time
  delay_time = sampInterval - iteration_time;
  i += 1;
  //Serial.println(i);

  
  delay(delay_time);  
  

    
  // Adjusts the PWM duty cycle
  analogWrite(ledPin, pwm_total_duty);

  
  // Assigns the current PWM duty cycle value to pwm_total_old
  pwm_total_old = pwm_total_duty;
  
}
