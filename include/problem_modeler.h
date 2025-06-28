#ifndef PROBLEM_MODELER_H
#define PROBLEM_MODELER_H
#include <scip/scip.h>

// SCIP instance
extern SCIP* scip;

// Puzzle
extern int puzzle[9][9];

// Create variables
extern SCIP_VAR* vars[9][9][9];

// Create constraints
extern SCIP_CONS* row_constrs[9][9]; 
extern SCIP_CONS* col_constrs[9][9]; 
extern SCIP_CONS* subgrid_constrs[9][3][3];
extern SCIP_CONS* fillgrid_constrs[9][9];  

// Fix variables for the given puzzle values
extern SCIP_Bool infeasible;
extern SCIP_Bool fixed;


SCIP_RETCODE init_model();
void create_variables();
SCIP_RETCODE add_variables();
SCIP_RETCODE create_constraints();
SCIP_RETCODE fix_variables();
SCIP_RETCODE solve();
void print_solution();
SCIP_RETCODE free_model();

void create_puzzle();
void print_puzzle();

#endif  