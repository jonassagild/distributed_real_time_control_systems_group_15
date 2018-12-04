//
//  i2c_master_and_slave.hpp
//  
//
//  Created by Jonas Åsnes  Sagild on 23.11.2018.
//

#ifndef i2c_master_and_slave_hpp
#define i2c_master_and_slave_hpp

#include "Arduino.h"
#include <Wire.h>

void receiveEvent (int howMany);

class I2C {
public:
    /*
     set_i2c
     enables i2c transfer
     input
     - int master_address: own address, needs to be unique
     - int slave_address: address to send data to
     */
    void setup();
    
    
    /*
     set_i2c
     enables i2c transfer
     input
     - int master_address: own address, needs to be unique
     - int slave_address: address to send data to
     */
    void send_message(char c, int other_add);
    
    
    /*
     set_i2c
     enables i2c transfer
     input
     - int master_address: own address, needs to be unique
     - int slave_address: address to send data to
     */
    void send_message(float message, int other_add);
    
    

private:
    char _chars[10]; // used to transfer float
    int _own_add = 22;
    
};

/*
 set_i2c
 enables i2c transfer
 input
 - int master_address: own address, needs to be unique
 - int slave_address: address to send data to
 */
void receive_event(int howMany);


#endif /* i2c_master_and_slave_hpp */
