#include <stdio.h>
#include <stdlib.h>

unsigned char REGISTER;

unsigned char parse_byte( const char * str ) {
	unsigned char result = 0;
	int start_pos = 0;
	
	if ( str[0] == '0' && str[1] == 'b' ) {
		start_pos = 2;
	}

	for ( int i = start_pos; str[i] == '0' || str[i] == '1'; i++ ){
		result = result * 2 + str[i] - '0';
	}
	
	return result;
}

void show_register(const char * label){
	printf("%s operation, REGISTER = 0b", label);
	
	for (int i = 7; i >= 0; i--) {
		printf("%d", (REGISTER & (1 << i)) >> i);
	}
	
	printf("\n");
}

void clear_bit_demo(unsigned char pin_number) {
	show_register("before");
	REGISTER = REGISTER & ~(1 << pin_number);
	show_register(" after");
}

int main (int argc, char * argv[] ) {
	if ( argc < 3) {
		printf("Error: expected 2 command line arguments.\n"
			"Usage: get_bit_demo initial_register_value pin_number\n\n"
			"initial_register_value: 0..255\n"
			"pin_number: 0..7\n"
		);
		return 1;
	}
	
	REGISTER = parse_byte(argv[1]);
	
	unsigned char pin_number = atoi(argv[2]);

	clear_bit_demo( pin_number );
	
	return 0;
}
