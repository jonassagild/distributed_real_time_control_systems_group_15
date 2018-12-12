//
//  Controller.cpp
//  
//
//  Created by Jonas Åsnes  Sagild on 23.10.2018.
//

#include "Controller.hpp"
#include <Wire.h>
#include "consensus.hpp"


Controller::Controller(bool feedforward, bool feedback, float k_p, float k_d, float k_i, float initial_lux_set_point, float end_lux_set_point, int number_of_measure_points){
    // set internal parameters
    _feedforward = feedforward;
    _feedback = feedback;
    _k_p = k_p;
    _k_d = k_d;
    _k_i = k_i;
    _initial_lux_set_point = initial_lux_set_point;
    _end_lux_set_point = end_lux_set_point;
    
    // set initial value
    analogWrite(_led_pin, (_initial_lux_set_point-0.42)/0.1846); // initial value
    // wait for LDR (and LED) to stabilize
    delay(100000);
    
    // calculate anread_set_point
    _anread_set_point = (log10(_end_lux_set_point)*(-0.7757)+0.6316)/(-0.0021);
    
    // set values for optimization
    _k_integral = _k_p*_k_i*_t*0.5;
}


void Controller::control() {
    _i = 0;
    while(true){
        _i = _i + 1;
        // get start time of iteration
        _start_time = millis();
        
        
        // if it's time to measure data
        if (_i % _iterations_between_measurement == 0){
            // if i2c
            if (_i2c_enabled) {
                
                // buffer which stores what to print
                char buffer[13] = {};
                // stores the address of the master
                dtostrf(_i2c_master_address, 3, 0, _chars);
                buffer[0] = _chars[0];
                buffer[1] = _chars[1];
                buffer[2] = _chars[2];
                buffer[3] = ',';

                if (_measure_anread) {
                    // converts float to string dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, empty array)
                    dtostrf(_measured_anread_old, 3, 2, _chars);
                    buffer[4] = _chars[0];
                    buffer[5] = _chars[1];
                    buffer[6] = _chars[2];
                    buffer[7] = _chars[3];
                    buffer[8] = _chars[4];
                    buffer[9] = _chars[5];
                }

                if(_measure_pwm) {
                    // coverts in to string
                    dtostrf(_pwm_total_old, 3, 0, _chars);
                    buffer[10] = ',';
                    buffer[11] = _chars[0];
                    buffer[12] = _chars[1];
                    buffer[13] = _chars[2];
                }

                Wire.beginTransmission(_i2c_slave_address);
                Wire.write(buffer, 13);
                Wire.endTransmission();
            }
        }
        
        // gets measured lux
        _measured_anread = analogRead(_sensor_pin);
        
        /* FEED-FORWARD CONTROLLER*/
        if (_feedforward) {
            _pwm_forward_duty = ((_end_lux_set_point-0.42)/0.1846); // move outside of loop
        } else {
            _pwm_forward_duty = 0;
        }
        
        if (_feedback) {
            // calculates error
            _err_value = _anread_set_point - _measured_anread;
            
            // calculates Proportional value
            _p_term = _k_p * _err_value;
            
            // calculates Differential value
            _d_term = (_k_d/(_k_d + _a*_t)) * _d_term_old - ((_k_p*_k_d*_a)/(_k_d+_a*_t))*(_measured_anread-_measured_anread_old);
            
            // calculates the Integral value with TUSTIN
            _i_term = _i_term_old + _k_integral*(_err_value + _err_value_old);
            
            // Prevents integral wind-up
            if (_i_term > _i_max){
                _i_term  = _i_max;
            } else if (_i_term  < _i_min){
                _i_term  = _i_min;
            }
            
            // calculates total backward
            _pwm_backward_duty = _p_term + _i_term + _d_term;
            
            // store old values
            _i_term_old = _i_term;
            _d_term_old = _d_term;
            _err_value_old = _err_value;
        } else {
            _pwm_backward_duty = 0;
        }

        // calculates total pwm
        _pwm_total_duty = _pwm_backward_duty + _pwm_forward_duty; // Total duty
        
        // PWM overflow prevention.
        if (_pwm_total_duty > 255){
            _pwm_total_duty = 255;
        } else if (_pwm_total_duty < 0){
            _pwm_total_duty = 0;
        }
        
        // Assigns the current PWM duty cycle value to pwm_total_old
        _pwm_total_old = _pwm_total_duty;
        
        // Holds the old lux value
        _measured_anread_old = _measured_anread;
        
        // waits until the iteration have used _sampling_interval_time
        _end_time = millis();
        _iteration_time = _end_time - _start_time;
        
        // calculate time to wait
        _delay_time = _sampling_interval - _iteration_time;
        
        // checks if the iteration have used too much time
        if (_iteration_time > _sampling_interval) {
            _print_iteration_time_overflow_info();
            
            break;
        }
        
        // waits until we've used _sampling_interval_time on this iteration
        delay(_delay_time);
        
        // Adjusts the PWM duty cycle
        analogWrite(_led_pin, _pwm_total_duty);

        _end_lux_set_point = consens();
		
		_anread_set_point = (log10(_end_lux_set_point)*(-0.7757) + 0.6316) / (-0.0021);
    }
}


void Controller::enable_i2c(int master_address, int slave_address){
    _i2c_master_address = master_address;
    _i2c_slave_address = slave_address;
    _i2c_enabled = true;
    Wire.begin(_i2c_master_address);
    Wire.onReceive(receive_i2c_message); //event handler
    TWAR = (_i2c_master_address << 1) | 1; // enable broadcasts to be received
}


static void Controller::receive_i2c_message(int how_many){
    while (Wire.available()> 0) { // check data on BUS
        char c = Wire.read(); //receive byte at I2C BUS
        Serial.print("the program shut down");
        if (c == 's'){
            exit(0);
        }
    }
}


void Controller::set_sampling_interval(int sampling_interval){
    _sampling_interval = sampling_interval;
}


void Controller::set_measure_anread(bool measure_anread){
    _measure_anread = measure_anread;
}


void Controller::set_measure_pwm(bool measure_pwm){
    _measure_pwm = measure_pwm;
}


void Controller::set_number_of_measure_points(int number_of_measure_points){
    _number_of_measure_points = number_of_measure_points;
}


void Controller::set_led_pin(int led_pin){
    _led_pin = led_pin;
}


void Controller::set_sensor_pin(int sensor_pin){
    _sensor_pin = sensor_pin;
}


void Controller::set_iterations_between_measurement(int iterations_between_measurement){
    _iterations_between_measurement = iterations_between_measurement;
}


void Controller::_print_iteration_time_overflow_info(){
    Serial.print("Delay time: ");
    Serial.println(_delay_time);
    Serial.println("Too long calculation time, exiting");
    Serial.print("Time used on last iteration: ");
    Serial.println((_end_time - _start_time));
    Serial.print("end_time: ");
    Serial.println(_end_time);
    Serial.print("start_time: ");
    Serial.println(_start_time);
    Serial.print("Iteration number: ");
    Serial.println(_i);
    delay(10000000);
}



