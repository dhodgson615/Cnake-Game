#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

typedef struct { int x, y; } Point;

Point snake[1024]; // Todo: make dynamically allocated

int snake_length;
int food_x;
int food_y;
int direction;
int width = 40;
int height = 20;
int delay = 100;
int game_over = 0;
int acceleration = 0;

void init_game()
{
    initscr();            // Initialize ncurses
    curs_set(0);          // Hide cursor
    keypad(stdscr, TRUE); // Enable arrow keys
    noecho();             // Disable key echo
    timeout(-1);          // Wait for keypress before starting
    if (has_colors())     // Check if terminal supports colors
    {
        start_color();                           // Enable color
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Snake
        init_pair(2, COLOR_RED, COLOR_BLACK);    // Walls
        init_pair(3, COLOR_GREEN, COLOR_BLACK);  // Food
    }
    snake_length = 1;
    snake[0].x = width / 2;
    snake[0].y = height / 2;

    food_x = 2 + rand() % (width - 4);  // Random x position
    food_y = 2 + rand() % (height - 2); // Random y position

    direction = 0; // Wait for arrow key input
}

void draw_game()
{
    clear();
    attron(COLOR_PAIR(2));
    for (int i = 0; i < width; i++) mvprintw(0, i, "#");       // Top wall
    for (int i = 0; i < width; i++) mvprintw(height, i, "#");  // Bottom wall
    for (int i = 0; i <= height; i++) mvprintw(i, 0, "#");     // Left wall
    for (int i = 0; i <= height; i++) mvprintw(i, width, "#"); // Right wall
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvprintw(food_y, food_x, "O"); // Food
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(1));
    for (int i = 0; i < snake_length; i++) mvprintw(snake[i].y, snake[i].x, "o"); // Snake
    attroff(COLOR_PAIR(1));

    mvprintw(height + 2, 0, "Score: %d", snake_length); // Score
}

void spawn_food()
{
    food_x = 2 + rand() % (width - 4); // Random x position
    food_y = 2 + rand() % (height - 2); // Random y position
}

void update_snake()
{
    Point new_head = snake[0];
    switch (direction)
    {
        case KEY_UP:    new_head.y--; break;
        case KEY_DOWN:  new_head.y++; break;
        case KEY_LEFT:  new_head.x--; break;
        case KEY_RIGHT: new_head.x++; break;
    }
    if (new_head.x <= 0 || new_head.x >= width || new_head.y <= 0 || new_head.y >= height) game_over = 1;           // Hit wall
    for (int i = 0; i < snake_length; i++) if (snake[i].x == new_head.x && snake[i].y == new_head.y) game_over = 1; // Hit self
    if (new_head.x == food_x && new_head.y == food_y) { snake_length++; spawn_food(); }                             // Eat food
    for (int i = snake_length - 1; i > 0; i--) snake[i] = snake[i - 1]; snake[0] = new_head;                        // Update head
    if (acceleration) delay = 180 - snake_length;                                                                   // Adjust delay dynamically
}

void apply_difficulty(char difficulty)
{
    switch (difficulty)
    {
        case 'e': // Easy
            delay = 110;
            width += 5;
            height += 5;
            break;
        case 'n': // Normal
            break; // Default settings
        case 'h': // Hard
            delay = 90;
            width -= 5;
            height -= 5;
            break;
        case 'a': // Acceleration
            acceleration = 1;
            delay = 180 - snake_length;
            break;
        case 'c': // Custom
            clear();
            echo();
            mvprintw(5, 10, "Enter width: ");
            scanw("%d", &width);
            mvprintw(6, 10, "Enter height: ");
            scanw("%d", &height);
            mvprintw(7, 10, "Enter delay (ms): ");
            scanw("%d", &delay);
            mvprintw(8, 10, "Acceleration? (y/n): ");
            char acc;
            scanw(" %c", &acc);
            acceleration = (acc == 'y' || acc == 'Y') ? 1 : 0;
            noecho();
            break;
        default:
            break; // Keep default settings if invalid input
            
char get_difficulty()
{
    clear();
    mvprintw(5, 10, "Select difficulty:");
    mvprintw(6, 10, "e - Easy");
    mvprintw(7, 10, "n - Normal");
    mvprintw(8, 10, "h - Hard");
    mvprintw(9, 10, "a - Acceleration");
    mvprintw(10, 10, "c - Custom");
    char choice;
    do { choice = getch(); }
        while (choice != 'e' && choice != 'n' && choice != 'h' && choice != 'a' && choice != 'c');
    return choice;
}

int main()
{
    srand(time(0));
    initscr();
    char difficulty = get_difficulty();
    apply_difficulty(difficulty);
    init_game();

    while (direction == 0)
    {   // Wait for arrow key input
        int key = getch(); // Get keypress
        if (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT) direction = key; // Set direction
    }

    timeout(delay);

    while (!game_over)
    {
        int key = getch();
        if (key != ERR && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT))
        {
            if ((direction == KEY_UP && key != KEY_DOWN) || (direction == KEY_DOWN && key != KEY_UP)||
                (direction == KEY_LEFT && key != KEY_RIGHT) || (direction == KEY_RIGHT && key != KEY_LEFT))
                direction = key; // Update direction
        }
        update_snake(); // Update snake position
        draw_game();    // Draw game
    }

    printf("Game Over! Final Length: %d\n", snake_length); // Print final score
    timeout(-1);
    endwin(); // End ncurses
    return 0;
}
