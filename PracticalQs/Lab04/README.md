## Pracical 04

### Task A
Question: Which I2C of the TM4C1294NCPDT is being utilised by the sensor?  

Answer: The I2C0 which is connected to Port B - Pin 2 & 3.  


Question: Which GPIO port and pins of the TM4C1294NCPDT does the I2C use?  

Answer: GPIO Port B - Pins 2 & 3.  


Question: What bits are being written to the Range Number Field in the configuration register
when SensorOpt3001Enable(true) is called?  

Answer: The config enable in hex is 10C4 -> Take equivalent C410, this given in binary is -> 1100 0100 0001 0000. Where the RNF is the bits in the left 
most corner - Bits 12 to 15. Page 21.  


Question: What is the value of the Conversion Time field when SensorOpt3001Enable(true) is
called.  

Answer: Conversion Bit Field is the bit 11. The bit is set to zero which is 100ms. Page 22.    


Question: When writing a 16bit value to the OPT3001 sensor, what byte is sent first (most
significant or least significant)?  

Answer: Note that register bytes are sent most significant byte first, followed by the least significant byte.



### Task B
- 

### Task C
- 


