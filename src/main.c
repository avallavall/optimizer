#include <stdio.h>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

int main() {
    #ifdef DEBUG
        printf("Debug mode\n");
    #endif

    SCIP* scip = NULL;
    SCIP_CALL(SCIPcreate(&scip));

    SCIP_CALL(SCIPincludeDefaultPlugins(scip));
    SCIP_CALL(SCIPcreateProbBasic(scip, "test"));


    // Create variables
    SCIP_VAR* vars[9][9][9];

    // Add variables to the problem
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            for(int k = 0; k < 9; k++) {
                SCIP_VAR* var = NULL;
                
                // buffer sized for "9-9-9\0"
                char name[6];
                int needed = snprintf(NULL, 0, "%d-%d-%d", i, j, k);
                if (needed >= (int)sizeof(name)) {
                    fprintf(stderr, "Error: name buffer too small for i=%d, j=%d, k=%d\n", i, j, k);
                    return 1;
                }

                snprintf(name, sizeof(name), "%d-%d-%d", i, j, k);
                SCIP_CALL(SCIPcreateVarBasic(scip, &var, name, 0.0, 1.0, 0.0, SCIP_VARTYPE_BINARY));
                SCIP_CALL(SCIPaddVar(scip, var));
                vars[i][j][k] = var;
                printf("Variable %s added\n", name);
            }
        }
    }


    // Add row constraints - each number 1-9 appears exactly once per row
    SCIP_CONS* row_constrs[9][9];  // Store all row constraints
    
    for(int i = 0; i < 9; i++) {
        for(int k = 0; k < 9; k++) {
            SCIP_CONS* cons = NULL;
            
            // Create constraint name "row_i_k"
            char const_name[16];  // "row_8_8\0" is 7 chars, using 16 for safety
            int needed = snprintf(NULL, 0, "row_%d_%d", i, k);
            if (needed >= (int)sizeof(const_name)) {
                fprintf(stderr, "Error: const_name buffer too small for i=%d, k=%d\n", i, k);
                return 1;
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
    SCIP_CONS* col_constrs[9][9];  // Store all column constraints
    
    for(int j = 0; j < 9; j++) {
        for(int k = 0; k < 9; k++) {
            SCIP_CONS* cons = NULL;
            
            // Create constraint name "col_j_k"
            char const_name[16];  // "col_8_8\0" is 7 chars, using 16 for safety
            int needed = snprintf(NULL, 0, "col_%d_%d", j, k);
            if (needed >= (int)sizeof(const_name)) {
                fprintf(stderr, "Error: const_name buffer too small for j=%d, k=%d\n", j, k);
                return 1;
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

    // Subgrid constraints - each number 1-9 appears exactly once in each 3x3 subgrid
    SCIP_CONS* subgrid_constrs[9][3][3];  // Store all subgrid constraints [k][p][q]
    
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
    SCIP_CONS* fillgrid_constrs[9][9];  // Store all fill grid constraints [i][j]
    
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


    // Fix variables for the given puzzle values
    SCIP_Bool infeasible;
    SCIP_Bool fixed;
    
    // Example puzzle (0 means empty cell)
    int puzzle[9][9] = {
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

    // Fix variables for the given values in the puzzle
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(puzzle[i][j] > 0) {
                SCIP_CALL(SCIPfixVar(scip, vars[i][j][puzzle[i][j] - 1], 1.0, &infeasible, &fixed));
                if(infeasible) {
                    fprintf(stderr, "Error: Infeasible puzzle at position (%d,%d)\n", i, j);
                    return 1;
                }
            }
        }
    }

    // Set objective sense (not really needed for feasibility problem)
    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));

    // Turn off verbose output
    SCIP_CALL(SCIPsetIntParam(scip, "display/verblevel", 1));
    
    // Solve the problem
    SCIP_CALL(SCIPsolve(scip));

    // Check solution status
    SCIP_STATUS soln_status = SCIPgetStatus(scip);
    
    if(soln_status == SCIP_STATUS_OPTIMAL) {  // Solution found
        SCIP_SOL* sol = SCIPgetBestSol(scip);
        
        // Extract solution
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                for(int k = 0; k < 9; k++) {
                    if(SCIPgetSolVal(scip, sol, vars[i][j][k]) > 0.5) {
                        puzzle[i][j] = k + 1;
                        break;
                    }
                }
            }
        }
        
        // Print solution
        printf("Solved puzzle:\n");
        for(int i = 0; i < 9; i++) {
            if(i % 3 == 0 && i > 0) printf("------+-------+------\n");
            for(int j = 0; j < 9; j++) {
                if(j % 3 == 0 && j > 0) printf("| ");
                printf("%d ", puzzle[i][j]);
            }
            printf("\n");
        }
    } else if(soln_status == SCIP_STATUS_INFEASIBLE) {
        printf("The puzzle is infeasible.\n");
    } else {
        printf("Solver stopped with status %d\n", soln_status);
    }

    // Free variables
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            for(int k = 0; k < 9; k++) {
                SCIP_CALL(SCIPreleaseVar(scip, &vars[i][j][k]));
            }
        }
    }

    // Free row constraints
    for(int i = 0; i < 9; i++) {
        for(int k = 0; k < 9; k++) {
            SCIP_CALL(SCIPreleaseCons(scip, &row_constrs[i][k]));
        }
    }
    
    // Free column constraints
    for(int j = 0; j < 9; j++) {
        for(int k = 0; k < 9; k++) {
            SCIP_CALL(SCIPreleaseCons(scip, &col_constrs[j][k]));
        }
    }
    
    // Free subgrid constraints
    for(int k = 0; k < 9; k++) {
        for(int p = 0; p < 3; p++) {
            for(int q = 0; q < 3; q++) {
                SCIP_CALL(SCIPreleaseCons(scip, &subgrid_constrs[k][p][q]));
            }
        }
    }
    
    // Free fill grid constraints
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            SCIP_CALL(SCIPreleaseCons(scip, &fillgrid_constrs[i][j]));
        }
    }

    SCIP_CALL(SCIPfree(&scip));

    return 0;
}
