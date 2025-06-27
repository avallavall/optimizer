#include <stdio.h>

int main() {
    #ifdef DEBUG
        printf("Debug mode\n");
    #endif
    printf("Hello, World!\n");
    return 0;
}
