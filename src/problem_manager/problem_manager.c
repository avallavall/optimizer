#include <stdio.h>
#include "problem_manager/problem_manager.h"
#include "problems/sudoku/sudoku_solver.h"

void problem_manager_dispatch_solver(problem_manager_type_t type) {
    SCIP_RETCODE retcode;
    
    switch (type) {
        case PROBLEM_MANAGER_TYPE_SUDOKU:
            retcode = manage_sudoku_problem();
            #ifdef DEBUG
                printf("Sudoku solver returned %d\n", retcode);
            #endif
            if (retcode != EXIT_SUCCESS) {
                fprintf(stderr, "[ERROR] Failed to solve Sudoku: %d\n", retcode);
            }
            break;
        case PROBLEM_MANAGER_TYPE_FERTILIZER:
            fprintf(stderr, "[ERROR] Fertilizer solver not implemented yet\n");
            break;
        default:
            fprintf(stderr, "[ERROR] Unknown problem type: %d\n", type);
            break;
    }
}
