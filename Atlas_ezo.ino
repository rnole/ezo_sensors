

#include <stdint.h>
#include <Wire.h>
#include "Atlas_ezo.h"

Atlas_ezo::Atlas_ezo(uint8_t addr){  
  i2c_addr = addr;
}

void Atlas_ezo::begin(void){
  return;
}

void Atlas_ezo::send_cmd(char cmd, float *value){

  uint8_t code      =  0;
  uint8_t index     =  0;
  char response[20] = {0};
  
  Wire.beginTransmission(i2c_addr);
  Wire.write(cmd);
  Wire.endTransmission();
  delay(900);

  Wire.requestFrom(i2c_addr, 20, 1);
  code = Wire.read();
  
  if(code == 1){
      while (Wire.available()){        
          response[index] = Wire.read();
                                   
          if (response[index] == 0){                     
              Wire.endTransmission();        
              break;                         
          }
          index++;
      }
      
      *value = atof(response);
  }
}

