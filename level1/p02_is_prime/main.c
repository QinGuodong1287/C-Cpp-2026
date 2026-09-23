#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <stdbool.h>

int main()
{
    unsigned int num;
    scanf("%u", &num);
    unsigned int max_i = (unsigned int)sqrt(num);
    bool is_prime = true;
    for(unsigned int i = 2; i < max_i; i ++)
    {
        if(num % i == 0)
        {
            is_prime = false;
            break;
        }
    }
    if(is_prime)
    {
        printf("%u is a prime number.\n", num);
    }
    else
    {
        printf("%u is not a prime number.\n", num);
    }
    return 0;
}