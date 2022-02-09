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
- 

#### Bitwise operations (Practical)
- Notes

#### Bit masks (Practical)
- Notes


##### bash commands
function compile () { gcc $1.c -o $1 -Wall -Werror -std=gnu99; }
compile X where is the the name of you script/file
