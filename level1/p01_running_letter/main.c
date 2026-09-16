#include <stdio.h>
#include <windows.h>

#define SLEEP_INTERVAL 50 // 用宏定义，增加可读性
#define WIDTH 80

int main()
{
    printf("hello world!\n");
    unsigned int position = 0, direction = 1;
    unsigned int i;
    while(1)
    {
        // system("cls");
        for(i = 0; i < position; i ++)
        {
            putchar(' ');
        }
        putchar('A');
        for(i = 0; i < WIDTH - 1 - position; i ++)
        {
            putchar(' ');
        }
        position += direction;
        if(position >= WIDTH - 1)
        {
            direction = -1;
        }
        else if(position <= 0)
        {
            direction = 1;
        }
        Sleep(SLEEP_INTERVAL); // magic number: 幻数
        putchar('\r');
    }
    return 0;
}