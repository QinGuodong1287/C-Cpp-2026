#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

int main()
{
    unsigned int memory[999] = {0};
    unsigned int memory_end = 0;
    unsigned int max_i;
    bool is_prime;
    clock_t start = clock();
    for(unsigned int num = 2; num <= 1000; num ++)
    {
        is_prime = true;
        max_i = (unsigned int)sqrt(num);
        for(unsigned int i = 0; i < memory_end; i ++)
        {
            if(memory[i] > max_i)
            {
                break;
            }
            if(num % memory[i] == 0)
            {
                is_prime = false;
                break;
            }
        }
        if(!is_prime)
        {
            continue;
        }
        printf("%d\n", num);
        memory[memory_end ++] = num;
    }
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Elapsed time: %.3lf seconds\n", elapsed);
    return 0;
}