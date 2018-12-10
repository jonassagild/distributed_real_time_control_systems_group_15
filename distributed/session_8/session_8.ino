#include "consensus.hpp"

void setup() {
  Serial.begin(115200);
  // void initialize_system(double _k_11, double _k_12, double _k_21, double _k_22, _l, _o, _c, _rho, _i2c_master_address);

  initialize_system(2, 1, 1, 2, 80, 0, 1, 0.07, 25, 1);
  
}

void loop() {

  consens();
  exit(0);
  
}
