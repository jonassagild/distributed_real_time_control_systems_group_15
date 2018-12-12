//
//  consensus.hpp
//  distributed
//
//  Created by Jonas Åsnes  Sagild on 28.11.2018.
//  Copyright © 2018 Jonas Åsnes  Sagild. All rights reserved.
//

#ifndef consensus_hpp
#define consensus_hpp

#include "Arduino.h"
#include "math.h"
#include <Wire.h>

struct Node{
    volatile int index; // node index
    volatile double d[2] = {0, 0}; // d – column vector with all normalized dimming (dim n)
    volatile double d_av[2] = {0, 0}; // average dimming level
    volatile double y[2] = {0, 0}; // Lagrange multipliers
    volatile double k[2]; // The coupling gains from the other luminaires to itself, ki
    volatile double n; // norm^2
    volatile double m; // ????
    volatile double c; // the local cost
    volatile double o; // The local external illuminance influence oi
    volatile double l; // The local illuminance lower bound (occupancy state) Li
    volatile double dim_neighbour[2] = {0, 0}; // the others neighbours dim
};

// struct containing results from primal_solve
struct Res {
    double d_best0;
    double d_best1;
    double cost_best;
};


/*
 initialize_system
 input
 - int master_address: own address, needs to be unique
 - int slave_address: address to send data to
 */
void initialize_system(double _k_11, double _k_12, double _k_21, double _k_22, double _l, double _o, double _c, double _rho, double i2c_base_address, int index);

/*
 initialize_node
 input
 
 */
void initialize_node(int index);


/*
 evaluate_cost
 input
 
 */
double evaluate_cost(Node node, double *d, double rho);


double check_feasibility(Node node, double *d);

/*
 primal_solve
 input
 */
Res primal_solve(Node node, double rho);

double consens();
/*
 consens
 input
 */

//* START help methods */
double vectorNorm(double vector[]);
double *constant_times_vector(double *vector, double constant);
double *copy_vector(double *vector);
double *vector_addition(double* vector1, double* vector2);
double *vector_subtraction(double* vector1, double* vector2);
double inner_product(double* vector1, double* vector2);
double iterate();
void send_i2c_message(double d1, double d2);
void receive_i2c_message(int how_many);

bool receive_is_ready_i2c_message();
/* END help methods */

/* TEST methods */
void send_pingpong_i2c();
void send_pongping_i2c();
void test_pingpong();
void send_is_ready_i2c_message_node2();
void send_is_ready_i2c_message();
void initailize_gains(int index);
#endif /* consensus_hpp */
