//
//  consensus.cpp
//  distributed
//
//  Created by Jonas Åsnes  Sagild on 28.11.2018.
//  Copyright © 2018 Jonas Åsnes  Sagild. All rights reserved.
//

#include "consensus.hpp"


double k_11, k_12, k_21, k_22; // K – Coupling matrix (dim 2 by 2)
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
    res.d_best0 = d_best[0];
    res.d_best1 = d_best[1];
//    Serial.println("calc costs:");
//    Serial.print("d_best[0] =");
//    Serial.print(d_best[0]);
//    Serial.print("   d_best[1] =");
//    Serial.println(d_best[1]);
    return res;
}

void initialize_system(double _k_11, double _k_12, double _k_21, double _k_22, double _l, double _o, double _c, double _rho, double i2c_base_address, int _index){
    // Coupling matrix (dim 2 by 2)
    k_11 = _k_11;
    k_12 = _k_12;
    k_21 = _k_21;
    k_22 = _k_22;
    // lower bound illuminance. double l1 = 80, l2 = 270;
    l = _l;
    // external illuminance double o1 = 50, o2 = 50;
    o = _o;
    // Energy "costs" at each desk i double c1 = 1, c2 = 1;
    c = _c;
    // Solve with consensus.
    rho = _rho;
    
    _i2c_master_address = i2c_base_address + _index;
    _i2c_slave_address = i2c_base_address + (3 - _index);
    
    Wire.begin(_i2c_master_address);
    Wire.onReceive(receive_i2c_message); //event handler
    TWAR = (_i2c_master_address << 1) | 1; // enable broadcasts to be received
    
    initialize_node(_index);
}

void initialize_node(int index){
    // initialize node values
    if (index == 1) {
        node.index = 1;
        node.k[0] = k_11;
        node.k[1] = k_12;
        node.m = node.n - pow(k_11, 2);

    } else {
        node.index = 2;
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

bool is_message_ready_message(char message){
    if (message == 'X') {
        return true;
    } else {
        return false;
    }
}

void send_is_ready_i2c_message(){
    //TODO mutex?
    Serial.print("Sending im ready message\n");
    Wire.beginTransmission(_i2c_slave_address);
    Wire.write('X');
    Serial.print("before \n");
    Wire.endTransmission(); // Crash here!
    Serial.print("after \n");

}


void receive_i2c_message(int how_many){
    // check if message is ready signal
    if (not is_other_node_ready) {
        while (Wire.available() > 0) { // check data on BUS
            char c = Wire.read(); //receive byte at I2S BUS
            Serial.print(c);
            if (is_message_ready_message(c)) {
                if (node.index == 2) {
                    send_is_ready_i2c_message();
                }
                is_other_node_ready = true;
            }
        }
    } else { // else run normal code
        int i = 0;
        char _d_1[8];
        char _d_2[8];
        bool _semicolon = false;
        while (Wire.available()> 0) { // check data on BUS
            char c = Wire.read(); //receive byte at I2S BUS

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

void iterate() {
    // update averages

    node.d_av[0] = (node.d[0]+node.dim_neighbour[0])/2;
    node.d_av[1] = (node.d[1]+node.dim_neighbour[1])/2;

    Serial.println("Ny runde");
    
    Serial.print("node.node.d[0] =");
    Serial.println(node.d[0]);
    Serial.print("node.dim_neighbour[0] =");
    Serial.println(node.dim_neighbour[0]);
    
    Serial.print("node.d_av[0] =");
    Serial.println(node.d_av[0]);
    Serial.println(" ");
    
    // Update local lagrangians
    node.y[0] = node.y[0] + rho*(node.d[0]-node.d_av[0]);
    node.y[1] = node.y[1] + rho*(node.d[1]-node.d_av[1]);

    Res res;

    res = primal_solve(node, rho);
    node.d[0] = res.d_best0;
    node.d[1] = res.d_best1;
    
    // delay(300);
    send_i2c_message(node.d[0], node.d[1]);
    
}

void consens(){

    while (is_other_node_ready == false){ // wait until node1 is ready
        if (node.index ==  1) {
            send_is_ready_i2c_message(); // send ready message
        }
        delay(1000);
        Serial.print(is_other_node_ready);
        Serial.println();
    }
    Serial.print("CONS");
    while(true) {
        if (_received_new_data == true){
            
            _received_new_data = false;
            iterate();
        }
        // Serial.print("kjører");
        // delay(100);
    }

}

