## First C programs; Variables, expressions, and data types; Standard input/output.

#### Heading.
- Notes

## Numeric types and making decisions.
#### Heading.
- Notes


## Loops; Redirecting standard input and output.
#### Heading.
- Notes


## Arrays, introducing strings.
#### Heading.
- Notes


## Scope and visibility; Functions.
#### Heading.
- Notes


## Binary Representation of data and bitwise operations.
#### Decimal Represenation (Theory)
- Digits 0 to 9 to make numbers <- base 10  
- Concept of Addtion, then Add with Carry. -> gives the idea of overflow when the carry is not represented  
- When subtracting we use something called 10s compliment: a vs ā. 
9s compliment -> 9 - digit -> add 1 to the resulting digit -> i.e 1234 -> 9999 - 1234 -> 8765 -> 8765 + 1 -> 8766
This is so we can use very simple hardware to perform both addition and substraction.
- Concept of mutpliying by power of 10: when multiplied by p10 the number is pushed to the left by a digit. 
- This is called left shift: Multiplied by the base of number system      
- Division of by the basw of the number system will result in a right shit. 

#### Binary Represenation (Theory)
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
Bunary number : 2s compliment : 1s compliment + 1

for 75 decimal = 64 + 11 
               = 64 + 8 + 2 + 1
               = 0 1 0 0 1 0 1 1
 
 for -12       
 start with 12 = 0000 1100
 1s compliment = 1111 0011
plus 1         = 1111 0100 
               = -12

#### Bitwise operations (Practical)
- Notes

#### Bit masks (Practical)
- Notes


##### bash commands
function compile () { gcc $1.c -o $1 -Wall -Werror -std=gnu99; }
compile X where is the the name of you script/file
