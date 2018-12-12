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
float end_lux = 0;
float start_lux = 0;

void setup() {
  // enable output on the led pin
  pinMode(led_pin, OUTPUT); 
  
  Serial.begin(115200);

    // set timer frequency
  // pin = 6, important!
  set_timer_frequency(6, 1);

  initialize_system(2, 1, 1, 2, 150, 30, 1, 0.07, 25, 1);  
}

void loop() {
if (Serial.available() > 0) {
      char_read = Serial.read();
      start_controlling();
  }
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
  Controller controller(true, true, 1.5, 0, 0.001, start_lux, end_lux, 15000000000000);
  controller.set_sensor_pin(1);
  controller.set_led_pin(6);
  controller.set_sampling_interval(300);
  controller.set_iterations_between_measurement(10);
  controller.set_measure_anread(true);
  controller.set_measure_pwm(true);
  controller.enable_i2c(own_add, 0); // sets master to own_address, and slave to 0
  controller.control();
}
