#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "pic18f45k22.h"
#include "Config.h" 
#include "time.h"
#include "lcd.h"

#define _XTAL_FREQ  8000000

// Definition of directions
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MAX_SNAKE_LENGTH 100

// Initial position of the prey
int x = 7;
int y = 1;

int score = 0; // Initialize score

// Pseudo-random number generation to position the prey
int pseudo_random(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

// Snake structure
typedef struct Snake {
    int position_x;
    int position_y;
    int length;
    int direction;
    struct Segment *segments;
} Snake;

// Segment structure (snake body)
typedef struct Segment {
    int position_x;
    int position_y;
} Segment;

Snake snake;
Segment snake_segments[MAX_SNAKE_LENGTH];

// Initialize the snake
void initialize_snake(Snake *snake) {
    snake->position_x = 0; // Initial position of the snake's head
    snake->position_y = 3;
    snake->length = 1;
    snake->direction = RIGHT;
    snake->segments = snake_segments;
}

// Variables to store new position of the snake
int new_position_x;
int new_position_y;

// Move the snake
void move_snake(Snake *snake) {
    // Save the coordinates of the head before modifying them
    int old_position_x = snake->position_x;
    int old_position_y = snake->position_y;

    // Determine the new position of the head
    new_position_x = old_position_x;
    new_position_y = old_position_y;
    switch (snake->direction) {
        case UP:
            new_position_y--;
            break;
        case DOWN:
            new_position_y++;
            break;
        case LEFT:
            new_position_x--;
            break;
        case RIGHT:
            new_position_x++;
            break;
    }

    // Update the position of the head
    snake->position_x = new_position_x;
    snake->position_y = new_position_y;

    // Move the segments of the snake
    for (int i = snake->length - 1; i > 0; i--) {
        snake_segments[i].position_x = snake_segments[i - 1].position_x;
        snake_segments[i].position_y = snake_segments[i - 1].position_y;
    }

    // Update the position of the initial segment with the old head coordinates
    snake_segments[0].position_x = old_position_x;
    snake_segments[0].position_y = old_position_y;
}

// Function to display the snake
void display_snake(Snake *snake) {
    // Clear the screen
    ClearLCD();
  
    // Set cursor position for the snake's head
    SetCursor(snake->position_y, snake->position_x);

    // Display the snake's head
    print("*");

    // Set cursor position for each segment of the snake
    for (int i = 0; i < snake->length; i++) {
        SetCursor(snake->segments[i].position_y, snake->segments[i].position_x);
    
        // Display the snake segment
        print("o");
    }
}

// Function to initialize ports
void init_ports() {
    ANSELD = 0;
    TRISD = 1;
}

// Function to move the snake based on user input
void snake_move(Snake *snake) {
    unsigned char right = PORTDbits.RD4;
    unsigned char left = PORTDbits.RD1;
    unsigned char up = PORTDbits.RD2;
    unsigned char down = PORTDbits.RD3;
    if (right == 1) {
        snake->direction = RIGHT;
    }
    if (down == 1) {
        snake->direction = DOWN;
    }
    if (left == 1) {
        snake->direction = LEFT;
    }
    if (up == 1) {
        snake->direction = UP;
    }
}

// Function to detect collisions
int detect_collision(Snake *snake) {
    // Check collisions with screen borders
    if (snake->position_x < 0 || snake->position_x >= 20 || snake->position_y < 0 || snake->position_y >= 4) {
        return 1; // Collision with screen borders
    }

    // Check collisions with snake's body
    for (int i = 0; i < snake->length; i++) {
        if (snake->position_x == snake->segments[i].position_x && snake->position_y == snake->segments[i].position_y) {
            return 1; // Collision with snake's body
        }
    }

    return 0; // No collision detected
}

// Function to generate prey
void generate_prey() {
    do {
        x = pseudo_random(0, 18);
        y = pseudo_random(0, 3);
        // Check that the prey is not generated on any part of the snake's body
        for (int i = 0; i < snake.length; i++) {
            if ((x == snake.segments[i].position_x && y == snake.segments[i].position_y)) {
                // If the prey is generated on any part of the snake's body, try again with new coordinates
                x = -1; // Mark coordinates as invalid
                y = -1;
                break;
            }
        }
    } while (x == -1 || y == -1); // Continue generating new coordinates until they are valid
}

// Function to display prey
void display_prey() {
    SetCursor(y, x);
    print("*");
}

void main() {
    // Initialize the snake and ports
    unsigned char seed = 52; // For example, you can use any value here

    // Use the value of seed as seed for srand()
    srand((unsigned int)seed);
    unsigned char P;
    TRISB = 0;
    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    P = 0X38;
    SendCommand(P);
    P = 0X0C;
    SendCommand(P);
    P = 0X01;
    SendCommand(P);
    init_ports();
    initialize_snake(&snake);

    int game_status = 2; // Game control variable

    // Main game loop
    while (1) {
        // Check if the game is ongoing
        while (game_status == 2) {
            if (PORTDbits.RD6 == 0) {
                // Display game start message if START button is pressed
                SetCursor(0, 5);
                print("SNAKE GAME");
                SetCursor(1, 5);
                print("----------");
                SetCursor(2, 0);
                print("Press START to play!");
                SetCursor(3, 0);
                print("********************");
            } else {
                game_status = 0;
                break;
            }
        }
        
        if (game_status == 0) {
            // Move the snake
            snake_move(&snake); 
            move_snake(&snake);
            if (detect_collision(&snake)) {
                // Display "GAME OVER" if collision is detected
                game_status = 1; 
                // End the game
            }

            // Handle collisions

            // Display the snake
            display_snake(&snake);
            display_prey();
            if (snake.position_x == 0 && snake.position_y == 0) {
                SetCursor(0, 0);
                print(" ");
            }
            __delay_ms(750);
            if (snake.position_x == x && snake.position_y == y) {
                SetCursor(y, x);
                print(" "); // Clear prey from the screen

                // Increase snake length by adding a segment
                snake.length++;
                score++;

                // Generate a new prey
                generate_prey();
            }
        } else {
            ClearLCD();
            // The game is over, permanently display "GAME OVER"
            while (1) {
                SetCursor(1, 5);
                print("GAME OVER");
                SetCursor(3, 3);
                print("SCORE:");
                
                char s[3];
                sprintf(s, "%d", score);
                SetCursor(3, 11);
                print(s);
                if (PORTDbits.RD6 == 1) {
                    game_status = 2;
                    break;
                }
                
            }
        }
    }
}
