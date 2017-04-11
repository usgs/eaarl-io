#include "eaarlio/memory_support.h"
#include "greatest.h"
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * eaarlio_memory_default
 *******************************************************************************
 */

/* Does it return something populated with non-NULL pointers? */
TEST test_stdlib_pointers()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    ASSERT(mem.malloc);
    ASSERT(mem.free);
    ASSERT(mem.realloc);
    ASSERT(mem.calloc);
    PASS();
}

/* Try malloc + free */
TEST test_stdlib_malloc()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    unsigned char *test = mem.malloc(&mem, 10);
    ASSERT(test);
    mem.free(&mem, test);
    PASS();
}

/* Try calloc + free */
TEST test_stdlib_calloc()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    unsigned char *test = mem.calloc(&mem, 2, 3);
    ASSERT(test);
    ASSERT_EQ_FMT(0x00, test[0], "%02x");
    ASSERT_EQ_FMT(0x00, test[1], "%02x");
    ASSERT_EQ_FMT(0x00, test[2], "%02x");
    ASSERT_EQ_FMT(0x00, test[3], "%02x");
    ASSERT_EQ_FMT(0x00, test[4], "%02x");
    ASSERT_EQ_FMT(0x00, test[5], "%02x");
    mem.free(&mem, test);
    PASS();
}

/* Try malloc + realloc + free */
TEST test_stdlib_realloc()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    unsigned char *test = mem.malloc(&mem, 3);
    ASSERT(test);
    test[0] = 0x04;
    test[1] = 0x55;
    test[2] = 0xA0;
    test = mem.realloc(&mem, test, 5);
    ASSERT_EQ_FMT(0x04, test[0], "%02x");
    ASSERT_EQ_FMT(0x55, test[1], "%02x");
    ASSERT_EQ_FMT(0xA0, test[2], "%02x");
    test = mem.realloc(&mem, test, 2);
    ASSERT_EQ_FMT(0x04, test[0], "%02x");
    ASSERT_EQ_FMT(0x55, test[1], "%02x");
    mem.free(&mem, test);
    PASS();
}

SUITE(suite_stdlib)
{
    RUN_TEST(test_stdlib_pointers);
    RUN_TEST(test_stdlib_malloc);
    RUN_TEST(test_stdlib_calloc);
    RUN_TEST(test_stdlib_realloc);
}

/*******************************************************************************
 * eaarlio_memory_valid
 *
 * int eaarlio_memory_valid(struct eaarlio_memory const *mem);
 *******************************************************************************
 */

TEST test_valid_sanity()
{
    eaarlio_memory_valid(NULL);
    PASS();
}

TEST test_valid_null_mem()
{
    ASSERT_EQ_FMT(0, eaarlio_memory_valid(NULL), "%d");
    PASS();
}

TEST test_valid_stdlib()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    ASSERT_EQ_FMT(1, eaarlio_memory_valid(&mem), "%d");
    PASS();
}

TEST test_valid_null_malloc()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    mem.malloc = NULL;
    ASSERT_EQ_FMT(0, eaarlio_memory_valid(&mem), "%d");
    PASS();
}

TEST test_valid_null_free()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    mem.free = NULL;
    ASSERT_EQ_FMT(0, eaarlio_memory_valid(&mem), "%d");
    PASS();
}

TEST test_valid_null_realloc()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    mem.realloc = NULL;
    ASSERT_EQ_FMT(0, eaarlio_memory_valid(&mem), "%d");
    PASS();
}

TEST test_valid_null_calloc()
{
    struct eaarlio_memory mem = eaarlio_memory_default;
    mem.calloc = NULL;
    ASSERT_EQ_FMT(0, eaarlio_memory_valid(&mem), "%d");
    PASS();
}

SUITE(suite_valid)
{
    RUN_TEST(test_valid_sanity);
    RUN_TEST(test_valid_null_mem);
    RUN_TEST(test_valid_stdlib);
    RUN_TEST(test_valid_null_malloc);
    RUN_TEST(test_valid_null_free);
    RUN_TEST(test_valid_null_realloc);
    RUN_TEST(test_valid_null_calloc);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_stdlib);
    RUN_SUITE(suite_valid);

    GREATEST_MAIN_END();
}
