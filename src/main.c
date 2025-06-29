#include <stdio.h>
#include <stdlib.h>
#include "problem_manager/problem_manager.h"

int main(void) {
    #ifdef DEBUG
        printf("[DEBUG] Starting in debug mode\n");
    #endif
    
    problem_manager_dispatch_solver(PROBLEM_MANAGER_TYPE_SUDOKU);
    
    return EXIT_SUCCESS;
}
