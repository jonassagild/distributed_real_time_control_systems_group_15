#include "Controller.hpp"
#include "Utilities.hpp"
#include <Wire.h>

// setup constants
const int led_pin = 6;
const int sensor_pin = 1;
const int other_add = 0; //other dev address
int own_add = 6;
// the computer that starts the event is master
bool master = true;

// end and start lux
float end_lux = 45;
float start_lux = 50;
int own_address = 24;


void setup() {
    // enable output on the led pin
    pinMode(led_pin, OUTPUT); 
    // initialize Serial
    Serial.begin(115200);
    
    // set timer frequency
    // pin = 6, important!
    set_timer_frequency(6, 1);


    if (not master){
      own_address = own_address - 1;
      
      // toggles the values
      end_lux = 50;
      start_lux = 45;
      own_add = own_add + 1;
    }

    // set up i2c
    Wire.begin();
    Wire.onReceive(receiveEvent); //event handler
    TWAR = (own_add << 1) | 1; // enable broadcasts to be received
}


void loop() {
  if (master) {
    char c;
    if (Serial.available() > 0) {
      c = Serial.read();
      Wire.beginTransmission(other_add); // gets the bus  
      Wire.write(c); // send byte to address on bus
      Wire.endTransmission(); // releases the bus
      // if start_signal
      if (c == 'k') {      
        Serial.print("kjør");
        start_controlling();
      }
    }
  }
}

void receiveEvent (int howMany) {
  while (Wire.available()> 0) { // check data on BUS
    char c = Wire.read(); //receive byte at I2S BUS
    // if start_signal
    if (c == 'k')  {
      start_controlling();
    }
  }
}


void start_controlling() {
    // starts the controller
    // (bool feedforward, bool feedback, float k_p, float k_d, float k_i, float initial_lux_set_point, float end_lux_set_point, number_of_measure_points)
    Controller controller(true, false, 1.5, 0, 0.001, start_lux, end_lux, 1500);
    controller.set_sensor_pin(1);
    controller.set_led_pin(6);
    controller.set_sampling_interval(300);
    controller.set_iterations_between_measurement(1);
    controller.set_measure_anread(true);
    controller.set_measure_pwm(true);
    controller.enable_i2c(24, 0); // sets master to own_address, and slave to 0
    controller.control();
}
