#include <stdlib.h>

#include "eaarlio/edb.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_memory.h"

/* Helper to initialize an EDB with data
 */
TEST init_edb(char *msg,
    int records,
    int files,
    int files_0,
    int files_1,
    struct eaarlio_memory *memory,
    struct eaarlio_edb *edb)
{
    edb->record_count = 5;
    edb->file_count = 2;

    if(records) {
        edb->records = memory->calloc(memory, 5, sizeof(struct eaarlio_memory));
        ASSERTm(msg, edb->records);
        edb->records[0].time_seconds = 1;
        edb->records[1].time_seconds = 2;
        edb->records[2].time_seconds = 3;
        edb->records[3].time_seconds = 4;
        edb->records[4].time_seconds = 5;
    }
    if(files) {
        edb->files = memory->calloc(memory, 2, sizeof(char *));
        ASSERTm(msg, edb->files);
        edb->files[0] = edb->files[1] = NULL;

        if(files_0) {
            edb->files[0] = memory->calloc(memory, 10, sizeof(char));
            ASSERTm(msg, edb->files[0]);
            strcpy(edb->files[0], "file0.tld");
        }
        if(files_1) {
            edb->files[1] = memory->calloc(memory, 10, sizeof(char));
            ASSERTm(msg, edb->files[1]);
            strcpy(edb->files[1], "file1.tld");
        }
    }

    PASS();
}

/* Checks an EDB to ensure it matches its expected initialization
 */
TEST check_edb(char *msg,
    int records,
    int files,
    int files_0,
    int files_1,
    struct eaarlio_edb *edb)
{
    ASSERTm(msg, edb);
    ASSERT_EQ_FMTm(msg, 5, edb->record_count, "%d");
    ASSERT_EQ_FMTm(msg, 2, edb->file_count, "%d");

    if(records) {
        ASSERTm(msg, edb->records);
        ASSERT_EQ_FMTm(msg, 1, edb->records[0].time_seconds, "%d");
        ASSERT_EQ_FMTm(msg, 2, edb->records[1].time_seconds, "%d");
        ASSERT_EQ_FMTm(msg, 3, edb->records[2].time_seconds, "%d");
        ASSERT_EQ_FMTm(msg, 4, edb->records[3].time_seconds, "%d");
        ASSERT_EQ_FMTm(msg, 5, edb->records[4].time_seconds, "%d");
    } else {
        ASSERT_FALSEm(msg, edb->records);
    }

    if(files) {
        ASSERTm(msg, edb->files);

        if(files_0) {
            ASSERTm(msg, edb->files[0]);
            ASSERT_STRN_EQm(msg, "file0.tld", edb->files[0], 10);
        } else {
            ASSERT_FALSEm(msg, edb->files[0]);
        }

        if(files_1) {
            ASSERTm(msg, edb->files[1]);
            ASSERT_STRN_EQm(msg, "file1.tld", edb->files[1], 10);
        } else {
            ASSERT_FALSEm(msg, edb->files[1]);
        }
    } else {
        ASSERT_FALSEm(msg, edb->files);
    }

    PASS();
}

/*******************************************************************************
 * eaarlio_edb_free
 *******************************************************************************
 */

TEST test_free_sanity()
{
    eaarlio_edb_free(NULL, NULL);
    PASS();
}

TEST test_free_null_edb()
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_free(NULL, NULL));
    PASS();
}

TEST test_free_mem_invalid(struct eaarlio_memory *memory,
    struct mock_memory *mock)
{
    struct eaarlio_memory invalid = *memory;
    struct eaarlio_edb edb = eaarlio_edb_empty();

    CHECK_CALL(init_edb("mem_invalid", 1, 1, 1, 1, memory, &edb));

    ASSERT_EQ_FMT(4, mock->ptrs_used, "%d");
    ASSERT_EQ_FMT(4, mock_memory_count_in_use(mock), "%d");

    // Make an invalid memory manager by setting free to null.
    invalid.free = NULL;

    ASSERT_EAARLIO_ERR(
        EAARLIO_MEMORY_INVALID, eaarlio_edb_free(&edb, &invalid));

    // Everything should be as it was initialized
    CHECK_CALL(check_edb("mem_invalid", 1, 1, 1, 1, &edb));

    // No memory should have been allocated or released
    ASSERT_EQ_FMT(4, mock->ptrs_used, "%d");
    ASSERT_EQ_FMT(4, mock_memory_count_in_use(mock), "%d");
    PASS();
}

TEST test_free_alloc(char *msg,
    int records,
    int files,
    int files_0,
    int files_1,
    struct eaarlio_memory *memory,
    struct mock_memory *mock)
{
    struct eaarlio_edb edb = eaarlio_edb_empty();
    int allocs = records + files + files_0 + files_1;

    CHECK_CALL(init_edb(msg, records, files, files_0, files_1, memory, &edb));

    ASSERT_EQ_FMTm(msg, allocs, mock->ptrs_used, "%d");
    ASSERT_EQ_FMTm(msg, allocs, mock_memory_count_in_use(mock), "%d");

    ASSERT_EAARLIO_SUCCESSm(msg, eaarlio_edb_free(&edb, memory));

    ASSERT_FALSEm(msg, edb.records);
    ASSERT_FALSEm(msg, edb.files);
    ASSERT_EQ_FMTm(msg, 5, edb.record_count, "%d");
    ASSERT_EQ_FMTm(msg, 2, edb.file_count, "%d");
    ASSERT_EQ_FMTm(msg, 0, mock_memory_count_in_use(mock), "%d");

    PASS();
}

SUITE(suite_free)
{
    struct mock_memory mock;
    struct eaarlio_memory memory;

    RUN_TEST(test_free_sanity);
    RUN_TEST(test_free_null_edb);

    mock_memory_new(&memory, &mock, 4);
    RUN_TESTp(test_free_mem_invalid, &memory, &mock);

    mock_memory_reset(&mock, 0);
    RUN_TESTp(test_free_alloc, "no alloc", 0, 0, 0, 0, &memory, &mock);

    mock_memory_reset(&mock, 1);
    RUN_TESTp(test_free_alloc, "alloc records", 1, 0, 0, 0, &memory, &mock);

    mock_memory_reset(&mock, 1);
    RUN_TESTp(
        test_free_alloc, "alloc files, neither", 0, 1, 0, 0, &memory, &mock);

    mock_memory_reset(&mock, 2);
    RUN_TESTp(
        test_free_alloc, "alloc files, first", 0, 1, 1, 0, &memory, &mock);

    mock_memory_reset(&mock, 2);
    RUN_TESTp(
        test_free_alloc, "alloc files, second", 0, 1, 0, 1, &memory, &mock);

    mock_memory_reset(&mock, 3);
    RUN_TESTp(test_free_alloc, "alloc files, both", 0, 1, 1, 1, &memory, &mock);

    mock_memory_reset(&mock, 4);
    RUN_TESTp(test_free_alloc, "alloc all", 1, 1, 1, 1, &memory, &mock);

    mock_memory_destroy(&memory);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_free);

    GREATEST_MAIN_END();
}
