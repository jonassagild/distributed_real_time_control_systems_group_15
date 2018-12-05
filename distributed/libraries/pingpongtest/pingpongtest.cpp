//
//  pingpongtest.cpp
//  
//
//  Created by Jonas Åsnes  Sagild on 05.12.2018.
//

#include "pingpongtest.hpp"

volatile bool received_message = false;

void initialize_system(){
    Wire.begin(25);
    Wire.onReceive(receive_i2c_message); //event handler
    TWAR = (25 << 1) | 1; // enable broadcasts to be
}

void send_i2c_message(){
    Wire.beginTransmission(24);
    Wire.write('X');
    Wire.endTransmission();
}

void receive_i2c_message(int how_many){
    char c;
    while (Wire.available() > 0) {
        c = Wire.read();
    }
    Serial.print(c);
    received_message = true;
}

void wait_loop() {
    while(received_message == false){
        // just wait
    }
    Serial.print(received_message);
    send_i2c_message();
    received_message = false;
    wait_loop();
}

void waiter() {
    while(true) {
        // just wait
    }
}

