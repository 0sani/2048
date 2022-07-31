#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/ioctl.h>
#include <ncurses.h>

// gcc -Wall 2048.c -lncurses

int* initGrid();
void makeMove(int* grid, int dir);
int isAlive(int* grid);
void display(WINDOW *screen, int *grid);

int score = 0;

int main(void) {

    srand(time(NULL));

    WINDOW *screen;

    initscr();
    clear();
    noecho();
    halfdelay(1);

    screen = newwin(15,31,7,7);
    keypad(screen, TRUE);

    int* grid = initGrid();

    display(screen, grid);

    int alive = isAlive(grid);
    
    // char for input
    int c;

    while (alive) {

        // await keyboard press
        int dir = -1;
        
        c = wgetch(screen);

        switch(c) {
            case KEY_UP:
                dir = 2;
                break;
            case KEY_DOWN:
                dir = 3;
                break;
            case KEY_LEFT:
                dir = 0;
                break;
            case KEY_RIGHT:
                dir = 1;
                break;
            default:
                break;
        }
        
        // make move
        if (dir != -1) {
            makeMove(grid, dir);
            display(screen, grid);
        }
    }    

    sleep(5);    

    clear();
    mvwprintw(screen, 0, 0, "You failed");
    

    // Ending protocol
    clrtoeol();
    refresh();
    endwin();
    return 0;
}

void display(WINDOW *screen, int* grid) {

    box(screen, 0, 0);

    int xoff = 7;
    int yoff = 4;

    for (int i = 0; i < 5; i++) {
        mvwprintw(screen, yoff+2*i, xoff,"+---+---+---+---+");
    }
    for (int i = 0; i < 4; i++) {
        mvwprintw(screen, yoff+1+2*i,xoff, "|   |   |   |   |");
    }
    wrefresh(screen);


    int x = 0;
    int y = 0;

    for (int i = 0; i < 16; i++) {
        x = i % 4;
        y = i / 4;

        int num = grid[i];
        char icon;
        if (num == 0) {
            icon = ' ';
        } else {
            icon = 64 + num;
        }
        
        mvwprintw(screen, yoff+1+2*y, xoff+2+4*x, "%c", icon);
        // mvwprintw(screen, yoff+1+2*y, xoff+2+4*x, "%d", num);
        
    }
    mvwprintw(screen, 1, 1, "Score: %-10d", score);
    wrefresh(screen);
    return;
}

void makeMove(int* grid, int dir) {

    int changed = 0;

    // slide the tiles:
    // left
    if (dir == 0) {
        // iterate through rows
        for (int row = 0; row < 4; row++) {
            int offset = row*4;

            // slides numbers to left
            int open = 1;
            while (open) {
                open = 0;
                for (int i = 0; i < 3; i++) {
                    if (grid[offset+i] == 0 && grid[offset+i+1] != 0) {
                        grid[offset+i] = grid[offset+i+1];
                        grid[offset+i+1] = 0;
                        changed = 1;
                        open = 1;
                    }
                }
            }
            // combines
            for (int i = 0; i < 3; i++) {
                if (grid[offset+i] != 0 && grid[offset+i] == grid[offset+i+1]) {
                    grid[offset+i]++;
                    grid[offset+i+1] = 0;
                    score += pow(2, grid[offset+i]);
                    changed = 1;
                }
            }
            // slides numbers to left (after combining)
            open = 1;
            while (open) {
                open = 0;
                for (int i = 0; i < 3; i++) {
                    if (grid[offset+i] == 0 && grid[offset+i+1] != 0) {
                        grid[offset+i] = grid[offset+i+1];
                        grid[offset+i+1] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
        }
    }
    // right
    if (dir == 1) {
        // iterate through rows
        for (int row = 3; row > -1; row--) {
            int offset = row*4;

            // slides numbers to left
            int open = 1;
            while (open) {
                open = 0;
                for (int i = 3; i > 0; i--) {
                    if (grid[offset+i] == 0 && grid[offset+i-1] != 0) {
                        grid[offset+i] = grid[offset+i-1];
                        grid[offset+i-1] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
            // combines
            for (int i = 3; i > 0; i--) {
                if (grid[offset+i] != 0 && grid[offset+i] == grid[offset+i-1]) {
                    grid[offset+i]++;
                    grid[offset+i-1] = 0;
                    score += pow(2, grid[offset+i]);
                    changed = 1;
                }
            }
            // slides numbers to left (after combining)
            open = 1;
            while (open) {
                open = 0;
                for (int i = 3; i > 0; i--) {
                    if (grid[offset+i] == 0 && grid[offset+i-1] != 0) {
                        grid[offset+i] = grid[offset+i-1];
                        grid[offset+i-1] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
        }
    }
    // up
    if (dir == 2) {
        // iterate through cols
        for (int col = 0; col < 4; col++) {
            int offset = col;

            // slides numbers down
            int open = 1;
            while (open) {
                open = 0;
                for (int i = 0; i < 3; i++) {
                    if (grid[offset+4*i] == 0 && grid[offset+4*i+4] != 0) {
                        grid[offset+4*i] = grid[offset+4*i+4];
                        grid[offset+4*i+4] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
            // combines
            for (int i = 0; i < 3; i++) {
                if (grid[offset+4*i] != 0 && grid[offset+4*i] == grid[offset+4*i+4]) {
                    grid[offset+4*i]++;
                    grid[offset+4*i+4] = 0;
                    score += pow(2, grid[offset+i]);
                    changed = 1;
                }
            }
            // slides numbers down (after combining)
            open = 1;
            while (open) {
                open = 0;
                for (int i = 0; i < 3; i++) {
                    if (grid[offset+4*i] == 0 && grid[offset+4*i+4] != 0) {
                        grid[offset+4*i] = grid[offset+4*i+4];
                        grid[offset+4*i+4] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
        }
    }
    // down
    if (dir == 3) {
        // iterate through cols
        for (int col = 3; col > -1; col--) {
            int offset = col;

            // slides numbers up
            int open = 1;
            while (open) {
                open = 0;
                for (int i = 3; i > 0; i--) {
                    if (grid[offset+4*i] == 0 && grid[offset+4*i-4] != 0) {
                        grid[offset+4*i] = grid[offset+4*i-4];
                        grid[offset+4*i-4] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
            // combines
            for (int i = 3; i > 0; i--) {
                if (grid[offset+4*i] != 0 && grid[offset+4*i] == grid[offset+4*i-4]) {
                    grid[offset+4*i]++;
                    grid[offset+4*i-4] = 0;
                    score += pow(2, grid[offset+i]);
                    changed = 1;
                }
            }
            // slides numbers up (after combining)
            open = 1;
            while (open) {
                open = 0;
                for (int i = 3; i > 0; i--) {
                    if (grid[offset+4*i] == 0 && grid[offset+4*i-4] != 0) {
                        grid[offset+4*i] = grid[offset+4*i-4];
                        grid[offset+4*i-4] = 0;
                        open = 1;
                        changed = 1;
                    }
                }
            }
        }
    }

    if (changed == 0) return;

    // generate random numbers
    // gets empty squares
    int found = 0;
    int num;
    while (!found) {
        num = rand() % 16;
        if (!grid[num]) {
            int choice = (rand() % 4) / 3;
            grid[num] = choice + 1;
            found = 1;
        }
    }
    return;
}

int isAlive(int* grid) {
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int square = grid[4*i+j];
            if (square == 0) return 1;
            if (i > 0 && square == grid[4*i+j-4]) return 1;
            if (i < 3 && square == grid[4*i+j+4]) return 1;
            if (j > 0 && square == grid[4*i+j-1]) return 1;
            if (j < 3 && square == grid[4*i+j+1]) return 1;
        }   
    }
    return 0;
}

int* initGrid() {
    int *grid = (int*)calloc(16, sizeof(int));

    

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            grid[4*i+j] = 0;
        }    
    }

    int a = rand() % 16;
    int b = rand() % 16;
    while (a==b) {
        b = rand() % 16;
    }
    grid[a] = (rand() % 4 / 3) + 1;
    grid[b] = (rand() % 4 / 3) + 1;

    return grid;
}
