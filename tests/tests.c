#include <criterion/criterion.h>

Test(sanity, test_assertion) {
    cr_assert(1 == 1, "1 should equal 1");
}
