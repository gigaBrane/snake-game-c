#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>  // for usleep
#include <time.h>    // for random seed

// Constants for game dimensions and initial settings
#define WIDTH 20
#define HEIGHT 20
#define INIT_SNAKE_LENGTH 3

// Structures to define Points and the Snake
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point position[WIDTH * HEIGHT];
    int length;
} Snake;

// Global variables
Point food;
int score = 0;
Snake snake;
int game_over = 0;
int direction = 'R';  // 'R' for Right, 'L' for Left, 'U' for Up, 'D' for Down

// Function to initialize game settings
void init_game() {
    initscr();            // Initialize ncurses
    clear();              // Clear the terminal screen
    noecho();             // Disable key echoing
    curs_set(0);          // Hide the cursor
    srand(time(0));       // Seed for random numbers
    
    // Initialize snake position and length
    snake.length = INIT_SNAKE_LENGTH;
    for (int i = 0; i < INIT_SNAKE_LENGTH; i++) {
        snake.position[i].x = WIDTH / 2 - i;
        snake.position[i].y = HEIGHT / 2;
    }
    
    // Place food in a random location
    food.x = rand() % WIDTH;
    food.y = rand() % HEIGHT;
}

// Function to render the game screen
void draw() {
    clear();
    for (int i = 0; i < WIDTH + 2; i++) mvprintw(0, i, "#");
    for (int i = 1; i < HEIGHT + 1; i++) {
        mvprintw(i, 0, "#");
        for (int j = 1; j <= WIDTH; j++) {
            if (i - 1 == food.y && j - 1 == food.x) {
                mvprintw(i, j, "F");  // Draw food
            } else {
                int is_snake_part = 0;
                for (int k = 0; k < snake.length; k++) {
                    if (snake.position[k].x == j - 1 && snake.position[k].y == i - 1) {
                        mvprintw(i, j, "O");  // Draw snake segment
                        is_snake_part = 1;
                        break;
                    }
                }
                if (!is_snake_part) mvprintw(i, j, " ");
            }
        }
        mvprintw(i, WIDTH + 1, "#");
    }
    for (int i = 0; i < WIDTH + 2; i++) mvprintw(HEIGHT + 1, i, "#");
    mvprintw(HEIGHT + 2, 0, "Score: %d", score);
    refresh();
}

// Function to get user input for snake direction
void get_input() {
    timeout(100);  // Delay for smoother movement
    int ch = getch();
    if (ch == 'w' && direction != 'D') direction = 'U';
    else if (ch == 's' && direction != 'U') direction = 'D';
    else if (ch == 'a' && direction != 'R') direction = 'L';
    else if (ch == 'd' && direction != 'L') direction = 'R';
}

// Function to update the snake's position
void update_snake() {
    // Move each segment to follow the previous one
    for (int i = snake.length - 1; i > 0; i--) {
        snake.position[i] = snake.position[i - 1];
    }
    
    // Move the head of the snake
    switch (direction) {
        case 'U': snake.position[0].y--; break;
        case 'D': snake.position[0].y++; break;
        case 'L': snake.position[0].x--; break;
        case 'R': snake.position[0].x++; break;
    }
}

// Function to check for collisions
void check_collisions() {
    Point head = snake.position[0];
    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) game_over = 1;
    
    // Check if the snake collides with itself
    for (int i = 1; i < snake.length; i++) {
        if (snake.position[i].x == head.x && snake.position[i].y == head.y) game_over = 1;
    }
    
    // Check if snake eats the food
    if (head.x == food.x && head.y == food.y) {
        snake.length++;
        score += 10;
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    }
}

// Main game loop
int main() {
    init_game();
    
    while (!game_over) {
        draw();
        get_input();
        update_snake();
        check_collisions();
        usleep(100000);  // Control the speed of the game
    }
    
    // End screen
    clear();
    mvprintw(HEIGHT / 2, (WIDTH - 10) / 2, "Game Over!");
    mvprintw(HEIGHT / 2 + 1, (WIDTH - 12) / 2, "Final Score: %d", score);
    refresh();
    getch();  // Wait for user input before exiting
    
    endwin();  // Close ncurses mode
    return 0;
}
