#include "consensus.hpp"
#include "Controller.hpp"
#include "Utilities.hpp"
#include <Wire.h>

// setup constants
const int led_pin = 6;
const int sensor_pin = 1;
const int other_add = 0; //other dev address
int own_add = 24;
char char_read;

// end and start lux
float end_lux = 40;
float start_lux = 5;

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
void setup() {
  // enable output on the led pin
  pinMode(led_pin, OUTPUT); 
  
  Serial.begin(115200);
  //initialize_system(end_lux, 0, 1, 0.07, 25, 1);  // MUST SET INDEX
  set_timer_frequency(6, 1); // pin 6
  
}

void loop() {
  
  start_controlling();
}

void receiveEvent (int howMany) {
  while (Wire.available()> 0) { // check data on BUS
    char char_read = Wire.read(); //receive byte at I2S BUS
    Serial.println(char_read);
    // if start_signal
    if (char_read == 'k')  {
      start_controlling();
    }
  }
}

void start_controlling() {
  // starts the controller
  // (bool feedforward, bool feedback, float k_p, float k_d, float k_i, float initial_lux_set_point, float end_lux_set_point, number_of_measure_points)
  Controller controller(true, true, 1.5, 0, 0.001, start_lux, end_lux, 1); // index of node is the last one // MUST SET INDEX
  controller.set_sensor_pin(1);
  controller.set_led_pin(6);
  controller.set_sampling_interval(300);
  controller.set_iterations_between_measurement(10);
  controller.set_measure_anread(true);
  controller.set_measure_pwm(true);
  controller.enable_i2c(own_add, 0); // sets master to own_address, and slave to 0
  controller.control(); // MUST SET INDEX
}
