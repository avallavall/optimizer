#include <criterion/criterion.h>
#include <stdio.h>

Test(a, a) {
    printf("Hello\n");
    cr_assert_eq(1, 1);
}

Test(a, b) {
    printf("Hello\n");
    cr_assert_eq(1, 1);
}

Test(a, c) {
    printf("Hello\n");
    cr_assert_eq(1, 1);
}

Test(a, d) {
    printf("Hello\n");
    cr_assert_eq(1, 1);
}