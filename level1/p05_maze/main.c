#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <wchar.h>

#define WIDTH 18
#define HEIGHT 18

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define UP_WALL_BIT 0x1
#define DOWN_WALL_BIT 0x2
#define LEFT_WALL_BIT 0x4
#define RIGHT_WALL_BIT 0x8
#define ALL_WALL_BIT (UP_WALL_BIT | DOWN_WALL_BIT | LEFT_WALL_BIT | RIGHT_WALL_BIT)

enum BlockType {Blank, Road, Wall};
struct Block
{
    enum BlockType type;
    unsigned char wall_bits; // bit 0: up, bit 1: down, bit 2: left, bit 3: right
};

struct Block map[WIDTH][HEIGHT] = {{Blank, 0}};

struct PathNode
{
    unsigned int row;
    unsigned int col;
    struct PathNode *next;
};

void clear_screen();
bool init_map(unsigned int *start_row, unsigned int *start_col, unsigned int *target_row, unsigned int *target_col);
void print_map(unsigned int player_row, unsigned int player_col);
void player_move(unsigned int *player_row, unsigned int *player_col, bool *game_end);

int main()
{
    unsigned int start_row = 0, start_col = 0;
    unsigned int player_row = 0, player_col = 0;
    unsigned int target_row = 0, target_col = 0;
    bool game_end = false;
    printf("hello world!\n");
    srand((unsigned int)time(NULL));
    if(!init_map(&start_row, &start_col, &target_row, &target_col))
    {
        fprintf(stderr, "Initializing maze map failed.\n");
        return 1;
    }
    player_row = start_row;
    player_col = start_col;
    do
    {
        print_map(player_row, player_col);
        player_move(&player_row, &player_col, &game_end);
        if(player_row == target_row && player_col == target_col)
        {
            clear_screen();
            print_map(player_row, player_col);
            game_end = true;
            printf("\nYou're win!\n");
            break;
        }
        if(!game_end)
        {
            // system("cls");
            clear_screen();
        }
    } while(!game_end);
    return 0;
}

void clear_screen()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if(!GetConsoleMode(hStdOut, &mode))
    {
        return;
    }
    DWORD original_mode = mode;
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hStdOut, mode);
    const wchar_t *clear_seq = L"\x1b[2J\x1b[H\x1b[3J";
    DWORD written;
    WriteConsoleW(hStdOut, clear_seq, (DWORD)wcslen(clear_seq), &written, NULL);
    SetConsoleMode(hStdOut, original_mode);
}

#define GENERATE_POLE \
    pole_position = rand() % (2 * (WIDTH + HEIGHT - 4)); \
    if(pole_position < WIDTH - 2) \
    { \
        row = 0; \
        col = pole_position + 1; \
        direction = DOWN; \
    } \
    else if(pole_position < WIDTH - 2 + HEIGHT - 2) \
    { \
        row = pole_position - (WIDTH - 2) + 1; \
        col = WIDTH - 1; \
        direction = LEFT; \
    } \
    else if(pole_position < 2 * (WIDTH - 2) + HEIGHT - 2) \
    { \
        row = HEIGHT - 1; \
        col = (WIDTH - 2) - (pole_position - (WIDTH - 2 + HEIGHT - 2)); \
        direction = UP; \
    } \
    else if(pole_position < 2 * (WIDTH - 2) + 2 * (HEIGHT - 2)) \
    { \
        row = (HEIGHT - 2) - (pole_position - (2 * (WIDTH - 2) + HEIGHT - 2)); \
        col = 0; \
        direction = RIGHT; \
    } \
    else \
    { \
        return false; \
    }

#define PLACE_POLE \
    map[row][col].type = Road; \
    if(direction == UP || direction == DOWN) \
    { \
        map[row][col].wall_bits = LEFT_WALL_BIT | RIGHT_WALL_BIT; \
    } \
    else if(direction == LEFT || direction == RIGHT) \
    { \
        map[row][col].wall_bits = UP_WALL_BIT | DOWN_WALL_BIT; \
    } \
    else \
    { \
        return false; \
    }

#define MOVE_A_STEP \
    switch(direction) \
    { \
    case UP: \
        if(row > 0) \
        { \
            row --; \
        } \
        break; \
    case DOWN: \
        if(row < HEIGHT - 1) \
        { \
            row ++; \
        } \
        break; \
    case LEFT: \
        if(col > 0) \
        { \
            col --; \
        } \
        break; \
    case RIGHT: \
        if(col < WIDTH - 1) \
        { \
            col ++; \
        } \
        break; \
    default: \
        return false; \
    }

// From other block to this block.
#define UPDATE_WALL_BIT(row, col, direction) \
    switch(direction) \
    { \
    case UP: \
        map[row][col].wall_bits &= ~DOWN_WALL_BIT; \
        break; \
    case DOWN: \
        map[row][col].wall_bits &= ~UP_WALL_BIT; \
        break; \
    case LEFT: \
        map[row][col].wall_bits &= ~RIGHT_WALL_BIT; \
        break; \
    case RIGHT: \
        map[row][col].wall_bits &= ~LEFT_WALL_BIT; \
        break; \
    default: \
        return false; \
    }

#define RECORD_PATH \
    if(recording_path) \
    { \
        if(current != NULL) \
        { \
            current->next = (struct PathNode *)malloc(sizeof(struct PathNode)); \
            if(current->next != NULL) \
            { \
                current = current->next; \
            } \
        } \
        else \
        { \
            current = (struct PathNode *)malloc(sizeof(struct PathNode)); \
            if(path_head == NULL) \
            { \
                path_head = current; \
            } \
        } \
        if(current != NULL) \
        { \
            current->row = row; \
            current->col = col; \
            current->next = NULL; \
        } \
        else \
        { \
            recording_path = false; \
        } \
    }

#define STRAIGHT_LIMIT 2

#define CREATE_PATH(require_recording) \
    if(next_row > row) \
    { \
        up_count = rand() % (HEIGHT - 1 - next_row); \
        down_count = next_row - row + up_count; \
    } \
    else if(next_row < row) \
    { \
        down_count = rand() % next_row; \
        up_count = row - next_row + down_count; \
    } \
    else \
    { \
        up_count = down_count = 0; \
    } \
    if(next_col > col) \
    { \
        left_count = rand() % (WIDTH - 1 - next_col); \
        right_count = next_col - col + left_count; \
    } \
    else if(next_col < col) \
    { \
        right_count = rand() % next_col; \
        left_count = col - next_col + right_count; \
    } \
    else \
    { \
        left_count = right_count = 0; \
    } \
    while(row != next_row || col != next_col) \
    { \
        if(up_count + down_count > 0) \
        { \
            row_step = rand() % (up_count + down_count + 1); \
            if(row_step >= up_count) \
            { \
                row_step -= up_count; \
                if(row_step > HEIGHT - 2 - row) \
                { \
                    row_step = HEIGHT - 2 - row; \
                } \
                if(row_step > STRAIGHT_LIMIT) \
                { \
                    row_step = STRAIGHT_LIMIT; \
                } \
                down_count -= row_step; \
                for(; row_step > 0; row_step --) \
                { \
                    map[row][col].wall_bits &= ~DOWN_WALL_BIT; \
                    row ++; \
                    if(map[row][col].type == Blank) \
                    { \
                        map[row][col].wall_bits = ALL_WALL_BIT; \
                    } \
                    map[row][col].type = Road; \
                    UPDATE_WALL_BIT(row, col, DOWN) \
                    if(require_recording) \
                    { \
                        RECORD_PATH \
                    } \
                } \
            } \
            else \
            { \
                row_step = up_count - row_step; \
                if(row_step > row - 1) \
                { \
                    row_step = row - 1; \
                } \
                if(row_step > STRAIGHT_LIMIT) \
                { \
                    row_step = STRAIGHT_LIMIT; \
                } \
                up_count -= row_step; \
                for(; row_step > 0; row_step --) \
                { \
                    map[row][col].wall_bits &= ~UP_WALL_BIT; \
                    row --; \
                    if(map[row][col].type == Blank) \
                    { \
                        map[row][col].wall_bits = ALL_WALL_BIT; \
                    } \
                    map[row][col].type = Road; \
                    UPDATE_WALL_BIT(row, col, UP) \
                    if(require_recording) \
                    { \
                        RECORD_PATH \
                    } \
                } \
            } \
        } \
        else \
        { \
            row_step = 0; \
        } \
        if(left_count + right_count > 0) \
        { \
            col_step = rand() % (left_count + right_count + 1); \
            if(col_step >= left_count) \
            { \
                col_step -= left_count; \
                if(col_step > WIDTH - 2 - col) \
                { \
                    col_step = WIDTH - 2 - col; \
                } \
                if(col_step > STRAIGHT_LIMIT) \
                { \
                    col_step = STRAIGHT_LIMIT; \
                } \
                right_count -= col_step; \
                for(; col_step > 0; col_step --) \
                { \
                    map[row][col].wall_bits &= ~RIGHT_WALL_BIT; \
                    col ++; \
                    if(map[row][col].type == Blank) \
                    { \
                        map[row][col].wall_bits = ALL_WALL_BIT; \
                    } \
                    map[row][col].type = Road; \
                    UPDATE_WALL_BIT(row, col, RIGHT) \
                    if(require_recording) \
                    { \
                        RECORD_PATH \
                    } \
                } \
            } \
            else \
            { \
                col_step = left_count - col_step; \
                if(col_step > col - 1) \
                { \
                    col_step = col - 1; \
                } \
                if(col_step > STRAIGHT_LIMIT) \
                { \
                    col_step = STRAIGHT_LIMIT; \
                } \
                left_count -= col_step; \
                for(; col_step > 0; col_step --) \
                { \
                    map[row][col].wall_bits &= ~LEFT_WALL_BIT; \
                    col --; \
                    if(map[row][col].type == Blank) \
                    { \
                        map[row][col].wall_bits = ALL_WALL_BIT; \
                    } \
                    map[row][col].type = Road; \
                    UPDATE_WALL_BIT(row, col, LEFT) \
                    if(require_recording) \
                    { \
                        RECORD_PATH \
                    } \
                } \
            } \
        } \
        else \
        { \
            col_step = 0; \
        } \
    }

bool init_map(unsigned int *start_row, unsigned int *start_col, unsigned int *target_row, unsigned int *target_col)
{
    if(start_row == NULL || start_col == NULL)
    {
        fprintf(stderr, "Cannot save start_row and start_col.\n");
        return false;
    }
    unsigned int row = 0, col = 0;
    unsigned int last_row = 0, last_col = 0;
    unsigned int steps = 0;
    unsigned int pole_position;
    unsigned int direction = 0, start_direction = 0;
    unsigned int next_row = 0, next_col = 0;
    // unsigned int up_weigh = 1, down_weigh = 1, left_weigh = 1, right_weigh = 1;
    for(col = 0; col < WIDTH; col ++)
    {
        map[0][col].type = Wall;
        map[0][col].wall_bits = ALL_WALL_BIT;
        map[HEIGHT - 1][col].type = Wall;
        map[HEIGHT - 1][col].wall_bits = ALL_WALL_BIT;
    }
    for(row = 1; row < HEIGHT - 1; row ++)
    {
        map[row][0].type = Wall;
        map[row][0].wall_bits = ALL_WALL_BIT;
        map[row][WIDTH - 1].type = Wall;
        map[row][WIDTH - 1].wall_bits = ALL_WALL_BIT;
    }
    GENERATE_POLE
    PLACE_POLE
    *start_row = row;
    *start_col = col;
    start_direction = direction;
    do
    {
        GENERATE_POLE
    } while(row == *start_row || col == *start_col);
    PLACE_POLE
    *target_row = row;
    *target_col = col;
    unsigned int step_end_row, step_end_col;
    MOVE_A_STEP
    map[row][col].type = Road;
    map[row][col].wall_bits = ALL_WALL_BIT;
    UPDATE_WALL_BIT(row, col, direction)
    step_end_row = row;
    step_end_col = col;
    if((start_direction == UP || start_direction == DOWN) && HEIGHT >= 9)
    {
        next_row = HEIGHT / 3 + rand() % 3;
        next_col = rand() % (WIDTH - 2) + 1;
    }
    else if((start_direction == LEFT || start_direction == RIGHT) && WIDTH >= 9)
    {
        next_col = WIDTH / 3 + rand() % 3;
        next_row = rand() % (HEIGHT - 2) + 1;
    }
    else
    {
        next_row = HEIGHT / 2;
        next_col = WIDTH / 2;
    }
    row = *start_row;
    col = *start_col;
    direction = start_direction;
    /*
    do
    {
        last_row = row;
        last_col = col;
        if(steps > 0)
        {
            if(row > 1 && direction != DOWN && map[row - 1][col] != Road)
            {
                up_weigh = middle_row < row ? row - middle_row : 1;
            }
            else
            {
                up_weigh = 0;
            }
            if(row < HEIGHT - 2 && direction != UP && map[row + 1][col] != Road)
            {
                down_weigh = middle_row > row ? middle_row - row : 1;
            }
            else
            {
                down_weigh = 0;
            }
            if(col > 1 && direction != RIGHT && map[row][col - 1] != Road)
            {
                left_weigh = middle_col < col ? col - middle_col : 1;
            }
            else
            {
                left_weigh = 0;
            }
            if(col < WIDTH - 2 && direction != LEFT && map[row][col + 1] != Road)
            {
                right_weigh = middle_col > col ? middle_col - col : 1;
            }
            else
            {
                right_weigh = 0;
            }
            if(up_weigh + down_weigh + left_weigh + right_weigh == 0)
            {
                row = last_row;
                col = last_col;
                continue;
            }
            direction = rand() % (up_weigh + down_weigh + left_weigh + right_weigh);
            if(direction < up_weigh)
            {
                direction = UP;
            }
            else if((direction -= up_weigh) < down_weigh)
            {
                direction = DOWN;
            }
            else if((direction -= down_weigh) < left_weigh)
            {
                direction = LEFT;
            }
            else if((direction -= left_weigh) < right_weigh)
            {
                direction = RIGHT;
            }
            else
            {
                return false;
            }
        }
        switch(direction)
        {
        case UP:
            if(row > 0)
            {
                row --;
            }
            break;
        case RIGHT:
            if(col < WIDTH - 1)
            {
                col ++;
            }
            break;
        case DOWN:
            if(row < HEIGHT - 1)
            {
                row ++;
            }
            break;
        case LEFT:
            if(col > 0)
            {
                col --;
            }
            break;
        default:
            return false;
        }
        printf("%d %d\n", row, col);
        if(map[row][col] == Wall)
        {
            row = last_row;
            col = last_col;
            continue;
        }
        map[row][col] = Road;
        steps ++;
        if(row == middle_row || col == middle_col)
        {
            middle_row = *target_row;
            middle_col = *target_col;
        }
    } while(row > 0 && row < HEIGHT - 1 && col > 0 && col < WIDTH - 1);
    */
    MOVE_A_STEP
    map[row][col].type = Road;
    map[row][col].wall_bits = ALL_WALL_BIT;
    UPDATE_WALL_BIT(row, col, direction)
    // unsigned int row_displacement = middle_row - *start_row, col_displacement = middle_col - *start_col;
    unsigned int row_step = 0, col_step;
    unsigned int up_count, down_count, left_count, right_count;
    struct PathNode *path_head = NULL, *current = NULL;
    bool recording_path = true;
    RECORD_PATH
    for(unsigned int i = 0; i <= 1; i ++)
    {
        CREATE_PATH(true)
        if(i == 0)
        {
            next_row = step_end_row;
            next_col = step_end_col;
        }
    }
    // Generate some wrong paths.
    for(current = path_head; current != NULL;)
    {
        row = current->row;
        col = current->col;
        next_row = rand() % (WIDTH - 2) + 1;
        next_col = rand() % (HEIGHT - 2) + 1;
        CREATE_PATH(false)
        for(unsigned int steps = rand() % 3 + 2; steps > 0; steps --)
        {
            if(current == NULL)
            {
                break;
            }
            current = current->next;
        }
        if(current != NULL)
        {
            current = current->next;
        }
    }
    while(path_head != NULL)
    {
        current = path_head->next;
        free(path_head);
        path_head = current;
    }
    return true;
}

void print_map(unsigned int player_row, unsigned int player_col)
{
    for(unsigned int row = 0; row < HEIGHT; row ++)
    {
        for(unsigned int col = 0; col < WIDTH; col ++)
        {
            putchar('+');
            if((map[row][col].wall_bits & UP_WALL_BIT) || (row > 0 && (map[row - 1][col].wall_bits & DOWN_WALL_BIT)))
            {
                putchar('-');
            }
            else
            {
                putchar(' ');
            }
        }
        puts("+");
        if(map[row][0].wall_bits & LEFT_WALL_BIT)
        {
            putchar('|');
        }
        else
        {
            putchar(' ');
        }
        for(unsigned int col = 0; col < WIDTH; col ++)
        {
            if(row != player_row || col != player_col)
            {
                switch(map[row][col].type)
                {
                case Blank:
                    putchar('?');
                    break;
                case Road:
                    putchar(' ');
                    break;
                case Wall:
                    putchar('#');
                    break;
                default:
                    putchar('?');
                    break;
                }
            }
            else
            {
                putchar('P');
            }
            // putchar(' ');
            if((map[row][col].wall_bits & RIGHT_WALL_BIT) || (col < WIDTH - 1 && (map[row][col + 1].wall_bits & LEFT_WALL_BIT)))
            {
                putchar('|');
            }
            else
            {
                putchar(' ');
            }
        }
        putchar('\n');
    }
    for(unsigned int col = 0; col < WIDTH; col ++)
    {
        putchar('+');
        if(map[HEIGHT - 1][col].wall_bits & DOWN_WALL_BIT)
        {
            putchar('-');
        }
        else
        {
            putchar(' ');
        }
    }
    puts("+");
}

void player_move(unsigned int *player_row, unsigned int *player_col, bool *game_end)
{
    bool receiving = true;
    char c = '\0';
    do
    {
        c = _getch();
        if(c == EOF)
        {
            *game_end = true;
            receiving = false;
            return;
        }
        else if(c == -32)
        {
            if((c = _getch()) == EOF)
            {
                *game_end = true;
                receiving = false;
                return;
            }
            switch(c)
            {
            case 72:
                c = 'w';
                break;
            case 77:
                c = 'd';
                break;
            case 80:
                c = 's';
                break;
            case 75:
                c = 'a';
                break;
            default:
                break;
            }
        }
        else if(c >= 'A' && c <= 'Z')
        {
            c += 'a' - 'A';
        }
        switch(c)
        {
        case 'q':
            *game_end = true;
            break;
        case 'w':
            if(*player_row > 0 && map[*player_row - 1][*player_col].type == Road
                && (map[*player_row][*player_col].wall_bits & UP_WALL_BIT) == 0
                && (map[*player_row - 1][*player_col].wall_bits & DOWN_WALL_BIT) == 0)
            {
                (*player_row) --;
                receiving = false;
            }
            break;
        case 'd':
            if(*player_col < WIDTH - 1 && map[*player_row][*player_col + 1].type == Road
                && (map[*player_row][*player_col].wall_bits & RIGHT_WALL_BIT) == 0
                && (map[*player_row][*player_col + 1].wall_bits & LEFT_WALL_BIT) == 0)
            {
                (*player_col) ++;
                receiving = false;
            }
            break;
        case 's':
            if(*player_row < HEIGHT - 1 && map[*player_row + 1][*player_col].type == Road
                && (map[*player_row][*player_col].wall_bits & DOWN_WALL_BIT) == 0
                && (map[*player_row + 1][*player_col].wall_bits & UP_WALL_BIT) == 0)
            {
                (*player_row) ++;
                receiving = false;
            }
            break;
        case 'a':
            if(*player_col > 0 && map[*player_row][*player_col - 1].type == Road
                && (map[*player_row][*player_col].wall_bits & LEFT_WALL_BIT) == 0
                && (map[*player_row][*player_col - 1].wall_bits & RIGHT_WALL_BIT) == 0)
            {
                (*player_col) --;
                receiving = false;
            }
            break;
        default:
            break;
        }
    } while(!(*game_end) && receiving);
}