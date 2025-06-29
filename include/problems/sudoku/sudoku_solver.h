#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <scip/scip.h>
#include <stdbool.h>

bool validate_sudoku_data(const char *data, char **error_msg);
int solve_sudoku(const char *data, char **error_msg);
SCIP_RETCODE manage_sudoku_problem();
SCIP_RETCODE init_model();
SCIP_RETCODE add_variables();
SCIP_RETCODE create_constraints();
SCIP_RETCODE fix_variables();
SCIP_RETCODE solve();
void print_solution();
SCIP_RETCODE free_model();

#endif