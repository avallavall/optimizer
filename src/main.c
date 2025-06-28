#include <stdio.h>
#include <stdlib.h>
#include "problem_modeler.h"

int main() {
    SCIP_RETCODE retcode;
    
    #ifdef DEBUG
        printf("Debug mode\n");
    #endif

    // Initialize and print the puzzle
    create_puzzle(puzzle);
    printf("Initial puzzle:\n");
    print_puzzle(puzzle);

    // Initialize the model
    retcode = init_model();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error initializing model: %d\n", retcode);
        return EXIT_FAILURE;
    }

    // Create and add variables
    create_variables();
    retcode = add_variables();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error adding variables: %d\n", retcode);
        free_model();
        return EXIT_FAILURE;
    }

    // Create constraints
    retcode = create_constraints();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error creating constraints: %d\n", retcode);
        free_model();
        return EXIT_FAILURE;
    }

    // Fix variables based on initial puzzle
    retcode = fix_variables();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error fixing variables: %d\n", retcode);
        free_model();
        return EXIT_FAILURE;
    }

    // Solve the puzzle
    retcode = solve();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error solving the puzzle: %d\n", retcode);
        free_model();
        return EXIT_FAILURE;
    }

    // Print the solution and clean up
    print_solution();
    
    retcode = free_model();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Error freeing model: %d\n", retcode);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
