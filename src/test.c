/**
 * @file test.c
 * @brief Test cases module
 * @author Yashas Nagaraj Udupa 
 */

#include "test.h"

void test_rotate_stepper_motor() {
    // Test cases for direction
    static const char directions[] = {-1, 0, 1, 2}; // Including valid and invalid values
    // Test cases for steptype
    static const char *steptypes[] = {"01", "02", "04", "08", "16", "32", "36", NULL}; // Various step types
    // Test cases for angle and RPM
    static const int angles[] = {INT_MIN, -1000, 0, 1000, INT_MAX}; 
    static const int rpms[] = {INT_MIN, -500, 0, 500, INT_MAX};

    printf("Starting tests for rotate_stepper_motor...\n");

    char ptr_e[10] = "PTR_E"; // Static buffer for expected values
    char ptr_a[10] = "PTR_A"; // Static buffer for actual values
    for (uint16_t test_no = 0; directions[test_no] != '\0'; test_no++) {
        for (int j = 0; steptypes[j] != NULL; j++) {
            for (int k = 0; k < sizeof(angles) / sizeof(angles[0]); k++) {
                for (int l = 0; l < sizeof(rpms) / sizeof(rpms[0]); l++) {
                    printf("Test Case : %u \t Direction=%d, Steptype=%s, Angle=%d, RPM=%d\n", 
                           test_no, directions[test_no % sizeof(directions)], steptypes[j], angles[k], rpms[l]);
                    int result = rotate_stepper_motor(directions[test_no % sizeof(directions)], (char*)steptypes[j], ptr_e, ptr_a, angles[k], rpms[l]);
                    printf("Test status: %d\n", result);
                }
            }
        }
    }

    printf("Tests completed.\n");
}