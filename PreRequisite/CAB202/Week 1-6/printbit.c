#include <stdio.h>

void show_byte(unsigned char val){
      printf("0b");

    for (int i = 7; i >= 0; i--){
        printf("%d", (val & (1 <<i)) >> i);
    }
}

int main(){
    unsigned char seventy_five = 75;
    unsigned char twelve = 12;
    unsigned char minus_twelve = -12;
    
    printf(" 75 base 10 is "); show_byte(seventy_five); printf(" base 2\n");
    printf(" 12 base 10 is "); show_byte(twelve); printf( " base 2\n");
    printf("-12 base 10 is "); show_byte(minus_twelve); printf(" base 2\n");
    return 0;
}