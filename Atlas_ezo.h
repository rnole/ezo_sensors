#ifndef ATLAS_EZO_H
#define ATLAS_EZO_H

class Atlas_ezo{
  
 private:  
  
 public:      
   uint8_t  i2c_addr;
  
   Atlas_ezo(uint8_t addr);
   void begin(void);
   void send_cmd(char cmd, float *value);
};



#endif  /* ATLAS_EZO_H */
