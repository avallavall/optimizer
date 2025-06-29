#ifndef PROBLEM_MANAGER_H
#define PROBLEM_MANAGER_H

typedef enum {
    TYPE_SUDOKU,
    TYPE_FERTILIZER_MIXING,
    TYPE_INVALID
} problem_manager_type_t;

typedef struct {
    int status;
    char *message;
} solver_result_t;

solver_result_t problem_manager_dispatch_solver(problem_manager_type_t type, const char *data);

#endif
