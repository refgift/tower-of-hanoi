#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>

#define MAX_DISKS 10
#define MAX_HEIGHT 10

int pegs[3][MAX_DISKS];
int tops[3];
int num_disks;

void init_pegs() {
    for (int i = 0; i < 3; i++) {
        tops[i] = -1;
    }
    for (int i = 0; i < num_disks; i++) {
        pegs[0][i] = num_disks - i;
        tops[0] = num_disks - 1;
    }
}

int can_move(int from, int to) {
    if (tops[from] == -1) return 0; // empty
    if (tops[to] == -1) return 1; // can move to empty
    return pegs[from][tops[from]] < pegs[to][tops[to]];
}

void move_disk(int from, int to) {
    if (!can_move(from, to)) return;
    int disk = pegs[from][tops[from]--];
    pegs[to][++tops[to]] = disk;
}

void draw() {
    clear();
    box(stdscr, 0, 0); // draw border
    int offset_y = LINES / 2 - (MAX_HEIGHT + 3) / 2;
    int offset_x = COLS / 2 - 35;
    int start_y = 2 + offset_y;
    int peg_x[3] = {15 + offset_x, 35 + offset_x, 55 + offset_x};
    int base_y = start_y + MAX_HEIGHT + 1;

    // draw base
    mvprintw(base_y, 1 + offset_x, "==================================================================");
    
    // draw pegs and disks
    for (int p = 0; p < 3; p++) {
        // draw pole
        for (int h = 0; h < MAX_HEIGHT; h++) {
            mvprintw(start_y + h, peg_x[p], "|");
        }
        // draw disks
        for (int d = 0; d <= tops[p]; d++) {
            int disk_size = pegs[p][d];
            int width = disk_size * 2 + 1;
            int left = peg_x[p] - disk_size;
            int y = base_y - 1 - d;
            int color_pair = (disk_size - 1) % 7 + 1;
            attron(COLOR_PAIR(color_pair));
            for (int w = 0; w < width; w++) {
                mvprintw(y, left + w, "#");
            }
            attroff(COLOR_PAIR(color_pair));
        }
    }
    refresh();
}

void hanoi(int n, int from, int to, int aux) {
    if (n == 1) {
        move_disk(from, to);
        draw();
        { struct timespec ts = {0, 500000000}; nanosleep(&ts, NULL); }
        return;
    }
    hanoi(n-1, from, aux, to);
    move_disk(from, to);
    draw();
    { struct timespec ts = {0, 500000000}; nanosleep(&ts, NULL); }
    hanoi(n-1, aux, to, from);
}

int main() {
    printf("Enter number of disks (1-%d): ", MAX_DISKS);
    scanf("%d", &num_disks);
    if (num_disks < 1 || num_disks > MAX_DISKS) {
        printf("Invalid number\n");
        return 1;
    }
    
    init_pegs();
    
    time_t start_time = time(NULL);
    
    initscr();
    noecho();
    curs_set(0);
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
    }
    
    draw();
    { struct timespec ts = {1, 0}; nanosleep(&ts, NULL); } // show initial
    
    hanoi(num_disks, 0, 2, 1);
    
    time_t end_time = time(NULL);
    double time_taken = difftime(end_time, start_time);
    
    int msg_y = LINES / 2 - (MAX_HEIGHT + 3) / 2;
    mvprintw(msg_y, COLS / 2 - 15, "Solved! Press any key to exit");
    mvprintw(msg_y + 1, COLS / 2 - 20, "Time taken: %.2f seconds", time_taken);
    refresh();
    getch();
    endwin();
    
    return 0;
}