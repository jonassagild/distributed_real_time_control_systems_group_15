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
    int index; // node index
    double d[2] = {0, 0}; // d – column vector with all normalized dimming (dim n)
    double d_av[2] = {0, 0}; // average dimming level
    double y[2] = {0, 0}; // Lagrange multipliers
    double k[2]; // The coupling gains from the other luminaires to itself, ki
    double n; // norm^2
    double m; // ????
    double c; // the local cost
    double o; // The local external illuminance influence oi
    double l; // The local illuminance lower bound (occupancy state) Li
    volatile double dim_neighbour[2]; // the others neighbours dim
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
void initialize_system(double _k_11, double _k_12, double _k_21, double _k_22, double _l, double _o, double _c, double _rho, double i2c_master_address);

/*
 initialize_node
 input
 
 */
void initialize_node();


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

void consens();
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
void iterate();
void send_i2c_message(double d1, double d2);
void receive_i2c_message(int how_many);
/* END help methods */



#endif /* consensus_hpp */
