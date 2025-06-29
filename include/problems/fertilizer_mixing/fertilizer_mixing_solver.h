#ifndef FERTILIZER_MIXING_SOLVER_H
#define FERTILIZER_MIXING_SOLVER_H

#include <stdbool.h>
#include <stdlib.h>

bool validate_fertilizer_mixing_data(const char *data, char **error_msg);
int solve_fertilizer_mixing(const char *data, char **error_msg);

#endif