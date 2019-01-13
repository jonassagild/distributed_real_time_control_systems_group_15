//
//  Controller.cpp
//  
//
//  Created by Jonas Åsnes  Sagild on 23.10.2018.
//

#include "Controller.hpp"
//#include <Wire.h>

Controller::Controller(bool feedforward, bool feedback, float k_p, float k_d, float k_i, float i_max, float i_min ,float initial_lux_set_point, float end_lux_set_point, int index){
    // set internal parameters
    _feedforward = feedforward;
    _feedback = feedback;
    _k_p = k_p;
    _k_d = k_d;
    _k_i = k_i;
    _i_max = i_max;
    _i_min = i_min;
    
    
    _initial_lux_set_point = initial_lux_set_point;
    _end_lux_set_point = end_lux_set_point;
    _index = index;
    
    // set initial value
    
    
    if(_index == 2) {
        double initial_pwm_forward_duty = ((initial_lux_set_point-2.7257)/0.3014);
        analogWrite(_led_pin, initial_pwm_forward_duty); // initial value
    } else {
        double initial_pwm_forward_duty = ((initial_lux_set_point-12.32)/0.1701);
        analogWrite(_led_pin, initial_pwm_forward_duty); // initial value
    }
    // wait for LDR (and LED) to stabilize
    delay(100000);
    
    if (_index == 1){
        _anread_set_point = (-0.7880*log10(_end_lux_set_point) + 6.0989 - 5.1786 ) / (-0.0023);
    }else{
        _anread_set_point =  (-0.7534*log10(_end_lux_set_point) + 5.7523 - 5.0762 ) / (-0.0020);
    }

    // set values for optimization
    _k_integral = _k_p*_k_i*_t*0.5;
}

void Controller::control() {
    
    // variables for calculating comfort error
    _comfort_error = 0;
    double temp_comfort_error;
    
    // total time in seconds since restart
    unsigned long total_time = millis()*1000;
    
    _i = 0;
    while(true){
        _i = _i + 1;
        // get start time of iteration
        _start_time = millis();
        
        // if it's time to measure data
        if (_i % _iterations_between_measurement == 0){
            //if plotting of values
            if(_measure_anread){
                if (_i < _iterations_between_measurement*_number_of_measure_points) {
                    _lux_values[_i/_iterations_between_measurement-1] = _measured_anread;
                }
            }
            
            /***
            // if i2c
            if (false) {
                
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
            ***/
            
        }
        
        /* FEED-FORWARD CONTROLLER*/
        if (_feedforward) {
            if(_index == 2) {
                // change formulas to be dependent on _anread_set_point
                _pwm_forward_duty = ((_end_lux_set_point-2.7257)/0.3014);
            } else {
                _pwm_forward_duty = ((_end_lux_set_point-12.32)/0.1701);
            }
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
        //send_i2c_duty_cycle(_pwm_total_duty, _index); // TESTING
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
        /*** CONSENSUS
        _end_lux_set_point = consens();
        
        if (_index == 1){
            _anread_set_point =  (-0.7880*log10(_end_lux_set_point)  + 6.0989 - 5.1786) / (-0.0023); //
        }else{
            _anread_set_point =  (-0.7534*log10(_end_lux_set_point)  + 5.7523 - 5.0762) / (-0.0020); //
        }
        ***/
        // gets measured lux
        _measured_anread = analogRead(_sensor_pin);
        //send_i2c_anread(_measured_anread, _index); // TESTING
        
        /***
        // comfort error
        //TODO Change to lux. Now in analog read values.
        if(_anread_set_point > _measured_anread){
            temp_comfort_error = temp_comfort_error + (_anread_set_point - _measured_anread);
            _comfort_error = temp_comfort_error/_i;
            //send_i2c_accumulated_comfort_error(_comfort_error, _index);
        }
        
        send_i2c_elapsed_time(millis(), _index);
        ***/
    }
    
}


void Controller::send_i2c_elapsed_time(unsigned long time, int index){
    char temp[3];
    dtostrf(time, 3, 0, temp);
    
    char message [5];
    for (int i = 2; i < 5; i++) {
        message[i] = temp[i-2];
    }
    
    if(index == 1){
        message[0] = '1';
    }else{
        message[0] = '2';
    }
    message[1] = 't';
    
    
    for (int i = 0; i <5; i++) {
        Serial.print(message[i]);
    }
    Serial.println("");
   
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(message, 5);
    Wire.endTransmission();
    
    
}


//OK
void Controller::send_i2c_duty_cycle(double duty_cycle, int index){
    char temp[3];
    dtostrf(duty_cycle, 3, 0, temp);
    
    char duty_cycle_message [5];
    for (int i = 2; i < 5; i++) {
        duty_cycle_message[i] = temp[i-2];
        }
    if(index == 1){
        duty_cycle_message[0] = '1';
    }else{
        duty_cycle_message[0] = '2';
    }
    duty_cycle_message[1] = 'd';
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(duty_cycle_message, 5);
    Wire.endTransmission();
}


//OK
void Controller::send_i2c_anread(double anread, int index){
    char temp[3];
    dtostrf(anread, 3, 0, temp);
    
    char message [5];
    for (int i = 2; i < 5; i++) {
        message[i] = temp[i-2];
    }
    if(index == 1){
        message[0] = '1';
    }else{
        message[0] = '2';
    }
    message[1] = 'l';
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(message, 5);
    Wire.endTransmission();
    
    for (int i = 0; i <5; i++) {
        Serial.print(message[i]);
    }
    Serial.println(" ");
    
}

void Controller::send_i2c_accumulated_comfort_error(double comfort_error, int index){
    char temp[3];
    dtostrf(comfort_error, 3, 0, temp);
    
    char message [5];
    for (int i = 2; i < 5; i++) {
        message[i] = temp[i-2];
    }
    
    if(index == 1){
        message[0] = '1';
    }else{
        message[0] = '2';
    }
    message[1] = 'e';
    
    /*
    for (int i = 0; i <5; i++) {
        Serial.print(message[i]);
    }
    Serial.println(" ");
    */
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(message, 5);
    Wire.endTransmission();
}

void Controller::enable_i2c(int master_address, int slave_address){
    _i2c_master_address = master_address;
    _i2c_slave_address = slave_address;
    _i2c_enabled = true;
    //Wire.begin(_i2c_master_address);
    //Wire.onReceive(receive_i2c_message); //event handler
    //TWAR = (_i2c_master_address << 1) | 1; // enable broadcasts to be received
}

static void Controller::receive_i2c_message(int how_many){
    while (Wire.available()> 0) { // check data on BUS
        char c = Wire.read(); //receive byte at I2C BUS
        //Serial.println("the program shut down");
        //if (c == 's'){
        //    exit(0);
        //}
    }
}

void Controller::set_sampling_interval(int sampling_interval){
    _sampling_interval = sampling_interval;
}

void Controller::set_measure_anread(bool measure_anread){
    _measure_anread = measure_anread;
}

void Controller::print_matlab_code(){
    Serial.print("function lux_values = values\n");
    delay(100);
    Serial.print("anread_values = [");
    delay(1000);
    Serial.print(_lux_values[0]);
    for (int k = 1; k < _number_of_measure_points; k++) {
        Serial.print(",");
        Serial.print(_lux_values[k]);
        delay(1000);
    }
    Serial.print("];");
    //Serial.print("\nlux_values = 10.^((((-0.0020).*anread_values)-0.6761)/(-0.7534));");
    //Serial.print("\nend");
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

volatile double k_11, k_12, k_21, k_22; // K – Coupling matrix (dim 2 by 2)
double l; // l – columns vector with lower bound illuminances (dim n)
double o; // o – columns vector of external illuminances (dim n)
double c; // c - the vector of all energy costs
double rho; // Solve with consensus. ρ = global optimization parameter?
char _chars[11]; // used to transfer message (d1;d2)
volatile bool _received_new_data = true; // true if we've received new data since last iteration
int _i2c_master_address = 26;
int _i2c_slave_address = 0;

// bool
volatile bool is_other_node_ready = false;
volatile bool is_other_node_ready_to_set_gain = false;
volatile bool send_is_ready_node2 = false;
//TEST
volatile bool is_coupling_gains_set = false;
volatile bool node2_ready_to_set_gain = false;
volatile bool node1_ready_to_set_gain = false;

// Declare node
Node node;

/* START help methods */
double vectorNorm(double vector[]) {
    return pow(vector[0], 2) + pow(vector[1], 2);
}

double *constant_times_vector(double *vector, double constant){
    static double new_vector[2];
    for (int i = 0; i < 2; i++) {
        new_vector[i] = constant * vector[i];
    }
    return new_vector;
}

double *copy_vector(double *vector){
    static double new_vector[2];
    for (int i = 0; i < 2; i++) {
        new_vector[i] = vector[i];
    }
    return new_vector;
}

double *vector_addition(double* vector1, double* vector2){
    static double new_vector[2];
    for (int i = 0; i < 2; i++) {
        new_vector[i] = vector1[i] + vector2[i];
    }
    return new_vector;
}

double *vector_subtraction(double* vector1, double* vector2){
    static double new_vector[2];
    for (int i = 0; i < 2; i++) {
        new_vector[i] = vector1[i] - vector2[i];
    }
    return new_vector;
}

double inner_product(double* vector1, double* vector2){
    double result = 0;
    for (int i = 0; i < 2; i++) {
        result = result + vector1[i] * vector2[i];
    }
    
    return result;
}

double evaluate_cost(Node node, double *d, double rho){
    
    double d_minus_d_av[2];
    for (int i = 0; i < 2; i++) {
        d_minus_d_av[i] = d[i] - node.d_av[i];
    }
    
    double cost = (node.c * d[node.index-1]) + inner_product(node.y, d_minus_d_av) + (rho/2)*vectorNorm(d_minus_d_av);
    
    return cost;
}

double check_feasibility(Node node, double *d){
    double tol = 0.001; // tolerance for rounding errors
    double check;
    if (d[node.index-1] < 0 - tol){
        return check = 0;
    }
    else if(d[node.index-1] > 100+tol ){
        return check = 0;
    }
    else if (inner_product(d, node.k) < node.l-node.o-tol){
        return check = 0;
    }
    return check = 1;
    
}
/* END help methods */

Res primal_solve(Node node, double rho){
    /*
     The primal iterate.
     To solve the problem we have to iterate on:
     • the computation of the primal variables di
     • the computation of the average of the primal variables of all nodes d_i and
     • the dual variables yi
     */
    
    
    Res res;
    double *pointer; // Use the same pointer several times
    
    // Initialize variables
    double d_best[2] = {-1, -1};
    double cost_best = 10000000;
    double d[2];
    double z[2];
    
    // calculate z
    for (int i = 0; i < 2; i++) {
        z[i] = node.d_av[i] * rho - node.y[i];
    }
    z[node.index-1] = z[node.index-1] - node.c;
    
    
    // "UNCONSTRAINED" OPTIMIZATION, WHERE THE CONSTRAINTS ARE EMBEDDED IN THE INDIVIDUAL COST FUNCTIONS.
    pointer = constant_times_vector(z, (1/rho));
    double d_u[2];
    d_u[0] = pointer[0];
    d_u[1] = pointer[1];
    // check feasibility of unconstrained
    double sol_unconstrained = check_feasibility(node, d_u);
    if (sol_unconstrained == 1) {
        double cost_uncostrained = evaluate_cost(node, d_u, rho);
        if (cost_uncostrained < cost_best){
            d[0] = d_u[0];
            d[1] = d_u[1];
            double cost = cost_uncostrained;
            res.cost_best = cost;
            res.d_best0 = d[0];
            res.d_best1 = d[1];
            return res;
            //IF UNCONSTRAINED SOLUTION EXISTS, THEN IT IS OPTIMAL NO NEED TO COMPUTE THE OTHER
        }
    }
    
    // COMPUTE MINIMUM CONSTRAINED TO LINEAR BOUNDARY
    pointer = constant_times_vector(z, 1/rho);
    double d_bl_1[2];
    d_bl_1[0] = pointer[0]; // Assigns value of
    d_bl_1[1] = pointer[1];
    pointer = constant_times_vector(node.k, 1/(node.n) * (node.o-node.l + ((1/rho) * inner_product(z, node.k))));
    double d_bl_2[2];
    d_bl_2[0] = pointer[0];
    d_bl_2[1] = pointer[1];
    pointer = vector_subtraction(d_bl_1, d_bl_2);
    double d_bl[2];
    d_bl[0] = pointer[0];
    d_bl[1] = pointer[1];
    
    // check feasibility of minimum constrained to linear boundary
    double sol_boundary_linear = check_feasibility(node, d_bl);
    // compute cost and if best store new optimum
    if (sol_boundary_linear == 1) {
        double cost_boundary_linear = evaluate_cost(node, d_bl, rho);
        if (cost_boundary_linear < cost_best){
            d_best[0] = d_bl[0];
            d_best[1] = d_bl[1];
            cost_best = cost_boundary_linear;
        }
    }
    
    //COMPUTE MINIMUM CONSTRAINEDTO 0 BOUNDARY
    pointer = constant_times_vector(z, 1/(rho));
    double d_b0[2];
    d_b0[0] = pointer[0];
    d_b0[1] = pointer[1];
    d_b0[node.index-1] = 0;
    // check feasibility of minimum constrained to 0 boundary
    double sol_boundary_0 = check_feasibility(node, d_b0);
    if (sol_boundary_0 == 1){
        double cost_boundary_0 = evaluate_cost(node, d_b0, rho);
        if (cost_boundary_0 < cost_best){
            d_best[0] = d_b0[0];
            d_best[1] = d_b0[1];
            cost_best = cost_boundary_0;
            
        }
    }
    
    // COMPUTE MINIMUM CONSTRAINED TO 100 BOUNDARY
    pointer = constant_times_vector(z, 1/(rho));
    double d_b1[2];
    d_b1[0] = pointer[0];
    d_b1[1] = pointer[1];
    d_b1[node.index-1] = 100;
    // check feasibility of minimum constrained to 100 boundary
    double sol_boundary_100 = check_feasibility(node, d_b1);
    if (sol_boundary_100 == 1){
        double cost_boundary_100 = evaluate_cost(node, d_b1, rho);
        if (cost_boundary_100 < cost_best){
            d_best[0] = d_b1[0];
            d_best[1] = d_b1[1];
            cost_best = cost_boundary_100;
        }
    }
    
    // COMPUTE MINIMUM CONSTRAINED TO LINEAR AND 0 BOUNDARY
    pointer = constant_times_vector(z, 1/(rho));
    double d_l0_1[2];
    d_l0_1[0] = pointer[0];
    d_l0_1[1] = pointer[1];
    pointer = constant_times_vector(node.k, (1/node.m)*(node.o-node.l));
    double d_l0_2[2];
    d_l0_2[0] = pointer[0];
    d_l0_2[1] = pointer[1];
    pointer = constant_times_vector(node.k,(1/rho/node.m)*((node.k[node.index-1]*z[node.index-1])-inner_product(z,node.k)));
    double d_l0_3[2];
    d_l0_3[0] = pointer[0];
    d_l0_3[1] = pointer[1];
    double d_l0[2];
    d_l0[0] = d_l0_1[0] - d_l0_2[0] + d_l0_3[0];
    d_l0[1] = d_l0_1[1] - d_l0_2[1] + d_l0_3[1];
    d_l0[node.index-1] = 0;
    
    // check feasibility of minimum constrained to linear and 0 boundary
    double sol_linear_0 = check_feasibility(node, d_l0);
    //compute cost and if best store new optimum
    if (sol_linear_0 == 1){
        double cost_linear_0 = evaluate_cost(node, d_l0, rho);
        if (cost_linear_0 < cost_best){
            d_best[0] = d_l0[0];
            d_best[1] = d_l0[1];
            cost_best = cost_linear_0;
        }
    }
    
    //COMPUTE MINIMUM CONSTRAINED TO LINEAR AND 100 BOUNDARY
    pointer = constant_times_vector(z, 1/(rho));
    double d_l1_1[2];
    d_l1_1[0] = pointer[0];
    d_l1_1[1] = pointer[1];
    pointer = constant_times_vector(node.k, (1/node.m)*(node.o-node.l+100*node.k[node.index-1]));
    double d_l1_2[2];
    d_l1_2[0] = pointer[0];
    d_l1_2[1] = pointer[1];
    pointer = constant_times_vector(node.k,(1/rho/node.m)*((node.k[node.index-1]*z[node.index-1])-inner_product(z,node.k)));
    double d_l1_3[2];
    d_l1_3[0] = pointer[0];
    d_l1_3[1] = pointer[1];
    double d_l1[2];
    d_l1[0] = d_l1_1[0] - d_l1_2[0] + d_l1_3[0];
    d_l1[1] = d_l1_1[1] - d_l1_2[1] + d_l1_3[1];
    d_l1[node.index-1] = 100;
    // check feasibility of minimum constrained to linear and 0 boundary
    double sol_linear_1 = check_feasibility(node, d_l1);
    if (sol_linear_1 == 1){
        double cost_linear_1 = evaluate_cost(node, d_l1, rho);
        if (cost_linear_1 < cost_best){
            d_best[0] = d_l1[0];
            d_best[1] = d_l1[1];
            cost_best = cost_linear_1;
        }
    }
    
    res.cost_best = cost_best;
    
    if( d_best[0] < 0){
        d_best[0] = 0;
    }
    if(d_best[1] < 0){
        d_best[1] = 0;
    }
    
    res.d_best0 = d_best[0];
    res.d_best1 = d_best[1];
    return res;
}

void initialize_system(double _l, double _o, double _c, double _rho, double i2c_base_address, int _index){
    // Coupling matrix (dim 2 by 2)
    node.index = _index;
    
    // lower bound illuminance. double l1 = 80, l2 = 270;
    l = _l;
    // external illuminance double o1 = 50, o2 = 50;
    o = _o;
    // Energy "costs" at each desk i double c1 = 1, c2 = 1;
    c = _c;
    // Solve with consensus.
    rho = _rho;
    
    _i2c_master_address = i2c_base_address + _index;
    _i2c_slave_address = 0;
    //_i2c_slave_address = i2c_base_address + (3 - _index);
    
    Wire.begin(_i2c_master_address);
    Wire.onReceive(receive_i2c_message); //event handler
    TWAR = (_i2c_master_address << 1) | 1; // enable broadcasts to be received
    
    // RPI SEND VALUES
    send_i2c_get_current_external_illuminance(_o, _index);
    send_i2c_current_occupancy_state(_l, _index);
    send_i2c_current_illuminance_lower_bound(_l, _index);
    
    
    initailize_gains(_index);
    initialize_node(_index);
    delay(3000);
}

void initialize_node(int index){
    // initialize node values
    if (index == 1) {
        node.k[0] = k_11;
        node.k[1] = k_12;
        node.m = node.n - pow(k_11, 2);
        
    } else {
        node.k[0] = k_21;
        node.k[1] = k_22;
        node.m = node.n - pow(k_22, 2);
    }
    
    node.n = vectorNorm(node.k);
    node.c = c;
    node.o = o;
    node.l = l;
}

void send_i2c_message(double d1, double d2){
    // send own locally optimized dimmings
    
    Wire.beginTransmission(_i2c_slave_address);
    dtostrf(d1, 3, 4, _chars);
    Wire.write(_chars, 8);
    Wire.write(';');
    dtostrf(d2, 3, 4, _chars);
    Wire.write(_chars, 8);
    Wire.endTransmission();
    
    
}

bool is_message_ready_message_node1(char message){
    if (message == 'X') {
        return true;
    } else {
        return false;
    }
}

bool is_message_ready_message_node2(char message){
    if (message == 'Y') {
        return true;
    } else {
        return false;
    }
}

void send_is_ready_i2c_message_node1(){
    Serial.print("NODE1 sending\n");
    
    //TEST
    char _ready_message[3] = {'1', 'R', 'X'};
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(_ready_message, 3);
    Wire.endTransmission();
    
}

void send_is_ready_i2c_message_node2(){
    Serial.print("NODE2 sending\n");
    
    //TEST
    // index, message type, message
    char _ready_message[3] = {'2', 'R', 'Y'};
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(_ready_message, 3);
    Wire.endTransmission(); // Crash here!
    
}

void receive_i2c_message(int how_many){
    // check if message is ready  to set gains signal
    if (is_other_node_ready_to_set_gain == false) {
        char c;
        int i = 0;
        while (Wire.available() > 0) { // check data on BUS
            c = Wire.read();
            Serial.print(c);
        }
        if (node.index == 2) {
            if (is_message_ready_message_node1(c)) { // Check if message from node 1 is ready message
                send_is_ready_node2 = true;
            }
        }else {
            if (is_message_ready_message_node2(c)){
                is_other_node_ready_to_set_gain = true;
            }
        }
        
        // check if coupling gains are set
    }else if (is_coupling_gains_set == false){
        char c;
        while (Wire.available() > 0) { // check data on BUS
            c = Wire.read();
        }
        if (node.index == 2) {
            if (is_message_ready_message_node1(c)) { // Check if message from node 1 is ready message
                delay(1000);
                double k_21_a = analogRead(1);
                k_21 = pow(10, (-0.0020*k_21_a + 5.0762 - 5.7523) / (-0.7534)) / 100; //
                Serial.print("k_21 = ");
                Serial.println(k_21);
                is_coupling_gains_set = true;
                node1_ready_to_set_gain = true;
            }
        }else{
            if (is_message_ready_message_node2(c)) { // Check if message from node 1 is ready message
                delay(1000);
                double k_12_a = analogRead(1);
                k_12= pow(10, (-0.0023*k_12_a + 5.1786 - 6.0989) / (-0.7880))/100; //
                Serial.print("k_12 = ");
                Serial.println(k_12);
                is_coupling_gains_set = true;
            }
        }
    }
    /*
     else if (is_other_node_ready == false) {
     char c;
     while (Wire.available() > 0) { // check data on BUS
     c = Wire.read();
     Serial.println(c);
     }
     if (node.index == 2) {
     if (is_message_ready_message_node1(c)) { // Check if message from node 1 is ready message
     send_is_ready_node2 = true;
     }
     }else {
     if (is_message_ready_message_node2(c)){
     is_other_node_ready = true;
     }
     }
     
     }*/
    else { // else run normal code
        
        int i = 0;
        char _d_1[8];
        char _d_2[8];
        bool _semicolon = false;
        while (Wire.available() > 0) { // check data on BUS
            char c = Wire.read(); //receive byte at I2S BUS
            // index and msg type
            /*
             if(i == 0 && (c == '1' || c == '2')){
             i = i + 1;
             }else if(i == 1 && c == 'G'){
             i = 0;
             }*/
            
            if (c == ';') {
                _semicolon = true;
                i = 0;
                continue;
            }
            if (_semicolon) {
                _d_2[i] = c;
            } else {
                _d_1[i] = c;
            }
            i = i + 1;
        }
        // add data from _d_1 and _d_2 to node.
        node.dim_neighbour[0] = atof(_d_1);
        node.dim_neighbour[1] = atof(_d_2);
        _received_new_data = true;
    }
}

double iterate(){
    Res res;
    res = primal_solve(node, rho);
    node.d[0] = res.d_best0;
    node.d[1] = res.d_best1;
    // update averages
    node.d_av[0] = (node.d[0]+node.dim_neighbour[0])/2;
    node.d_av[1] = (node.d[1]+node.dim_neighbour[1])/2;
    
    // Update local lagrangians
    node.y[0] = node.y[0] + rho*(node.d[0]-node.d_av[0]);
    node.y[1] = node.y[1] + rho*(node.d[1]-node.d_av[1]);
    
    send_i2c_message(node.d[0], node.d[1]);
    
    // Calculate lux to set by controller
    double _end_lux_set_point = node.k[0]* node.d[0] + node.k[1] * node.d[1];
    //Serial.println(_end_lux_set_point);
    return _end_lux_set_point;
}

double consens(){
    // Initialize nodes
    /*
     Serial.println("Check other node");
     while (is_other_node_ready == false){ // wait until node1 is ready
     if (node.index ==  1) {
     send_is_ready_i2c_message_node1(); // send ready message
     }else if (node.index == 2 && send_is_ready_node2 == true){
     is_other_node_ready = true;
     send_is_ready_i2c_message_node2();
     }
     }
     */
    double lux;
    int iterations = 0;
    
    while(true) {
        if (iterations == 10){
            break;
        }
        if (_received_new_data == true){
            _received_new_data = false;
            iterations++;
            lux = iterate();
        }
    }
    return lux;
}

void initailize_gains(int index){
    // Initialize nodes
    while (is_other_node_ready_to_set_gain == false){ // wait until node1 is ready
        if (index ==  1) {
            send_is_ready_i2c_message_node1(); // send ready message
            delay(1000);
        }else if (index == 2 && send_is_ready_node2 == true){
            is_other_node_ready_to_set_gain = true;
            send_is_ready_i2c_message_node2();
        }
    }
    
    if(index == 1){
        analogWrite(6, 255); // Light up node 1.
        
        //TEST
        // index, message type, message
        char _ready_message[3] = {'1', 'G', 'X'};
        
        Wire.beginTransmission(_i2c_slave_address);
        Wire.write(_ready_message, 3);
        Wire.endTransmission();
        delay(1000); // Wait until light is stable
        double k_11_a = analogRead(1);
        k_11 = pow(10, (-0.0023*k_11_a + 5.1786 - 6.0989) / (-0.7880)) / 100; //
        Serial.print("k_11 = ");
        Serial.println(k_11);
        delay(1000); // Wait until read
        analogWrite(6, 0);
    }
    
    if(index == 2){
        // Wait until gain is set in node 1
        while(node1_ready_to_set_gain==false){
        }
        delay(3000);
        analogWrite(6, 255);
        
        //TEST
        // index, message type, message
        char _ready_message[3] = {'2', 'G', 'Y'};
        
        Wire.beginTransmission(_i2c_slave_address);
        Wire.write(_ready_message, 3);
        Wire.endTransmission();
        delay(1000); // Wait until light is stable
        double k_22_a = analogRead(1);
        k_22 = pow(10, (-0.0020*k_22_a +  5.0762 - 5.7523) / (-0.7534)) / 100; //+
        Serial.print("k_22 = ");
        Serial.println(k_22);
        delay(1000); // Wait until read
        analogWrite(6, 0);
    }
    
}

void send_i2c_current_occupancy_state(double l, int index){
    
    char occupancy_value;
    if (l > 60){ //  DESK OCCUPIED. DECALRE VARIABLE WITH THRESHOLD
        occupancy_value = '1';
    }else{
        occupancy_value = '0';
    }
    
    char message [3];
    
    if(index == 1){
        message[0] = '1';
    }else{
        message[0] = '2';
    }
    message[1] = 's';
    message[2] = occupancy_value;
    
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(message, 3);
    Wire.endTransmission();
    
    for (int i = 0; i <3; i++) {
        Serial.print(message[i]);
    }
    Serial.println("");
}

void send_i2c_current_illuminance_lower_bound(double lower_bound_illuminance, int index){
    
    char temp[3];
    dtostrf(lower_bound_illuminance, 3, 0, temp);
    
    char message [5];
    for (int i = 2; i < 5; i++) {
        message[i] = temp[i-2];
    }
    
    if(index == 1){
        message[0] = '1';
    }else{
        message[0] = '2';
    }
    message[1] = 'L';
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(message, 5);
    Wire.endTransmission();
    
    for (int i = 0; i <5; i++) {
        Serial.print(message[i]);
    }
    Serial.println("");
}

void send_i2c_get_current_external_illuminance(double external_illuminance, int index){
    
    char temp[3];
    dtostrf(external_illuminance, 3, 0, temp);
    
    char message [5];
    for (int i = 2; i < 5; i++) {
        message[i] = temp[i-2];
    }
    
    if(index == 1){
        message[0] = '1';
    }else{
        message[0] = '2';
    }
    message[1] = 'o';
    
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write(message, 5);
    Wire.endTransmission();
    
    for (int i = 0; i <5; i++) {
        Serial.print(message[i]);
    }
    Serial.println("");
    
}


