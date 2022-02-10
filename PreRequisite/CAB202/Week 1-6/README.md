## First C programs; Variables, expressions, and data types; Standard input/output.
### Heading.
- Notes

## Numeric types and making decisions.
### Heading.
- Notes


## Loops; Redirecting standard input and output.
### Heading.
- Notes


## Arrays, introducing strings.
### Heading.
- Notes


## Scope and visibility; Functions.
### Heading.
- Notes


## Binary Representation of data and bitwise operations.
### Decimal Represenation (Theory)
- Digits 0 to 9 to make numbers <- base 10  
- Concept of Addtion, then Add with Carry. -> gives the idea of overflow when the carry is not represented  
- When subtracting we use something called 10s compliment: a vs ā. 
9s compliment -> 9 - digit -> add 1 to the resulting digit -> i.e 1234 -> 9999 - 1234 -> 8765 -> 8765 + 1 -> 8766
This is so we can use very simple hardware to perform both addition and substraction.
- Concept of mutpliying by power of 10: when multiplied by p10 the number is pushed to the left by a digit. 
- This is called left shift: Multiplied by the base of number system      
- Division of by the basw of the number system will result in a right shit. 

### Binary Represenation (Theory)
- Same thing that we looked at in the previous part in the decimal system we will look at it in the binary system.
- Starting from fixed decimal 4 digit decimal -> 8 bit binary fixed binary

In decimal, 0 to 9999  
In decimal, numbers are 0, 1,....9  

In binary,  0 to 1111 1111 (255)  
In binary,  numbers are 0,1  

Converting between binary and decimal number system.  

Binary digIT = BIT  
0 = 0000 0000  
1 = 0000 0001  
2 = 0000 0010  
3 = 0000 0011  
4 = 0000 0100  

When the binary numbers go 1 -> 10 -> 100 the decimal goes from 1,2,4  

- Represent negative nmber  
Decimal number: 10s compliment: 9s compliment + 1  
Binary number : 2s compliment : 1s compliment + 1  

for 75 decimal = 64 + 11  
               = 64 + 8 + 2 + 1  
               = 0 1 0 0 1 0 1 1  
 
 for -12       
 start with 12 = 0000 1100  
 1s compliment = 1111 0011  
plus 1         = 1111 0100  
               = -12  
To. do 75 - 12: get 75, the -12 then add, binary represenation you get when converted becomes 63  

- Multiplication by powers of 2
Same as powers of 10, except in binary rep.  
Example:  
7 x 2^0 = 0111 x 1 = 0111    
7 x 2^1 = 0111 x 2 = 1110  
7 x 2^2 = 0111 x 2 x 2 = 11100  
We fimd the concept of left shfit. Move and put a zero there  

### Bitwise operations (Practical)
- gcc ,Bit Shift, AND, OR, XOR, Compliment
- The printbit.c code is a good example of bitwise operations. 
- The binary form of the number will appear from the program.
- A generic property of the negative number in decimal + the base adds up to 255 ?
- Left shift operator is the number getting multiplied by 2 in binary. This is denoted by  
left shift 0: <<0,  
left shift 1: <<1,  
In general: x << y == x * 2^y (2 to the power of y).

- Right shift operator is the number getting divided by 2 in binary. This is denoted by ((7 << 3) /2) also given by (7 <<3) >> 1.
In general: x >> y == x / 2^y (2 to the power of y).

- Bitwise AND (&) vs logical AND (&&). 
Bitwise AND (&) is the bitwise operation that returns 1 if both operands are 1, otherwise it returns 0.  
Whereas logical AND (&&) is the logical operation, anything non zero is considered to be "true". you get the result of "0b 0000 0001"

- Bitwise OR (|) vs logical OR (||).
Bitwise OR (|) is the bitwise operation that returns 1 if either operand is 1, otherwise it returns 0.  
Logical OR, give a result of "0b 0000 0001"  

- Example: to show the clear difference between logical and bitwise

- XOR ( ^ ).  
0 ^ 0 -> 0  
0 ^ 1 -> 1  
1 ^ 0 -> 1  
1 ^ 1 -> 0  
Only bitwise exists.  

- Understand the different "gates" so they can be manipulated in circuit boards.  

<img width="536" alt="Screen Shot 2022-02-10 at 10 58 56 pm" src="https://user-images.githubusercontent.com/71302996/153413174-b964f779-848c-473c-9fa4-e365d36a840e.png">

- Understand the compliment operator (~).  
Gives the opposite of the bit.

- Combining compliment with XOR finds that the bits remain the same.

### Bit masks (Practical)
- Set, Clear, Toggle, Test individual bits within a byte.


#### bash commands
function compile () { gcc $1.c -o $1 -Wall -Werror -std=gnu99; }  
compile X where is the the name of you script/file
