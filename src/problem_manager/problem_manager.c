#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "problem_manager/problem_manager.h"
#include "problems/sudoku/sudoku_solver.h"
#include "problems/fertilizer_mixing/fertilizer_mixing_solver.h"
#include "../lib/cJSON/cJSON.h"

static char* create_error_result(const char *message) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "error");
    cJSON_AddStringToObject(root, "message", message);
    char *result = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return result;
}

static char* create_success_result(const char *message, const char *solver_name) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddStringToObject(root, "message", message);
    cJSON_AddStringToObject(root, "solver", solver_name);
    char *result = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return result;
}

solver_result_t problem_manager_dispatch_solver(problem_manager_type_t type) {
    solver_result_t result = {0};
    
    switch (type) {
        case TYPE_SUDOKU: {
            int retcode = manage_sudoku_problem();
            if (retcode == EXIT_SUCCESS) {
                result.status = 0;
                result.result = create_success_result("Sudoku solved successfully", "sudoku");
            } else {
                result.status = retcode;
                result.result = create_error_result("Failed to solve Sudoku");
            }
            break;
        }
        case TYPE_FERTILIZER_MIXING: {
            int retcode = manage_fertilizer_mixing_problem();
            if (retcode == EXIT_SUCCESS) {
                result.status = 0;
                result.result = create_success_result("Fertilizer mixing problem solved", "fertilizer");
            } else {
                result.status = retcode;
                result.result = create_error_result("Failed to solve fertilizer mixing problem");
            }
            break;
        }
        default:
            result.status = -1;
            result.result = create_error_result("Unknown problem type");
            break;
    }
    
    return result;
}

void problem_manager_free_result(solver_result_t *result) {
    if (result && result->result) {
        free(result->result);
        result->result = NULL;
    }
}
