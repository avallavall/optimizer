#ifndef PROBLEM_MANAGER_H
#define PROBLEM_MANAGER_H

typedef enum {
    TYPE_SUDOKU,
    TYPE_FERTILIZER_MIXING,
    TYPE_INVALID
} problem_manager_type_t;

// Structure to hold solver results
typedef struct {
    int status;  // 0 = success, non-zero = error
    char *result; // JSON string with the result (must be freed by caller)
} solver_result_t;

// Dispatch solver and return results
solver_result_t problem_manager_dispatch_solver(problem_manager_type_t type);

// Free resources used by solver result
void problem_manager_free_result(solver_result_t *result);

#endif
