//
//  Utilities.hpp
//  
//
//  Created by Jonas Åsnes  Sagild on 23.10.2018.
//

#ifndef Utilities_hpp
#define Utilities_hpp

#include "Arduino.h"

/*
 set_timer_frequency
 input
 - int pin: pin to change frequency of
 - int divisor: divisor to divide max frequency of pin to.
 
 E.g. set_timer_frequency(6,1) sets timer 6 to max (16Mhz).
 */
void set_timer_frequency(int pin, int divisor);

#endif /* Utilities_hpp */
