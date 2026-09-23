#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

void hanoi(char a, char b, char c, unsigned int count);

int main()
{
    hanoi('A', 'B', 'C', 3);
    // hanoi('A', 'B', 'C', 64);
    return 0;
}

void hanoi(char a, char b, char c, unsigned int count)
{
    if(count == 0)
    {
        return;
    }
    else if(count == 1)
    {
        printf("%c -> %c\n", a, c);
        return;
    }
    hanoi(a, c, b, count - 1);
    printf("%c -> %c: %u\n", a, c, count);
    hanoi(b, a, c, count - 1);
}
