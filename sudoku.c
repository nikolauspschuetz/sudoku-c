#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFSIZE 80
#define MAX_LINE_LENGTH 1000

char * ENV_VAR = "PUZZLE_FILE";

int board[9][9];

bool taken[9] = {
   false, false, false,
   false, false, false,
   false, false, false
};

int options = 0, solution = 0, depth = 0, max_depth=1000;

bool solve();

bool backtrack();

void print_board();

bool board_is_valid();

char * get_puzzle_file() {
   if(!getenv(ENV_VAR)) return "puzzles/ppg-20221205.txt";
   else return getenv(ENV_VAR);
}

bool skip_line(char * line) {
   return (!sizeof(line)) || line[0] == '\n' || line[0] == '#';
}

void read_board() {
   FILE *textfile;
   char *puzzle_file;
   char line[MAX_LINE_LENGTH];
   puzzle_file = get_puzzle_file();
   printf("%s: %s\n", ENV_VAR, puzzle_file);
   textfile = fopen(puzzle_file, "r");
   if (textfile == NULL) exit(1);
   int r = 0; 
   while(fgets(line, MAX_LINE_LENGTH, textfile)){
      int c = 0;
      if (!skip_line(line)) {
         // printf("line: %s", line);
         for (int i = 0; i < sizeof(line); i++) {
            if (c < 9 && line[i] >= '0' && line[i] <= '9') {
               int n;
               switch (line[i]) {
                  case '0': n = 0; break;
                  case '1': n = 1; break;
                  case '2': n = 2; break;
                  case '3': n = 3; break;
                  case '4': n = 4; break;
                  case '5': n = 5; break;
                  case '6': n = 6; break;
                  case '7': n = 7; break;
                  case '8': n = 8; break;
                  case '9': n = 9; break;
               }
               if (i > 0 && line[i-1] == '-') n *= -1;
               if (n < 0) n *= 0;
               board[r][c] = n;
               c++;
            }
         }
         r++;
      }
   }
   fclose(textfile);

   if (!board_is_valid()) {
      print_board();
      printf("board is not valid\n");
      exit(1);
   }

}

void print_board() {
   for (int r = 0; r < 9; r++) {
      for (int c = 0; c < 9; c++) {
         if (c < 8) printf("%i ", (board[r][c]));
         else printf("%i", (board[r][c]));
      }
      printf("\n");
   }
}

bool group_is_valid(int a[9]) {
   int g[9] = {
      0, 0, 0,
      0, 0, 0,
      0, 0, 0
   };
   for (int i = 0; i < 9; i++) {
      if (a[i] > 0) g[a[i]-1]++;
   }
   for (int i = 0; i < 9; i++) {
      if (g[i] > 1) {
         // printf("found %i of %i in group: ", g[i], i+1);
         // for (int j = 0; j < 9; j++) printf("%i ", a[j]);
         // for (int j = 0; j < 9; j++) printf("%i ", g[j]);
         // printf("\n");
         // print_board();
         return false;
      }
   }
   return true;
}

void reset_taken() {
   for (int i = 0; i < 9; i++) taken[i] = false;
}

void set_taken(int r, int c, bool reset) {
   if (reset) reset_taken();
   for (int i = 0; i < 9; i++) {
      if (board[r][i] > 0) {
         taken[board[r][i]-1] = true;
      }
      if (board[i][c] > 0) {
         taken[board[i][c]-1] = true;
      }
   }
   int rb = r / 3, cb = c / 3;
   for (int i = rb * 3; i < (rb * 3) + 3; i++) {
      for (int j = cb * 3; j < (cb * 3) + 3; j++) {
         if (board[i][j] > 0) {
            taken[board[i][j]-1] = true;
         }
      }
   }
}

bool board_is_valid() {
   int col[9];
   int row[9];
   int box[9];
   for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
         row[j] = board[i][j];
         col[j] = board[j][i];
      }
      if (!group_is_valid(row)){
         // printf("row %i is not valid: ", i);
         // for (int i = 0; i < 9; i++) printf("%i ", row[i]);
         // printf("\n");
         return false;
      }
      if (!group_is_valid(col)){
         // printf("col %i is not valid: ", i);
         // for (int i = 0; i < 9; i++) printf("%i ", col[i]);
         // printf("\n");
         return false;
      }
   }
   for (int rb = 0; rb < 3; rb++) {
      for (int cb = 0; cb < 3; cb++) {
         int b = 0;
         for (int r = rb * 3; r < (rb * 3) + 3; r++) {
            for (int c = cb * 3; c < (cb * 3) + 3; c++) {
               box[b] = board[r][c];
               b++;               
            }
         }
         if (!group_is_valid(box)) {
            // printf("box %i, %i is not valid: ", rb, cb);
            // for (int i = 0; i < 9; i++) printf("%i ", box[i]);
            // printf("\n");            
            return false;
         }
      }
   }
   return true;
}

bool done() {
   for (int r = 0; r < 9; r++) {
      for (int c = 0; c < 9; c++) {
         if (board[r][c] < 1) return false;
      }
   }
   return true;
}

void set_solutions() {
   options = 0, solution = 0;
   for (int i = 0; i < 9; i++) {
      if (!taken[i]) {
         options++;
         solution = i+1;
      }
   }
}

int solve_cell(int r, int c) {
   set_taken(r, c, true);
   set_solutions();
   if (options == 1) return solution;
   else return 0;
}

bool solve_one() {
   for (int r = 0; r < 9; r++) {
      for (int c = 0; c < 9; c++) {
         if (board[r][c] < 1) {
            int s = solve_cell(r, c);
            if (s > 0) {
               board[r][c] = s;
               // printf("found solution %i to %i, %i\n", s, r, c);
               return true;
            }
         }
      }
   }
   return false;
}

void reset(int backup[9][9]) {
   // printf("resetting board at depth %i\n", depth);
   // print_board();
   for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) board[i][j] = backup[i][j];
   depth--;
   // printf("reset board to depth %i\n", depth);
   // print_board();
   
}

bool backtrack() {
   int backup[9][9];
   // printf("backtracking at depth %i\n", depth);
   if (depth > max_depth) exit(1);
   // print_board();
   // make backup
   for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
         backup[i][j] = board[i][j];
      }
   }
   struct Smallest {
      int r;
      int c;
      int a;
   } smallest = {-1, -1, 9};
   int a, r, c;
   for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) if (board[i][j] < 1) {
      set_taken(i, j, true);
      a = 0;
      for (int t = 0; t < 9; t++) if (!taken[t]) a++;
      if (a == 0) {
         // printf("all %i, %i options are taken at depth %i\n", i, j, depth);
         return false;
      }
      if (a < smallest.a) {
         smallest.r = i; smallest.c = j, smallest.a = a;
      }
   }
   a = smallest.a, r = smallest.r, c = smallest.c;
   set_taken(r, c, true);
   bool tk[9];
   for (int i = 0; i < 9; i++) tk[i] = taken[i];
   // printf("broke from search for unsolved space %i, %i with %i options at depth %i\n", r, c, a, depth);
   for (int t = 0; t < 9; t++) if (!tk[t]) {
      int n = t+1;
      // printf("backtracking %i, %i on n=%i\n", r, c, n);
      board[r][c] = n;
      // print_board();
      depth++;
      if (!solve()) {
         // printf("backtracking %i, %i on %i at depth %i failed, resetting\n", r, c, n, depth);
         reset(backup);
      } else {
         depth--;
         printf("backtracking %i, %i on %i from depth %i converged\n", r, c, n, depth);
         return true;
      }
   }
   return false;
}

bool solve() {
   while ((!done()) && board_is_valid()) if (!solve_one()) if (!backtrack()) return false;
   return done() && board_is_valid();
}

int main() {
   read_board();
   print_board();
   int out;
   if (solve()) {
      printf("solved board\n");
      out = 0;
   } else {
      printf("failed to solve board\n");
      out = 1;
   }
   print_board();
   return out;
}