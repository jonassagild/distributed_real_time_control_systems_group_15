//
//  i2c_master_and_slave.cpp
//  
//
//  Created by Jonas Åsnes  Sagild on 23.11.2018.
//

#include "i2c_master_and_slave.hpp"

void I2C::setup(){
    Wire.begin(_own_add);
    Wire.onReceive(receive_event);
    
    // enable broadcasts to be received
    TWAR = (_own_add << 1) | 1;
}


void I2C::send_message(char c, int other_add){
    Wire.beginTransmission(other_add); // gets the bus
    Wire.write(c); // send byte to address on bus
    Wire.endTransmission(); // releases the bus
}


void I2C::send_message(float message, int other_add){
    dtostrf(message, 3, 2, _chars);
    Wire.beginTransmission(other_add);
    Wire.write(_chars);
    Wire.write(',');
    Wire.endTransmission();
}



void receive_event(int howMany){
    while (Wire.available()> 0) { // check data on BUS
        char c = Wire.read(); //receive byte at I2S BUS
        Serial.println(c);
    }
}
