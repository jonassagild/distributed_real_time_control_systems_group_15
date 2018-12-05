#include "pingpongtest.hpp"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initialize_system();

}

void loop() {
  // put your main code here, to run repeatedly:
  send_i2c_message();
  delay(100);
}
