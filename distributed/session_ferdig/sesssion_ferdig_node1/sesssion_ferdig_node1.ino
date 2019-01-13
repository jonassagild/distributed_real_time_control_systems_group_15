#include "Controller.hpp"
#include "Utilities.hpp"
#include <Wire.h>

// setup constants
const int led_pin = 6;
const int sensor_pin = 1;
const int other_add = 0; //other dev address
int own_add = 24;
char char_read;

// Run one arduino for _iterations_between_measurement*_number_of_measure_points saving _number_of_measure_points to a list. 
// Then print the data such that it is easy to use in matlab
 


// SET NODE INDEX HERE
int index = 1;

// DESIRED ILLUMINANCE AT DESK i
float end_lux = 45;
float start_lux = 5;


void setup() {
  // enable output on the led pin
  pinMode(led_pin, OUTPUT); 
  
  Serial.begin(115200);

  initialize_system(end_lux, 0, 1, 0.07, 25, index);  // MUST SET INDEX
  set_timer_frequency(6, 1); // pin 6
  
}

void loop() {


  start_controlling();
  /*
  char c;
  
  if (Serial.available() > 0) {
    c = Serial.read();
    if (c == 's') {
      start_controlling();
      }
  }
  */
}

void start_controlling() {
  // starts the controller
  // (bool feedforward, bool feedback, float k_p, float k_d, float k_i, float initial_lux_set_point, float end_lux_set_point, node index)
  int number_measurements = 250;
   
  Controller controller(true, true, 1.5, 0, 0.001, 500, -500, start_lux, end_lux, index); // index of node is the last one // MUST SET INDEX
  controller.set_sensor_pin(1);
  controller.set_led_pin(6);
  controller.set_sampling_interval(100);
  controller.set_iterations_between_measurement(10);
  controller.set_number_of_measure_points(number_measurements);
  controller.set_measure_anread(true);
  controller.set_measure_pwm(false);
  //controller.enable_i2c(26, 0); // sets master to own_address, and slave to 0
  controller.control(); // MUST SET INDEX
  delay(100000);
  controller.print_matlab_code();
  delay(1000000000000000);
}




/* NODE 1:
  kp = 1.5
  kd = 0
  ki = 0.001 
  imax = 500
  imin = -500

  NODE 2:
  kp = 0.3
  kd = 0
  ki = 0.003
  imax = 100
  imin = -50
*/
