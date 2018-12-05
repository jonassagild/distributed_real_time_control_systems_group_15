//
//  pingpongtest.hpp
//  
//
//  Created by Jonas Åsnes  Sagild on 05.12.2018.
//

#ifndef pingpongtest_hpp
#define pingpongtest_hpp

#include "Arduino.h"
#include <Wire.h>

void initialize_system();

void send_i2c_message();

void receive_i2c_message(int how_many);

#endif /* pingpongtest_hpp */
