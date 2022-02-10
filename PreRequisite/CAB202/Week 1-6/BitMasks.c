#include <stdio.h>

void show_byte(unsigned char val){
	printf("0b");
	
	for (int i = 7; i >= 0; i--) {
		printf("%d", (val & (1 << i)) >> i);
	}
}

// Function-like macros.
#define show_mask(label,expr) {printf( #label ": Mask " #expr " is equal to "); show_byte(expr); printf("\n");}
#define show_bits(label, expr) {printf( #label ": The value of " #expr " is "); show_byte(expr); printf("\n");}
#define show_bit(label, expr) {printf( #label ": Bit value " #expr " is %d\n", expr);}

int main () {
	// Definition: a "bit mask" is a numeric value in which one or more bits have been
	// set so they can be used to insert or extract specific bits from some other
	// expression.
	
	// Ex 1: Create a mask which can be used to access or change bit 5.
	show_mask(Ex 1, (1<<5));
	
	// Ex 2: Create a mask which can be used to access or change bit 3.
	
	// Ex 3: Create a mask which can be used to access or change bits 1, 3, 5, and 6.

	// Definition: a bit is set when its value is 1. Setting a bit means making 
	//	its value equal to 1.
	
	// Ex 4: Use a bit mask in combination with the bitwise OR operator to set bits
	//		2, 4 and 6 of a byte with initial value 0b00110011.
	show_bits(Ex 4, 0b00110011 | ((1<<2)|(1<<4)|(1<<6)) );
	
	// Ex 5: Use a bit mask in combination with the bitwise OR operator to set bits
	//		5 of a byte with initial value 0b10010011.

	// Definition: a bit is clear when its value is 0. Clearing a bit means 
	//	making its value equal to 0.
	
	// Ex 6: Use a bit mask in combination with the bitwise COMPLEMENT and AND 
	//	operators to clear bits 2, 4 and 6 of a byte with initial value 0b00110011.
	
	// Ex 7: Use a bit mask in combination with the bitwise COMPLEMENT and AND 
	//	operators to clear bits 5 of a byte with initial value 0b10010011.

	// Definition: toggle means to set a bit if it is to clear, or to clear a 
	//	bit if it is set.
	
	// Ex 8: Use a bit mask in combination with the bitwise XOR operator to toggle bits
	//		2, 4 and 6 of a byte with initial value 0b00110011.
	
	// Ex 9: Use a bit mask in combination with the bitwise XOR operator to toggle bits
	//		5 of a byte with initial value 0b10010011.
	
	// Ex 10: Use a bit mask to get the value of bit 5 of a byte with initial value
	//      0b00100101
	
	// Ex 11: Use a bit mask to get the value of bit 0 of a byte with initial value
	//      0b00100101
	
	// Ex 12: Use a bit mask to get the value of bit 1 of a byte with initial value
	//      0b00100101
	show_bit(Ex 12, (0b00100101 & (1<<1)) >> 1 );
	
	return 0;
}