#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//Function declarations
uint8_t swap_bit(uint8_t data, int position_1, int position_2);
int revbits(uint8_t a);

int main()
{
    uint8_t var = 0x84; //Declaring a variable for bit operations
    //uint8_t var2 = 0xA0;

    printf("Given Number: %d", var);

    var |= (1 << 3); //set bit 3
    printf("\nSet bit 3: %d", var);

    var |= (1 << 4) | (1 << 6); // set bit 4 and 6
    printf("\nSet bit 4 and 6: %d", var);

    var &= ~(1 << 2); //clear bit 2
    printf("\nClear bit 2: %d", var);

    var &= ~(1 << 2) & ~(1 << 7); //clear bit 2 and 7
    printf("\nClear bit 2 and 7: %d", var);

    var ^= (1 << 3); //toggle bit 3
    printf("\nToggle bit 3: %d", var);

    var = (var | (1 << 2)) & ~(1 << 5) & ~(1 << 7); //set bit 2 and clear bit 5 and 7
    //var2 = (var2 &~(1<<5) &~(1<<7)) | (1<<2); //set bit 2 and clear bit 5 and 7
    printf("\nSet bit 2 and clear bit 5 and 7: %d", var);

    printf("\nSwaping bit 3 and 5 of var: %d", swap_bit(var, 3, 5));
    printf("\nSwaping bit 0 and 2 of var: %d", swap_bit(var, 0, 2));

    printf("\nVar after reverse: %d", revbits(var)); //calling the revbits function
    return 0;
}

//this function will swap any two bits of a 8bit data according to user input
uint8_t swap_bit(uint8_t data, int position_1, int position_2)
{
    uint8_t bit_1 = (data >> position_1) & 1; //masking the 1st bit
    uint8_t bit_2 = (data >> position_2) & 1; //masking the 2nd bit

    uint8_t xor_of_bit = bit_1 ^ bit_2;

    int result = data ^ (xor_of_bit << position_1 | xor_of_bit << position_2);

    return result;
}

//this function reverses the bit position of a 8 bit data
int revbits(uint8_t a)
{
    uint8_t nbit = sizeof(a) * 8; //number of bits
    uint8_t rev_num = 0;
    int i;

    for (i = 0; i < nbit; i++)
    {
        if ((a & (1 << i)))                   //ith bit position is set in input
            rev_num |= 1 << ((nbit - 1) - i); //if ith bit position is set then set the bit at ((nbit - 1) - i) position at output
    }
    return rev_num;
}