//
//  Controller.hpp
//  
//
//  Created by Jonas Åsnes  Sagild on 23.10.2018.
//

#ifndef Controller_hpp
#define Controller_hpp

#include "Arduino.h"


class Controller {
public:
    /*
     Controller
     input
     - bool feedforward: true if controller should use feedforward
     - bool feedback: true if controller should use feedback
     - float k_p: value of proportional constant
     - float k_d: value of derivative constant
     - float k_i: value of integral constant
     - float initial_lux_set_point: value of the initial lux set point
     - float end_lux_set_point: value of the end lux set point
     
     Constructs the Controller object.
    */
    Controller(bool feedforward, bool feedback, float k_p, float k_d, float k_i, float initial_lux_set_point, float end_lux_set_point, int index);
    
    /*
     control
     Starts a loop that will control the LED. The control will quit after 300 iterations, or the value of number_of_measure_points * iterations_between_measurement.
     */
    void control(int index);
    
    /*
     set_i2c
     enables i2c transfer
     input
     - int master_address: own address, needs to be unique
     - int slave_address: address to send data to
     */
    void enable_i2c(int master_address, int slave_address);
    
    
    /*
     receive_i2c_message
     Receives messages sent on broadcast, or to _i2c_master_address.
     - int how_many: we do not know what this does.
     */
    static void receive_i2c_message(int how_many);
    
    
    /*
     set_sampling_interval
     input
     - int sampling_interval:
     */
    void set_sampling_interval(int sampling_interval);
    
    /*
     set_measure_actuation
     input
     - bool measure_actuation: true if you want to measure actuation, false if not
     */
    void set_measure_anread(bool measure_anread);
    
    
    /*
     set_measure_pwm
     input
     - bool measure_pwm: true if you want to measure pwm, false if not
     */
    void set_measure_pwm(bool measure_pwm);
    
    /*
     set_number_of_measure_points
     input
     - int number_of_measure_points: the number of measure points you want. Actuation and pwm will have the same number of points. 150 is deafault, and recommended if measuring both actuation and pwm. 300 is recommended if only measuring one variable.
     */
    void set_number_of_measure_points(int number_of_measure_points);
    
    /*
     set_led_pin
     input
     - int led_pin: The pin to control
     */
    void set_led_pin(int led_pin);
    
    /*
     set_sensor_pin
     input
     - int sensor_pin: The sensor pin to read values from
     */
    void set_sensor_pin(int sensor_pin);
    
    /*
     set_iterations_between_measurement
     input
     - int iterations_between_measurement: the number of iterations between each measurement. Default is 1. E.g. if set to 3, only every third iterations would be measured.
     */
    void set_iterations_between_measurement(int iterations_between_measurement);
    
private:
    /*
     _print_iteration_time_overflow_info
     prints information about iteration time overflow to Serial.
     */
    void _print_iteration_time_overflow_info();
    
    // parameters
    int _led_pin = 6;
    int _sensor_pin = 1;
    int _number_of_measure_points = 150;
    int _iterations_between_measurement = 1;
    int _sampling_interval = 30;
    bool _measure_pwm = false;
    bool _measure_anread = false;
    bool _feedforward;
    bool _feedback;
    float _k_p;
    float _k_d;
    float _k_i;
    float _initial_lux_set_point;
    float _end_lux_set_point;
    
    // variables
    int _i = 0; // iteration number
    int _i2c_master_address; // master address
    int _i2c_slave_address; // slave address
    char _chars[10]; // used to transfer float
    bool _i2c_enabled = false;
    float _a = 10; // from slides: a typically between 5 and 20.
    float _anread_set_point = 0; // calculated by lux_set_point
    float _i_max = 500;     // Used to prevent integral wind-up
    float _i_min = -500;    // Used to prevent integral wind-up
    float _err_value;      // Holds the calculated Error value
    float _pwm = 0; // Holds the new PWM value
    float _pwm_old = 0;
    float _i_term_old = 0; //Store old integral value
    float _d_term_old = 0; //Store old differential value
    float _err_value_old = 0; //Store old error value
    float _measured_anread = 0;
    float _measured_anread_old = 0; // Holds the old lux value
    float _t = _sampling_interval; // Sampling time
    float _p_term, _i_term, _d_term = 0;
    float _pwm_forward_duty = 0;
    float _pwm_backward_duty = 0;
    float _pwm_total_duty = 0;
    float _pwm_total_old = 0;
    unsigned long _start_time; // variables for clock
    unsigned long _end_time;
    unsigned long _delay_time;
    unsigned long _iteration_time;
    
    // optimization
    float _k_integral = 0;
    
    
    
};

#endif /* Controller_hpp */
