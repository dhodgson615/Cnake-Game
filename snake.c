#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

typedef struct { int x, y; } Point;

Point snake[256];

int snake_length, food_x, food_y, direction;
int game_over = 0;

void init_game() {
    initscr();             // Initialize ncurses
    curs_set(0);           // Hide cursor
    keypad(stdscr, TRUE);  // Enable arrow keys
    noecho();              // Disable key echo
    timeout(-1);           // Wait for keypress before starting

    snake_length = 1;
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;

    // Ensure food spawns within a valid range
    food_x = 2 + rand() % (WIDTH - 4);
    food_y = 2 + rand() % (HEIGHT - 2);

    direction = 0;
}

void draw_game() {
    clear();
    
    // Draw top and bottom walls
    for (int i = 0; i < WIDTH; i++) mvprintw(0, i, "#");
    for (int i = 0; i < WIDTH; i++) mvprintw(HEIGHT, i, "#");

    // Draw left and right walls
    for (int i = 0; i <= HEIGHT; i++) mvprintw(i, 0, "#");
    for (int i = 0; i <= HEIGHT; i++) mvprintw(i, WIDTH, "#");

    mvprintw(food_y, food_x, "O"); // Draw food

    for (int i = 0; i < snake_length; i++)
        mvprintw(snake[i].y, snake[i].x, "o"); // Draw snake
}

void spawn_food() {
    food_x = 2 + rand() % (WIDTH - 4);
    food_y = 2 + rand() % (HEIGHT - 2);
}

void update_snake() {
    Point new_head = snake[0];
    switch (direction) {
        case KEY_UP:    new_head.y--; break;
        case KEY_DOWN:  new_head.y++; break;
        case KEY_LEFT:  new_head.x--; break;
        case KEY_RIGHT: new_head.x++; break;
    }

    // Check collisions with walls
    if (new_head.x <= 0 || new_head.x >= WIDTH || new_head.y <= 0 || new_head.y >= HEIGHT)
        game_over = 1;

    // Check collisions with self
    for (int i = 0; i < snake_length; i++)
        if (snake[i].x == new_head.x && snake[i].y == new_head.y)
            game_over = 1;

    // Check food collision
    if (new_head.x == food_x && new_head.y == food_y) {
        snake_length++;
        spawn_food();
    }

    // Update snake body
    for (int i = snake_length - 1; i > 0; i--)
        snake[i] = snake[i - 1];
    snake[0] = new_head;
}

int main() {
    srand(time(0));
    init_game();

    // Wait for arrow key input
    while (direction == 0) {
        int key = getch();
        if (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT)
            direction = key;
    }

    timeout(100);  // Set a 0.1 second delay

    while (!game_over) {
        int key = getch();
        if (key != ERR && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT)) {
            // Prevent reversing direction
            if ((direction == KEY_UP && key != KEY_DOWN) ||
                (direction == KEY_DOWN && key != KEY_UP) ||
                (direction == KEY_LEFT && key != KEY_RIGHT) ||
                (direction == KEY_RIGHT && key != KEY_LEFT)) {
                direction = key;
            }
        }
        update_snake();
        draw_game();
    }
    endwin();
    printf("Game Over! Final Length: %d\n", snake_length);
    return 0;
}
