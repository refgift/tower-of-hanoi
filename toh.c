#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>

#define MAX_DISKS 12
#define MAX_MOVES (1 << MAX_DISKS)

typedef struct {
  int from, to;
} Move;

int pegs[3][MAX_DISKS];
int tops[3];
int num_disks;
int move_count;
Move history[MAX_MOVES];
int hist_len;
int pending_from;
int abort_solve;
double last_elapsed;

double now_sec(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
    return ts.tv_sec + ts.tv_nsec * 1e-9;
  return time(NULL);
}

void init_pegs(void) {
  int i;
  for (i = 0; i < 3; i++) tops[i] = -1;
  for (i = 0; i < num_disks; i++) {
    pegs[0][i] = num_disks - i;
  }
  tops[0] = num_disks - 1;
}

void init_state(void) {
  init_pegs();
  move_count = 0;
  hist_len = 0;
  pending_from = -1;
  abort_solve = 0;
  last_elapsed = 0.0;
}

int can_move(int from, int to) {
  if (tops[from] == -1) return 0;
  if (tops[to] == -1) return 1;
  return pegs[from][tops[from]] < pegs[to][tops[to]];
}

void apply_move(int from, int to) {
  if (!can_move(from, to)) return;
  int d = pegs[from][tops[from]--];
  pegs[to][++tops[to]] = d;
  if (hist_len < MAX_MOVES) {
    history[hist_len].from = from;
    history[hist_len].to = to;
    hist_len++;
  }
  move_count++;
}

void undo(void) {
  if (hist_len == 0) return;
  Move m = history[--hist_len];
  int d = pegs[m.to][tops[m.to]--];
  pegs[m.from][++tops[m.from]] = d;
  move_count--;
}

int is_solved(void) {
  return tops[2] == num_disks - 1;
}

void draw(void) {
  int p, d, y, w, left, color, base_y;
  int peg_x[3];
  int max_d = num_disks;
  clear();
  box(stdscr, 0, 0);
  peg_x[0] = 2 + max_d;
  peg_x[1] = COLS / 2;
  peg_x[2] = COLS - 3 - max_d;
  base_y = LINES - 6;
  left = peg_x[0] - max_d;
  int r = peg_x[2] + max_d;
  for (w = left; w <= r; w++) mvaddch(base_y, w, '=');
  int pole_top = base_y - max_d - 1;
  for (p = 0; p < 3; p++) {
    for (y = pole_top; y < base_y; y++) {
      mvaddch(y, peg_x[p], '|');
    }
  }
  mvprintw(base_y + 1, peg_x[0] - 1, "1");
  mvprintw(base_y + 1, peg_x[1] - 1, "2");
  mvprintw(base_y + 1, peg_x[2] - 1, "3");
  for (p = 0; p < 3; p++) {
    for (d = 0; d <= tops[p]; d++) {
      int sz = pegs[p][d];
      int wid = sz * 2 + 1;
      left = peg_x[p] - sz;
      y = base_y - 1 - d;
      color = ((sz - 1) % 7) + 1;
      if (has_colors()) attron(COLOR_PAIR(color));
      for (w = 0; w < wid; w++) {
        mvaddch(y, left + w, '#');
      }
      if (has_colors()) attroff(COLOR_PAIR(color));
    }
  }
  mvprintw(LINES - 3, 2, "Moves: %d (min %d)   1/2/3:move  a:auto  u:undo  r:reset  q/ESC:quit",
           move_count, (1 << num_disks) - 1);
  if (pending_from >= 0) {
    mvprintw(LINES - 2, 2, "Pending from peg %d - pick dest or same to cancel", pending_from + 1);
  }
  if (is_solved()) {
    int cy = LINES / 2 - 3;
    const char *msg = "SOLVED!";
    attron(A_BOLD);
    mvprintw(cy, (COLS - (int)strlen(msg)) / 2, "%s", msg);
    char buf[80];
    snprintf(buf, sizeof(buf), "Elapsed: %.3fs   Moves: %d", last_elapsed, move_count);
    mvprintw(cy + 1, (COLS - (int)strlen(buf)) / 2, "%s", buf);
    attroff(A_BOLD);
  }
  refresh();
}

void hanoi(int n, int from, int to, int aux) {
  if (abort_solve || n < 1) return;
  if (n == 1) {
    apply_move(from, to);
    draw();
    if (num_disks <= 6) napms(180);
    else if (num_disks <= 9) napms(40);
    else if (num_disks <= 11) napms(5);
    int ch = getch();
    if (ch == 'q' || ch == 27) abort_solve = 1;
    return;
  }
  hanoi(n - 1, from, aux, to);
  if (abort_solve) return;
  apply_move(from, to);
  draw();
  if (num_disks <= 6) napms(180);
  else if (num_disks <= 9) napms(40);
  else if (num_disks <= 11) napms(5);
  int ch = getch();
  if (ch == 'q' || ch == 27) abort_solve = 1;
  hanoi(n - 1, aux, to, from);
}

void do_auto(void) {
  int src = -1, p;
  for (p = 0; p < 3; p++) {
    if (tops[p] == num_disks - 1) {
      src = p;
      break;
    }
  }
  if (src < 0) {
    mvprintw(LINES - 4, 2, "Auto only works from full tower on a peg - press 'r'");
    refresh();
    napms(1400);
    return;
  }
  if (src == 2) return;
  int aux = 3 - src - 2;
  pending_from = -1;
  abort_solve = 0;
  double t0 = now_sec();
  hanoi(num_disks, src, 2, aux);
  if (!abort_solve) {
    last_elapsed = now_sec() - t0;
  }
  draw();
}

int main(int argc, char *argv[]) {
  num_disks = 5;
  if (argc > 1) {
    num_disks = atoi(argv[1]);
    if (num_disks < 1 || num_disks > MAX_DISKS) {
      fprintf(stderr, "disks must be 1..%d\n", MAX_DISKS);
      return 1;
    }
  }
  init_state();
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
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
  double launch = now_sec();
  draw();
  for (;;) {
    int ch = getch();
    if (ch == ERR) {
      napms(25);
      continue;
    }
    int need = 0;
    if (ch == 'q' || ch == 27) {
      break;
    } else if (ch == 'r') {
      init_state();
      need = 1;
    } else if (ch == 'u') {
      undo();
      need = 1;
    } else if (ch == 'a') {
      do_auto();
      need = 1;
    } else if (ch >= '1' && ch <= '3') {
      int p = ch - '1';
      if (pending_from < 0) {
        if (tops[p] >= 0) {
          pending_from = p;
          need = 1;
        }
      } else {
        if (p == pending_from) {
          pending_from = -1;
          need = 1;
        } else if (can_move(pending_from, p)) {
          apply_move(pending_from, p);
          pending_from = -1;
          need = 1;
          if (is_solved()) {
            last_elapsed = now_sec() - launch;
          }
        }
      }
    }
    if (need) draw();
  }
  endwin();
  return 0;
}
