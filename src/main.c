#include <stdio.h>
#include <stdlib.h>
#include "problems/sudoku/sudoku_solver.h"

int main() { 
    #ifdef DEBUG
        printf("Debug mode\n");
    #endif
    manage_sudoku_problem();
    return 0;
}
