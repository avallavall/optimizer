# Sudoku Solver with SCIP - Implementation Details

This document explains the structure and functionality of the Sudoku solver implemented in `src/main.c` using the SCIP Optimization Suite.

## Program Flow

### 1. Initialization
- **SCIP Environment Setup**:
  - Creates a SCIP environment using `SCIPcreate()`
  - Includes default SCIP plugins with `SCIPincludeDefaultPlugins()`
  - Creates a new SCIP problem instance with `SCIPcreateProbBasic()`

### 2. Variable Creation
- **Binary Variables**:
  - Creates a 9x9x9 3D array to store binary variables `vars[i][j][k]`
  - Each variable represents whether number `k+1` appears in cell `(i,j)`
  - Variables are named in the format `i-j-k` for debugging purposes
  - All variables are binary (0 or 1) with bounds [0,1]
  - Variables are added to the SCIP problem using `SCIPaddVar()`

### 3. Constraint Definition

#### a) Row Constraints
- Ensures each number 1-9 appears exactly once per row
- For each row `i` and number `k`:
  - Creates a linear constraint: sum(x_ijk for j=0..8) = 1
  - Added using `SCIPcreateConsBasicLinear()` and `SCIPaddCons()`

#### b) Column Constraints
- Ensures each number 1-9 appears exactly once per column
- For each column `j` and number `k`:
  - Creates a linear constraint: sum(x_ijk for i=0..8) = 1
  - Similar structure to row constraints

#### c) Subgrid (3x3) Constraints
- Ensures each number 1-9 appears exactly once in each 3x3 subgrid
- For each subgrid and number `k`:
  - Creates a linear constraint summing over the 9 cells in the subgrid
  - Added using the same pattern as row/column constraints

#### d) Cell Fill Constraints
- Ensures each cell contains exactly one number
- For each cell `(i,j)`:
  - Creates a linear constraint: sum(x_ijk for k=0..8) = 1
  - Added using `SCIPcreateConsBasicLinear()`

### 4. Puzzle Initialization
- Defines a hardcoded Sudoku puzzle as a 9x9 grid
- 0 represents empty cells
- For each non-zero value in the puzzle:
  - Fixes the corresponding variable to 1 using `SCIPfixVar()`
  - This enforces the initial numbers in the Sudoku grid

### 5. Solving the Problem
- Sets the objective sense to minimization (though not strictly needed for feasibility problems)
- Disables SCIP's output for cleaner execution
- Calls `SCIPsolve()` to find a solution
- Checks the solution status with `SCIPgetStatus()`

### 6. Solution Extraction
- If a solution is found:
  - Retrieves the best solution using `SCIPgetBestSol()`
  - For each cell `(i,j)`, finds which number `k` has value 1
  - Prints the solved Sudoku grid in a readable format

### 7. Cleanup
- Releases all constraints using `SCIPreleaseCons()`
- Releases all variables using `SCIPreleaseVar()`
- Frees the SCIP environment with `SCIPfree()`

## Key SCIP Functions Used
- `SCIPcreate()`: Creates a SCIP environment
- `SCIPcreateVarBasic()`: Creates a new variable
- `SCIPcreateConsBasicLinear()`: Creates a linear constraint
- `SCIPfixVar()`: Fixes a variable to a specific value
- `SCIPsolve()`: Solves the optimization problem
- `SCIPgetSolVal()`: Retrieves a variable's value in a solution
- `SCIPreleaseVar()`/`SCIPreleaseCons()`: Releases resources for variables/constraints
- `SCIPfree()`: Frees the SCIP environment

## Error Handling
- Uses `SCIP_CALL()` macro to check return codes from SCIP functions
- Prints error messages and exits if any SCIP operation fails
- Includes buffer size checks for string operations
