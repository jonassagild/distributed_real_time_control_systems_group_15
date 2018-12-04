#include "Controller.hpp"
#include "Utilities.hpp"

// setup constants
const int led_pin = 6;
const int sensor_pin = 1;


void setup() {
    pinMode(led_pin, OUTPUT); // enable output on the led pin
    Serial.begin(9600); // initialize Serial

    // set timer frequency
    set_timer_frequency(6, 1);
}

void loop() {
  // (bool feedforward, bool feedback, float k_p, float k_d, float k_i, float initial_lux_set_point, float end_lux_set_point, number_of_measure_points)
  Controller controller(true, true, 1.5, 0, 0.001, 10, 20, 150);
  controller.set_sampling_interval(40);
  controller.set_iterations_between_measurement(1);
  controller.set_measure_anread(true);
  controller.set_measure_pwm(true);
  controller.control();
  float *anread_values = controller.get_measured_anread_values();
  float *pwm_values = controller.get_measured_pwm();

  print_matlab_code(pwm_values, anread_values);
  
  exit(0);

}

void print_matlab_code(float *pwm_values, float *anread_values) {
  
  Serial.print("data = [");
  delay(10000);
  for(int j=0; j<150; j++){
    Serial.print(anread_values[j]);
    Serial.print(" ");
  }
  delay(10000);
  Serial.print("];");
  Serial.println( );

  Serial.println();
  Serial.print("pwm_values = [");
  for(int j=0; j<150; j++){
    Serial.print(pwm_values[j]);
    Serial.print(" ");
  }
  delay(100);
  Serial.print("];");
  
  // matlab code
  Serial.println( );
  Serial.print("plot_session_4(data, pwm_values, ");
  Serial.print(40);
  Serial.print(");");
  Serial.println( );
  
  delay(100000);
}
