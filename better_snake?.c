#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

typedef struct { int x, y; } Point;

Point snake[256];
int snake_length = 1, food_x, food_y, direction;
int game_over = 0;

void init_game();
void draw_game();
void spawn_food();
void update_snake();
void move_snake(Point *new_head);
int check_collision(Point *new_head);
void shift_snake_body();
void draw_walls();
void draw_food();
void draw_snake();
void handle_user_input();
void update_direction(int key);
void game_over_screen();

void init_game() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    noecho();
    timeout(-1);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
    }

    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    direction = 0;
    
    spawn_food();
}

void draw_game() {
    clear();
    draw_walls();
    draw_food();
    draw_snake();
}

void draw_walls() {
    attron(COLOR_PAIR(2));
    for (int i = 0; i < WIDTH; i++) mvprintw(0, i, "#");
    for (int i = 0; i < WIDTH; i++) mvprintw(HEIGHT, i, "#");
    for (int i = 0; i <= HEIGHT; i++) mvprintw(i, 0, "#");
    for (int i = 0; i <= HEIGHT; i++) mvprintw(i, WIDTH, "#");
    attroff(COLOR_PAIR(2));
}

void draw_food() {
    attron(COLOR_PAIR(3));
    mvprintw(food_y, food_x, "O");
    attroff(COLOR_PAIR(3));
}

void draw_snake() {
    attron(COLOR_PAIR(1));
    for (int i = 0; i < snake_length; i++) 
        mvprintw(snake[i].y, snake[i].x, "o");
    attroff(COLOR_PAIR(1));
}

void spawn_food() {
    food_x = 2 + rand() % (WIDTH - 4);
    food_y = 2 + rand() % (HEIGHT - 2);
}

void update_snake() {
    Point new_head = snake[0];
    move_snake(&new_head);

    if (check_collision(&new_head)) {
        game_over = 1;
    } else {
        shift_snake_body();
        snake[0] = new_head;
    }
}

void move_snake(Point *new_head) {
    switch (direction) {
        case KEY_UP:    new_head->y--; break;
        case KEY_DOWN:  new_head->y++; break;
        case KEY_LEFT:  new_head->x--; break;
        case KEY_RIGHT: new_head->x++; break;
    }
}

int check_collision(Point *new_head) {
    if (new_head->x <= 0 || new_head->x >= WIDTH || new_head->y <= 0 || new_head->y >= HEIGHT)
        return 1;

    for (int i = 0; i < snake_length; i++)
        if (snake[i].x == new_head->x && snake[i].y == new_head->y)
            return 1;

    if (new_head->x == food_x && new_head->y == food_y) {
        snake_length++;
        spawn_food();
    }

    return 0;
}

void shift_snake_body() {
    for (int i = snake_length - 1; i > 0; i--)
        snake[i] = snake[i - 1];
}

void handle_user_input() {
    int key = getch();
    if (key != ERR && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT)) {
        update_direction(key);
    }
}

void update_direction(int key) {
    if ((direction == KEY_UP && key != KEY_DOWN) ||
        (direction == KEY_DOWN && key != KEY_UP) ||
        (direction == KEY_LEFT && key != KEY_RIGHT) ||
        (direction == KEY_RIGHT && key != KEY_LEFT)) {
        direction = key;
    }
}

void game_over_screen() {
    endwin();
    printf("Game Over! Final Length: %d\n", snake_length);
}

int main() {
    srand(time(0));
    init_game();

    while (direction == 0) {
        handle_user_input();
    }

    timeout(100);
    while (!game_over) {
        handle_user_input();
        update_snake();
        draw_game();
    }

    game_over_screen();
    return 0;
}
