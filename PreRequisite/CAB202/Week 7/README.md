## Introduction to Microcontrollers

#### Getting started
- Adding preprocessor -> header files -> <avr/io.h>. Similar to standard in/out and standard library 
- Defining macros with the library to make things easier. Ex: macro -> 

#### Breadboards
- Remeber how are connected
- Add the ground to the negative (black line)

#### Digital Input and Output
- DDRB -> Data direction register (ddr) and then specify register -> Then specify whether pin is used for input or output
- PORTX -> where x is the specified register, PORT is used for output
- PINX -> where x is the specified register, PIN is used for input

#### Using macros 
<img width="860" alt="Screen Shot 2022-02-02 at 1 15 18 pm" src="https://user-images.githubusercontent.com/71302996/152087743-4abd02a7-62cc-41ec-a9e0-cc8e99880843.png">

- Code that can be used -> macros  
/* 
 *  Setting data directions in a data direction register (DDR)
 */

/*
 *  Setting, clearing, and reading bits in registers.
 *  reg is the name of a register; pin is the index (0..7)
 *  of the bit to set, clear or read.
 *  (WRITE_BIT is a combination of CLEAR_BIT & SET_BIT)
 */

#define SET_BIT(reg, pin)			(reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)			(reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)	(reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)			(((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)		(BIT_VALUE((reg),(pin))==1)



#### Digital I/O: Turn on/off LEDs using switches
- Look at the specifications/schematics and do the task.
- Uno borad has two GNDs see which one goes where. 
- VCC on the schematics is the 5V on uno board


#### Digital Input: Read switches


#### Additional notes
