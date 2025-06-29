#include <stdio.h>
#include <stdlib.h>
#include "problem_manager/problem_manager.h"
#include "restserver/restserver.h"

int main(void) {
    #ifdef DEBUG
        printf("[DEBUG] Starting in debug mode\n");
    #endif
    
    start_webserver();

    return 0;
}
