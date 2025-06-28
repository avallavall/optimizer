#include "problem_modeler.h"
#include <stdio.h>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

// Global variable definitions
SCIP* scip = NULL;
int puzzle[9][9];
SCIP_VAR* vars[9][9][9] = {0};
SCIP_CONS* row_constrs[9][9] = {0};
SCIP_CONS* col_constrs[9][9] = {0};
SCIP_CONS* subgrid_constrs[9][3][3] = {0};
SCIP_CONS* fillgrid_constrs[9][9] = {0};
SCIP_Bool infeasible = FALSE;
SCIP_Bool fixed = FALSE;

void create_puzzle() {
    int initial[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            puzzle[i][j] = initial[i][j];
        }
    }
}

void print_puzzle() {
    for (int i = 0; i < 9; i++) {
        if (i > 0 && i % 3 == 0) {
            printf("------+-------+------\n");
        }
        for (int j = 0; j < 9; j++) {
            if (j > 0 && j % 3 == 0) {
                printf("| ");
            }
            printf("%d ", puzzle[i][j]);
        }
        printf("\n");
    }
}

void print_solution() {
    printf("\nSolution:\n");
    print_puzzle();
}

SCIP_RETCODE init_model() {    
    SCIP_CALL(SCIPcreate(&scip));
    SCIP_CALL(SCIPincludeDefaultPlugins(scip));
    SCIP_CALL(SCIPcreateProbBasic(scip, "test"));
    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));
    SCIP_CALL(SCIPsetIntParam(scip, "display/verblevel", 0));
    
    return SCIP_OKAY;
}

void create_variables() {
    memset(vars, 0, sizeof(vars));
}

SCIP_RETCODE add_variables() {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            for(int k = 0; k < 9; k++) {
                SCIP_VAR* var = NULL;
                
                // buffer sized for "9-9-9\0"
                char name[6];
                int needed = snprintf(NULL, 0, "%d-%d-%d", i, j, k);
                if (needed >= (int)sizeof(name)) {
                    fprintf(stderr, "Error: name buffer too small for i=%d, j=%d, k=%d\n", i, j, k);
                    return SCIP_ERROR;
                }

                snprintf(name, sizeof(name), "%d-%d-%d", i, j, k);
                SCIP_CALL(SCIPcreateVarBasic(scip, &var, name, 0.0, 1.0, 0.0, SCIP_VARTYPE_BINARY));
                SCIP_CALL(SCIPaddVar(scip, var));
                vars[i][j][k] = var;
                #ifdef DEBUG
                    printf("Variable %s added\n", name);
                #endif
            }
        }
    }
    return SCIP_OKAY;
}

SCIP_RETCODE create_constraints() {

    // Add row constraints - each number 1-9 appears exactly once per row
    
    for(int i = 0; i < 9; i++) {
        for(int k = 0; k < 9; k++) {
            SCIP_CONS* cons = NULL;
            
            // Create constraint name "row_i_k"
            char const_name[16];  // "row_8_8\0" is 7 chars, using 16 for safety
            int needed = snprintf(NULL, 0, "row_%d_%d", i, k);
            if (needed >= (int)sizeof(const_name)) {
                fprintf(stderr, "Error: const_name buffer too small for i=%d, k=%d\n", i, k);
                return SCIP_ERROR;
            }
            snprintf(const_name, sizeof(const_name), "row_%d_%d", i, k);
            
            // Create constraint: sum(x_ijk for j=0..8) = 1
            SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, const_name, 0, NULL, NULL, 1.0, 1.0));
            
            // Add all variables in this row for number k+1
            for(int j = 0; j < 9; j++) {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[i][j][k], 1.0));
            }
            
            SCIP_CALL(SCIPaddCons(scip, cons));
            row_constrs[i][k] = cons;  // Store the constraint
        }
    }

    // Add column constraints - each number 1-9 appears exactly once per column
    
    for(int j = 0; j < 9; j++) {
        for(int k = 0; k < 9; k++) {
            SCIP_CONS* cons = NULL;
            
            // Create constraint name "col_j_k"
            char const_name[16];  // "col_8_8\0" is 7 chars, using 16 for safety
            int needed = snprintf(NULL, 0, "col_%d_%d", j, k);
            if (needed >= (int)sizeof(const_name)) {
                fprintf(stderr, "Error: const_name buffer too small for j=%d, k=%d\n", j, k);
                return SCIP_ERROR;
            }
            snprintf(const_name, sizeof(const_name), "col_%d_%d", j, k);
            
            // Create constraint: sum(x_ijk for i=0..8) = 1
            SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, const_name, 0, NULL, NULL, 1.0, 1.0));
            
            // Add all variables in this column for number k+1
            for(int i = 0; i < 9; i++) {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[i][j][k], 1.0));
            }
            
            SCIP_CALL(SCIPaddCons(scip, cons));
            col_constrs[j][k] = cons;  // Store the constraint
        }
    }

    // Add subgrid constraints - each number 1-9 appears exactly once per 3x3 subgrid
    
    for(int k = 0; k < 9; k++) {
        for(int p = 0; p < 3; p++) {
            for(int q = 0; q < 3; q++) {
                SCIP_CONS* cons = NULL;
                
                // Create constraint name "subgrid_k_p_q"
                char const_name[24];  // "subgrid_8_2_2\0" is 13 chars, using 24 for safety
                int needed = snprintf(NULL, 0, "subgrid_%d_%d_%d", k, p, q);
                if(needed >= (int)sizeof(const_name)) {
                    fprintf(stderr, "Error: const_name buffer too small for k=%d, p=%d, q=%d\n", k, p, q);
                    return 1;
                }
                snprintf(const_name, sizeof(const_name), "subgrid_%d_%d_%d", k, p, q);
                
                // Create constraint: sum(x_ijk for i,j in subgrid) = 1
                SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, const_name, 0, NULL, NULL, 1.0, 1.0));
                
                // Add variables in the current 3x3 subgrid for number k+1
                for(int j = 3 * p; j < 3 * (p + 1); j++) {
                    for(int i = 3 * q; i < 3 * (q + 1); i++) {
                        SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[i][j][k], 1.0));
                    }
                }
                
                SCIP_CALL(SCIPaddCons(scip, cons));
                subgrid_constrs[k][p][q] = cons;  // Store the constraint
            }
        }
    }

    // Ensure that the complete puzzle grid is filled with one number per cell
    
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            SCIP_CONS* cons = NULL;
            
            // Create constraint name "fillgrid_i_j"
            char const_name[24];  // "fillgrid_8_8\0" is 13 chars, using 24 for safety
            int needed = snprintf(NULL, 0, "fillgrid_%d_%d", i, j);
            if(needed >= (int)sizeof(const_name)) {
                fprintf(stderr, "Error: const_name buffer too small for i=%d, j=%d\n", i, j);
                return 1;
            }
            snprintf(const_name, sizeof(const_name), "fillgrid_%d_%d", i, j);
            
            // Create constraint: sum(x_ijk for k=0..8) = 1
            SCIP_CALL(SCIPcreateConsBasicLinear(scip, &cons, const_name, 0, NULL, NULL, 1.0, 1.0));
            
            // Add all numbers 1-9 for this cell
            for(int k = 0; k < 9; k++) {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, vars[i][j][k], 1.0));
            }
            
            SCIP_CALL(SCIPaddCons(scip, cons));
            fillgrid_constrs[i][j] = cons;  // Store the constraint
        }
    }
    
    return SCIP_OKAY;
}

SCIP_RETCODE fix_variables() {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(puzzle[i][j] > 0) {
                SCIP_CALL(SCIPfixVar(scip, vars[i][j][puzzle[i][j] - 1], 1.0, &infeasible, &fixed));
                if(infeasible) {
                    fprintf(stderr, "Error: Infeasible puzzle at position (%d,%d)\n", i, j);
                    return SCIP_ERROR;
                }
            }
        }
    }
    return SCIP_OKAY;
}

SCIP_RETCODE solve() {
    // Solve the problem
    SCIP_RETCODE retcode = SCIPsolve(scip);
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error in SCIPsolve: %d\n", retcode);
        return retcode;
    }

    // Check solution status
    SCIP_STATUS soln_status = SCIPgetStatus(scip);
    
    if(soln_status == SCIP_STATUS_OPTIMAL) {  // Solution found
        SCIP_SOL* sol = SCIPgetBestSol(scip);
        if (sol == NULL) {
            fprintf(stderr, "Error: No solution found despite optimal status\n");
            return SCIP_ERROR;
        }
        
        // Extract solution
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                for(int k = 0; k < 9; k++) {
                    SCIP_Real val = SCIPgetSolVal(scip, sol, vars[i][j][k]);
                    if(val > 0.5) {
                        puzzle[i][j] = k + 1;
                        break;
                    }
                }
            }
        }
        return SCIP_OKAY;
    } else if(soln_status == SCIP_STATUS_INFEASIBLE) {
        printf("The puzzle is infeasible.\n");
        return SCIP_OKAY;  // Not an error, just no solution exists
    } else {
        printf("Solver stopped with status %d\n", soln_status);
        return SCIP_ERROR;
    }
}

SCIP_RETCODE free_model() {
    SCIP_RETCODE retcode;
    
    // Free variables
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            for(int k = 0; k < 9; k++) {
                if (vars[i][j][k] != NULL) {
                    retcode = SCIPreleaseVar(scip, &vars[i][j][k]);
                    if (retcode != SCIP_OKAY) {
                        fprintf(stderr, "Error releasing variable at [%d][%d][%d]\n", i, j, k);
                        return retcode;
                    }
                }
            }
        }
    }

    // Free row constraints
    for(int i = 0; i < 9; i++) {
        for(int k = 0; k < 9; k++) {
            if (row_constrs[i][k] != NULL) {
                retcode = SCIPreleaseCons(scip, &row_constrs[i][k]);
                if (retcode != SCIP_OKAY) {
                    fprintf(stderr, "Error releasing row constraint [%d][%d]\n", i, k);
                    return retcode;
                }
            }
        }
    }
    
    // Free column constraints
    for(int j = 0; j < 9; j++) {
        for(int k = 0; k < 9; k++) {
            if (col_constrs[j][k] != NULL) {
                retcode = SCIPreleaseCons(scip, &col_constrs[j][k]);
                if (retcode != SCIP_OKAY) {
                    fprintf(stderr, "Error releasing column constraint [%d][%d]\n", j, k);
                    return retcode;
                }
            }
        }
    }
    
    // Free subgrid constraints
    for(int k = 0; k < 9; k++) {
        for(int p = 0; p < 3; p++) {
            for(int q = 0; q < 3; q++) {
                if (subgrid_constrs[k][p][q] != NULL) {
                    retcode = SCIPreleaseCons(scip, &subgrid_constrs[k][p][q]);
                    if (retcode != SCIP_OKAY) {
                        fprintf(stderr, "Error releasing subgrid constraint [%d][%d][%d]\n", k, p, q);
                        return retcode;
                    }
                }
            }
        }
    }
    
    // Free fillgrid constraints
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if (fillgrid_constrs[i][j] != NULL) {
                retcode = SCIPreleaseCons(scip, &fillgrid_constrs[i][j]);
                if (retcode != SCIP_OKAY) {
                    fprintf(stderr, "Error releasing fillgrid constraint [%d][%d]\n", i, j);
                    return retcode;
                }
            }
        }
    }
    
    // Free the SCIP instance
    if (scip != NULL) {
        retcode = SCIPfree(&scip);
        if (retcode != SCIP_OKAY) {
            fprintf(stderr, "Error freeing SCIP instance\n");
            return retcode;
        }
    }
    
    return SCIP_OKAY;
}