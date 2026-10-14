#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int num;
    struct Node *next;
};

int main()
{
    struct Node head = {0, NULL}, *current = &head, *tail = NULL;
    unsigned int n;
    int num;
    printf("hello world!\n");
    // Step 1: Build a chain.
    scanf("%u", &n);
    if(n == 0)
    {
        return 0;
    }
    for(unsigned int i = 0; i < n; i ++)
    {
        scanf("%d", &num);
        current->next = (struct Node *)malloc(sizeof(struct Node));
        current = current->next;
        current->num = num;
        current->next = NULL;
    }
    tail = current;
    for(current = head.next; current != NULL; current = current->next)
    {
        printf("%d ", current->num);
    }
    putchar('\n');
    // Step 2: Reverse the chain.
    current = head.next;
    struct Node *prev = NULL, *next = NULL;
    if(tail != &head)
    {
        head.next = tail;
        tail = NULL;
        while(current != NULL)
        {
            next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }
        prev = NULL;
        next = NULL;
        for(current = head.next; current != NULL; current = current->next)
        {
            printf("%d ", current->num);
        }
    }
    putchar('\n');
    // Step 3: Find the first node, which store the number 5.
    current = head.next;
    unsigned int i;
    for(i = 0; i < n; i ++)
    {
        if(current == NULL)
        {
            break;
        }
        if(current->num == 5)
        {
            printf("%u\n", i);
            break;
        }
        current = current->next;
    }
    if(current != NULL)
    {
        for(current = current->next; current != NULL; current = current->next)
        {
            i ++;
            if(current->num == 5)
            {
                printf("%u\n", i);
                break;
            }
        }
        if(current == NULL)
        {
            printf("-1\n");
        }
    }
    else
    {
        printf("-1\n-1\n");
    }
    for(current = head.next; current != NULL;)
    {
        next = current->next;
        free(current);
        current = next;
    }
    head.next = NULL;
    return 0;
}