# Cnake Game

Cnake Game is a classic Snake game implemented in C. The game involves controlling a snake to eat food and grow in length while avoiding collisions with the walls or itself. It works using the `ncurses` library. The game is played within a terminal, where the player controls the snake to eat food and grow longer while avoiding collisions with walls and itself.

## What It Does

- Allows the player to control the snake via keyboard.
- The snake grows when it eats food
- The game ends when the snake collides with the walls or itself

## How It Works

`snake.c` includes `ncurses`, `stdlib`, and `time` and defines the dimensions of the grid as 40x20.

```c
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
```

### Representing the Snake

The snake itself is defined as an array of coordinates, created using a C `struct`.

```c
typedef struct {
    int x, y;
} Point;

Point snake[100];
```

### Global Variables

Several important variables are defined here:

```c
int snake_length;
int food_x, food_y;
int direction;
int game_over = 0;
```

### Initializing the Game

The game is initialized using the following steps:

1. `initscr()` - Initialize the ncurses library
2. `curs_set(0)` - Hide the cursor
3. `keypad(stdscr, TRUE)` - Enable arrow key input
4. ` noecho()` - Disable keyboard echoing (writing characters to the terminal)
5. `timeout(-1)` - Wait for a keypress to start the game
6. `snake_length = 1` - Start the snake
7. Setting the snake's starting coordinates
8. Spawning the first food in valid coordinates
9. Setting no initial movement direction for the snake

Which are outlined in this method:

```c
void init_game() {
    initscr();             ///< Initialize the ncurses library
    curs_set(0);           ///< Hide the cursor
    keypad(stdscr, TRUE);  ///< Enable keypad input (e.g., arrow keys)
    noecho();              ///< Disable echoing of input characters
    timeout(-1);           ///< Wait for a keypress before starting the game

    snake_length = 1;
    snake[0].x = WIDTH / 2;   ///< Set the snake's initial x-coordinate
    snake[0].y = HEIGHT / 2;  ///< Set the snake's initial y-coordinate

    // Ensure food spawns within a valid range
    food_x = 2 + rand() % (WIDTH - 4);
    food_y = 2 + rand() % (HEIGHT - 2);

    direction = 0;  ///< No direction initially
}
```

### Rendering the Game

The `draw_game()` function clears the screen and draws the walls, snake, and food.

```c
void draw_game() {
    clear();  ///< Clear the screen

    // Draw top and bottom walls
    for (int i = 0; i < WIDTH; i++) mvprintw(0, i, "#");
    for (int i = 0; i < WIDTH; i++) mvprintw(HEIGHT, i, "#");

    // Draw left and right walls
    for (int i = 0; i <= HEIGHT; i++) mvprintw(i, 0, "#");
    for (int i = 0; i <= HEIGHT; i++) mvprintw(i, WIDTH, "#");

    mvprintw(food_y, food_x, "O"); // Draw food

    for (int i = 0; i < snake_length; i++)
        mvprintw(snake[i].y, snake[i].x, "o"); // Draw snake body
}
```

### Generating the Food

The `spawn_food()` function randomly generates the position of food within valid grid coordinates.

```c
void spawn_food() {
    food_x = 2 + rand() % (WIDTH - 4);   ///< Randomize the x-coordinate of the food
    food_y = 2 + rand() % (HEIGHT - 2);  ///< Randomize the y-coordinate of the food
}
```

### Handling Snake Movement and Growth

The `update_snake()` function updates the snake’s position based on the current direction. It checks for collisions with walls, the snake itself, and food. If the snake eats food, it grows in length. The snake’s body is then updated to reflect its new position.

```c
void update_snake() {
    Point new_head = snake[0];

    // Move the snake in the current direction
    switch (direction) {
        case KEY_UP:    new_head.y--; break;
        case KEY_DOWN:  new_head.y++; break;
        case KEY_LEFT:  new_head.x--; break;
        case KEY_RIGHT: new_head.x++; break;
    }

    // Check for collisions with walls
    if (new_head.x <= 0 || new_head.x >= WIDTH || new_head.y <= 0 || new_head.y >= HEIGHT)
        game_over = 1;

    // Check for collisions with the snake itself
    for (int i = 0; i < snake_length; i++)
        if (snake[i].x == new_head.x && snake[i].y == new_head.y)
            game_over = 1;

    // Check for food collision
    if (new_head.x == food_x && new_head.y == food_y) {
        snake_length++;        ///< Increase snake's length
        spawn_food();          ///< Spawn new food
    }

    // Update the snake's body
    for (int i = snake_length - 1; i > 0; i--)
        snake[i] = snake[i - 1];  ///< Move each body segment to the position of the previous segment
    snake[0] = new_head;  ///< Set the new head position
}
```

### Main Loop

The `main()` function is the game loop. It initializes the game, waits for the player to press a direction key, and runs the game until it ends. During the game loop, it handles user input, updates the snake’s position, and draws the game.

```c
int main() {
    srand(time(0));  ///< Seed the random number generator with the current time
    init_game();     ///< Initialize the game state

    // Wait for arrow key input to start the game
    while (direction == 0) {
        int key = getch();  ///< Get user input
        if (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT)
            direction = key;  ///< Set the initial direction
    }

    timeout(100);  ///< Set a timeout for the game loop (100ms)

    // Game loop
    while (!game_over) {
        int key = getch();  ///< Get user input for direction change
        if (key != ERR && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT)) {
            // Prevent reversing direction
            if ((direction == KEY_UP && key != KEY_DOWN) ||
                (direction == KEY_DOWN && key != KEY_UP) ||
                (direction == KEY_LEFT && key != KEY_RIGHT) ||
                (direction == KEY_RIGHT && key != KEY_LEFT)) {
                direction = key;  ///< Update the direction if valid
            }
        }

        update_snake();  ///< Update the snake's position
        draw_game();     ///< Redraw the game screen
    }

    endwin();  ///< End ncurses mode
    printf("Game Over! Final Length: %d\n", snake_length);  ///< Print the final game over message
    return 0;
}
```
