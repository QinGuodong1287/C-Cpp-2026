#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TYPE_LENGTH 50 // characters count

struct ItemInfo
{
    char *type;
    unsigned int amount;
    struct ItemInfo *next;
};

// items is the head (ahead of the items contents).
typedef bool (*operate_func)(struct ItemInfo *items);

void receive_string_input(char *buffer, size_t size);
bool display_items(struct ItemInfo *items);
bool add_item(struct ItemInfo *items);
bool remove_item(struct ItemInfo *items);
void fix_items_amount(struct ItemInfo *items);
void free_items(struct ItemInfo *items);
bool load_items_from_file(struct ItemInfo *items);
bool save_items_to_file(struct ItemInfo *items);

const char *filename = "items.txt";

int main()
{
    operate_func funcs[3] = {display_items, add_item, remove_item};
    struct ItemInfo items = {"warehouse", 0, NULL};
    int operation;
    printf("hello world!\n");
    if(!load_items_from_file(&items))
    {
        fprintf(stderr, "Maybe the file does not exists?\n");
    }
    printf("\nWelcome to the warehouse management system.\n");
    while(true)
    {
        printf("1. Display items\n");
        printf("2. Add a item\n");
        printf("3. Remove a item\n");
        printf("4. Exit the program\n");
        printf("Please enter the operation number before its corresponding operation: ");
        scanf("%d", &operation);
        putchar('\n');
        if(operation == 4)
        {
            break;
        }
        else if(operation < 1 || operation > 3)
        {
            fprintf(stderr, "Your operation number is invalid. The number must be 1, 2 or 3.\n");
            continue;
        }
        if(funcs[operation - 1](&items))
        {
            printf("The operation is completed successfully.\n");
        }
        else
        {
            fprintf(stderr, "The operation is exited unexpectedly.\n");
        }
        putchar('\n');
    }
    if(save_items_to_file(&items))
    {
        printf("Saved items to file successfully.\n");
    }
    free_items(&items);
    printf("Thank you for using the warehouse management system.\nGoodbye.\n");
    return 0;
}

void receive_string_input(char *buffer, size_t size)
{
    if(size == 0)
    {
        return;
    }
    char c;
    do
    {
        c = getchar();
    } while(c != EOF && c != '\n');
    if(c == EOF)
    {
        return;
    }
    for(size_t i = 0; i < size; i ++)
    {
        c = getchar();
        if(c == EOF || c == '\n')
        {
            break;
        }
        buffer[i] = c;
    }
    if(c != EOF)
    {
        while(c != EOF && c != '\n')
        {
            c = getchar();
        }
    }
}

bool display_items(struct ItemInfo *items)
{
    if(items == NULL)
    {
        fprintf(stderr, "The items list is NULL.\n");
        return false;
    }
    if(items->next == NULL || items->amount == 0)
    {
        printf("There is no item in the warehouse.\n");
        return true;
    }
    unsigned int types_count = 0;
    printf("There %s %u item%s in the warehouse:\n", items->amount != 1? "are": "is", items->amount, items->amount != 1 ? "s" : "\0");
    for(struct ItemInfo *current = items->next; current != NULL; current = current->next)
    {
        types_count ++;
        printf("%u. Type: %s, Amount: %u.\n", types_count, current->type != NULL? current->type: "<NULL string>", current->amount);
    }
    return true;
}

bool add_item(struct ItemInfo *items)
{
    if(items == NULL)
    {
        fprintf(stderr, "The items list is NULL.\n");
        return false;
    }
    char type[MAX_TYPE_LENGTH + 1] = {'\0'};
    printf("Please enter the type of the new item (max length = 50): ");
    receive_string_input(type, MAX_TYPE_LENGTH);
    struct ItemInfo *current, *prev = items;
    for(current = items->next; current != NULL; current = current->next)
    {
        if(strcmp(current->type, type) == 0)
        {
            break;
        }
        prev = current;
    }
    if(current == NULL)
    {
        current = prev;
        current->next = (struct ItemInfo *)malloc(sizeof(struct ItemInfo));
        current = current->next;
        if(current == NULL)
        {
            fprintf(stderr, "Sorry, we cannot allocate memory to store new item information.\n");
            return false;
        }
        size_t type_len = strlen(type);
        current->type = (char *)malloc((type_len + 1) * sizeof(char));
        if(current->type == NULL)
        {
            fprintf(stderr, "Sorry, we cannot allocate memory to store new item information.\n");
            return true;
        }
        strcpy(current->type, type);
        current->type[type_len] = '\0';
        current->amount = 1;
        current->next = NULL;
    }
    else
    {
        current->amount ++;
    }
    items->amount ++;
    printf("Added an item in the warehouse: %s.\n", type);
    printf("Type: %s, Amount: %u, Total: %u\n", type, current->amount, items->amount);
    return true;
}

bool remove_item(struct ItemInfo *items)
{
    if(items == NULL)
    {
        fprintf(stderr, "The items list is NULL.\n");
        return false;
    }
    if(items->next == NULL)
    {
        printf("There is nothing to remove.\n");
        return true;
    }
    fix_items_amount(items);
    if(items->amount == 0)
    {
        printf("There is nothing to remove.\n");
        return true;
    }
    char type[MAX_TYPE_LENGTH + 1] = {'\0'};
    printf("Please enter the type of the item to remove: ");
    receive_string_input(type, MAX_TYPE_LENGTH);
    struct ItemInfo *current, *prev = items;
    for(current = items->next; current != NULL; current = current->next)
    {
        if(current->type != NULL && strcmp(current->type, type) == 0)
        {
            break;
        }
        prev = current;
    }
    if(current == NULL)
    {
        printf("There is nothing to remove.\n");
        return true;
    }
    unsigned int current_amount;
    if(current->amount > 1)
    {
        current->amount --;
        current_amount = current->amount;
    }
    else
    {
        prev->next = current->next;
        free(current->type);
        free(current);
        current = NULL;
        current_amount = 0;
    }
    items->amount --;
    printf("Removed an item: %s.\n", type);
    printf("Type: %s, Amount: %u, Total: %u\n", type, current_amount, items->amount);
    return true;
}

void fix_items_amount(struct ItemInfo *items)
{
    if(items == NULL)
    {
        return;
    }
    unsigned int amount = 0;
    for(struct ItemInfo *current = items->next; current != NULL; current = current->next)
    {
        amount += current->amount;
    }
    items->amount = amount;
}

void free_items(struct ItemInfo *items)
{
    for(struct ItemInfo *current = items->next; current != NULL;)
    {
        items->next = current->next;
        free(current->type);
        free(current);
        current = items->next;
    }
}

bool load_items_from_file(struct ItemInfo *items)
{
    FILE *fp;
    if((fp = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Cannot load items from file.\n");
        return false;
    }
    free_items(items);
    char c;
    char type[MAX_TYPE_LENGTH + 1] = {'\0'};
    size_t type_len;
    unsigned int amount;
    struct ItemInfo *current = items;
    int read_status = 0;
    unsigned int unnamed_count = 0;
    bool result = true;
    while(!feof(fp))
    {
        type[0] = '\0';
        read_status = fscanf(fp, "%u", &amount);
        if(read_status == EOF)
        {
            break;
        }
        if(read_status < 1)
        {
            amount = 1;
        }
        do
        {
            c = fgetc(fp);
        } while(c != EOF && c != '\n' && (c == ' ' || c == '\t'));
        if(c == EOF || c == '\n')
        {
            sprintf(type, "unnamed_%u", ++ unnamed_count);
        }
        else
        {
            type[0] = c;
            fgets(type + 1, MAX_TYPE_LENGTH - 1, fp);
            type_len = strlen(type);
            if(type[type_len - 1] == '\n')
            {
                type[type_len - 1] = '\0';
            }
        }
        current->next = (struct ItemInfo *)malloc(sizeof(struct ItemInfo));
        current = current->next;
        if(current == NULL)
        {
            fprintf(stderr, "Sorry, we cannot allocate memory to store new item information.\n");
            result = false;
            break;
        }
        type_len = strlen(type);
        current->type = (char *)malloc((type_len + 1) * sizeof(char));
        if(current->type != NULL)
        {
            strcpy(current->type, type);
            current->type[type_len] = '\0';
        }
        else
        {
            fprintf(stderr, "Sorry, we cannot allocate memory to store new item information.\n");
            result = false;
        }
        current->amount = amount;
        current->next = NULL;
    }
    fclose(fp);
    fix_items_amount(items);
    return result;
}

bool save_items_to_file(struct ItemInfo *items)
{
    FILE *fp;
    if((fp = fopen(filename, "w")) == NULL)
    {
        fprintf(stderr, "Cannot save items to file.\n");
        return false;
    }
    for(struct ItemInfo *current = items->next; current != NULL; current = current->next)
    {
        fprintf(fp, "%u %s\n", current->amount, current->type != NULL? current->type: "<NULL string>");
    }
    fclose(fp);
    return true;
}
