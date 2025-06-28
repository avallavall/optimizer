#include <criterion/criterion.h>
#include "../include/problem_modeler.h"

Test(sudoku, test_puzzle_creation) {
    int expected[9][9] = {
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
    
    // Initialize puzzle
    create_puzzle();
    
    // Verify puzzle initialization
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cr_assert_eq(puzzle[i][j], expected[i][j], 
                        "Mismatch at [%d][%d]: expected %d, got %d", 
                        i, j, expected[i][j], puzzle[i][j]);
        }
    }
}

Test(sudoku, test_solution) {
    SCIP_RETCODE retcode;
    
    // Initialize puzzle
    create_puzzle();
    
    // Initialize model with error checking
    printf("Initializing model...\n");
    retcode = init_model();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Failed to initialize model: %d\n", retcode);
        cr_assert_fail("Failed to initialize model");
    }
    
    // Solve the puzzle with error checking
    printf("Solving puzzle...\n");
    retcode = solve();
    if (retcode != SCIP_OKAY) {
        fprintf(stderr, "Failed to solve puzzle: %d\n", retcode);
        free_model();
        cr_assert_fail("Failed to solve puzzle");
    }
    
    // Verify the solution is valid
    printf("Verifying solution...\n");
    
    // Simple check: ensure no zeros in the solution
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] < 1 || puzzle[i][j] > 9) {
                fprintf(stderr, "Invalid value at [%d][%d]: %d\n", i, j, puzzle[i][j]);
                free_model();
                cr_assert_fail("Invalid solution found");
            }
        }
    }
    
    // Clean up
    printf("Cleaning up...\n");
    free_model();
    printf("Test completed successfully\n");
}

Test(sudoku, test_invalid_puzzle) {
    // This test would check behavior with an invalid puzzle
    // Currently just a placeholder
    cr_assert(1, "Placeholder for invalid puzzle test");
}
