#ifndef PROBLEM_MANAGER_H
#define PROBLEM_MANAGER_H

typedef enum {
    PROBLEM_MANAGER_TYPE_SUDOKU,
    PROBLEM_MANAGER_TYPE_FERTILIZER
} problem_manager_type_t;

void problem_manager_dispatch_solver(problem_manager_type_t type);

#endif
