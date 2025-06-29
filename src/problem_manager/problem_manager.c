#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "problem_manager/problem_manager.h"
#include "problems/sudoku/sudoku_solver.h"
#include "problems/fertilizer_mixing/fertilizer_mixing_solver.h"


solver_result_t problem_manager_dispatch_solver(problem_manager_type_t type, const char *data) {
    solver_result_t result = {0, NULL};
    char *error_msg = NULL;
    
    switch (type) {
        case TYPE_SUDOKU: {
            printf("Dispatching Sudoku problem\n");
            
            int retcode = solve_sudoku(data, &error_msg);
            
            if (retcode == EXIT_SUCCESS) {
                result.status = 0;
                result.message = strdup("Sudoku solved successfully");
            } else {
                result.status = retcode;
                result.message = error_msg ? error_msg : strdup("Failed to solve Sudoku");
            }
            break;
        }
        case TYPE_FERTILIZER_MIXING: {
            printf("Dispatching fertilizer mixing problem\n");
            
            int retcode = solve_fertilizer_mixing(data, &error_msg);
            
            if (retcode == EXIT_SUCCESS) {
                result.status = 0;
                result.message = strdup("Fertilizer mixing problem solved successfully");
            } else {
                result.status = retcode;
                result.message = error_msg ? error_msg : strdup("Failed to solve fertilizer mixing problem");
            }
            break;
        }
        default:
            result.status = -1;
            result.message = strdup("Unknown problem type");
            break;
    }
    
    return result;
}