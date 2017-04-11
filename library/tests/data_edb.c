#include "eaarlio/edb.h"
#include "eaarlio/error.h"
#include "greatest.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
/* Visual Studio complains about the unsigned const array definitions */
#pragma warning(disable : 4245)
#endif

/* A collection of encoded/decoded EDB data for use in testing.
 */

/* Utility functions for checking structs
 */
inline TEST check_header(struct eaarlio_edb_header *exp,
    struct eaarlio_edb_header *got)
{
    ASSERT(exp);
    ASSERT(got);
    ASSERT_EQ_FMT(exp->files_offset, got->files_offset, "%d");
    ASSERT_EQ_FMT(exp->record_count, got->record_count, "%d");
    ASSERT_EQ_FMT(exp->file_count, got->file_count, "%d");
    PASS();
}

inline TEST check_record(struct eaarlio_edb_record *exp,
    struct eaarlio_edb_record *got)
{
    ASSERT(exp);
    ASSERT(got);
    ASSERT_EQ_FMT(exp->time_seconds, got->time_seconds, "%d");
    ASSERT_EQ_FMT(exp->time_fraction, got->time_fraction, "%d");
    ASSERT_EQ_FMT(exp->record_offset, got->record_offset, "%d");
    ASSERT_EQ_FMT(exp->record_length, got->record_length, "%d");
    ASSERT_EQ_FMT(exp->file_index, got->file_index, "%d");
    ASSERT_EQ_FMT(exp->pulse_count, got->pulse_count, "%d");
    ASSERT_EQ_FMT(exp->digitizer, got->digitizer, "%d");
    PASS();
}

/* Uniqueness - each byte is unique to ensure that everything encodes/decodes
 * as expected. The values may not make much sense and are intended for testing
 * in isolation at a lower level.
 */

/* A header where each byte is unique.
 */
struct eaarlio_edb_header dec_header_unique = {.files_offset = 67305985,
    .record_count = 134678021,
    .file_count = 202050057 };

unsigned char const enc_header_unique[] = {
    /* files_offset */
    '\x01', '\x02', '\x03', '\x04',
    /* record_count */
    '\x05', '\x06', '\x07', '\x08',
    /* file_count */
    '\x09', '\x0A', '\x0B', '\x0C'
};

/* A record where each byte is unique.
 */
struct eaarlio_edb_record dec_record_unique = {.time_seconds = 67305985,
    .time_fraction = 134678021,
    .record_offset = 202050057,
    .record_length = 269422093,
    .file_index = 4625,
    .pulse_count = 19,
    .digitizer = 20 };

unsigned char const enc_record_unique[] = {
    /* time_seconds */
    '\x01', '\x02', '\x03', '\x04',
    /* time_fraction */
    '\x05', '\x06', '\x07', '\x08',
    /* record_offset */
    '\x09', '\x0A', '\x0B', '\x0C',
    /* record_length */
    '\x0D', '\x0E', '\x0F', '\x10',
    /* file_index */
    '\x11', '\x12',
    /* pulse_count */
    '\x13',
    /* digitizer */
    '\x14'
};

/* Plausible data for header + records.
 */

struct eaarlio_edb_header dec_header_sample = {.files_offset = 112,
    .record_count = 5,
    .file_count = 3 };

unsigned char const enc_header_sample[] = {
    /* files_offset */
    '\x70', '\x00', '\x00', '\x00',
    /* record_count */
    '\x05', '\x00', '\x00', '\x00',
    /* file_count */
    '\x03', '\x00', '\x00', '\x00',
};

struct eaarlio_edb_record dec_record_sample1 = {.time_seconds = 1350486255,
    .time_fraction = 291622,
    .record_offset = 54714872,
    .record_length = 123702,
    .file_index = 1,
    .pulse_count = 119,
    .digitizer = 0 };

unsigned char const enc_record_sample1[] = {
    /* time_seconds */
    '\xef', '\xc8', '\x7e', '\x50',
    /* time_fraction */
    '\x26', '\x73', '\x04', '\x00',
    /* record_offset */
    '\xf8', '\xe1', '\x42', '\x03',
    /* record_length */
    '\x36', '\xe3', '\x01', '\x00',
    /* file_index */
    '\x01', '\x00',
    /* pulse_count */
    '\x77',
    /* digitizer */
    '\x00'
};

struct eaarlio_edb_record dec_record_sample2 = {.time_seconds = 1350486255,
    .time_fraction = 322874,
    .record_offset = 54838574,
    .record_length = 123674,
    .file_index = 1,
    .pulse_count = 119,
    .digitizer = 1 };

unsigned char const enc_record_sample2[] = {
    /* time_seconds */
    '\xef', '\xc8', '\x7e', '\x50',
    /* time_fraction */
    '\x3a', '\xed', '\x04', '\x00',
    /* record_offset */
    '\x2e', '\xc5', '\x44', '\x03',
    /* record_length */
    '\x1a', '\xe3', '\x01', '\x00',
    /* file_index */
    '\x01', '\x00',
    /* pulse_count */
    '\x77',
    /* digitizer */
    '\x01'
};

struct eaarlio_edb_record dec_record_sample3 = {.time_seconds = 1350486255,
    .time_fraction = 353522,
    .record_offset = 54962248,
    .record_length = 123706,
    .file_index = 1,
    .pulse_count = 119,
    .digitizer = 0 };

unsigned char const enc_record_sample3[] = {
    /* time_seconds */
    '\xef', '\xc8', '\x7e', '\x50',
    /* time_fraction */
    '\xf2', '\x64', '\x05', '\x00',
    /* record_offset */
    '\x48', '\xa8', '\x46', '\x03',
    /* record_length */
    '\x3a', '\xe3', '\x01', '\x00',
    /* file_index */
    '\x01', '\x00',
    /* pulse_count */
    '\x77',
    /* digitizer */
    '\x00'
};

struct eaarlio_edb_record dec_record_sample4 = {.time_seconds = 1350486255,
    .time_fraction = 385585,
    .record_offset = 55085954,
    .record_length = 123670,
    .file_index = 1,
    .pulse_count = 119,
    .digitizer = 1 };

unsigned char const enc_record_sample4[] = {
    /* time_seconds */
    '\xef', '\xc8', '\x7e', '\x50',
    /* time_fraction */
    '\x31', '\xe2', '\x05', '\x00',
    /* record_offset */
    '\x82', '\x8b', '\x48', '\x03',
    /* record_length */
    '\x16', '\xe3', '\x01', '\x00',
    /* file_index */
    '\x01', '\x00',
    /* pulse_count */
    '\x77',
    /* digitizer */
    '\x01'
};

struct eaarlio_edb_record dec_record_sample5 = {.time_seconds = 1350486255,
    .time_fraction = 415547,
    .record_offset = 55209624,
    .record_length = 123674,
    .file_index = 1,
    .pulse_count = 119,
    .digitizer = 0 };

unsigned char const enc_record_sample5[] = {
    /* time_seconds */
    '\xef', '\xc8', '\x7e', '\x50',
    /* time_fraction */
    '\x3b', '\x57', '\x06', '\x00',
    /* record_offset */
    '\x98', '\x6e', '\x4a', '\x03',
    /* record_length */
    '\x1a', '\xe3', '\x01', '\x00',
    /* file_index */
    '\x01', '\x00',
    /* pulse_count */
    '\x77',
    /* digitizer */
    '\x00'
};

/* Plausible data for file names.
 */

char const dec_fn_len0[] = "";
unsigned char const enc_fn_len0[] = { '\x00', '\x00' };

char const dec_fn_len1[] = "h";
unsigned char const enc_fn_len1[] = { '\x01', '\x00', 'h' };

char const dec_fn_len5[] = "mnopq";
unsigned char const enc_fn_len5[] = { '\x05', '\x00', 'm', 'n', 'o', 'p', 'q' };

char const dec_fn_len7[] = "abcdefg";
unsigned char const enc_fn_len7[] = { '\x07', '\x00', 'a', 'b', 'c', 'd', 'e',
    'f', 'g' };

char const dec_fn_len258[] =
    "abcdefghijklmnopqrstuvwxyz1234ABCDEFGHIJKLMNOPQRSTUVWXYZ678950!@#$%^&*"
    "abcdefghijklmnopqrstuvwxyz1234ABCDEFGHIJKLMNOPQRSTUVWXYZ678950!@#$%^&*"
    "abcdefghijklmnopqrstuvwxyz1234ABCDEFGHIJKLMNOPQRSTUVWXYZ678950!@#$%^&*"
    "abcdefghijklmnopqrstuvwxyz1234ABCDEFGHIJKLMNOPQR";
unsigned char const enc_fn_len258[] = {
    /* Length is 258 */
    '\x02', '\x01',
    /* 70 characters */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '6', '7', '8', '9',
    '5', '0', '!', '@', '#', '$', '%', '^', '&', '*',
    /* 70 characters */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '6', '7', '8', '9',
    '5', '0', '!', '@', '#', '$', '%', '^', '&', '*',
    /* 70 characters */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '6', '7', '8', '9',
    '5', '0', '!', '@', '#', '$', '%', '^', '&', '*',
    /* 48 characters */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R'
};

char const dec_fn_contains_null[] = "ij";
unsigned char const enc_fn_contains_null[] = { '\x05', '\x00', 'i', 'j', '\x00',
    'k', 'l' };

char const dec_fn_tld_a[] = "123456-246802.tld";
unsigned char const enc_fn_tld_a[] = { '\x11', '\x00', '1', '2', '3', '4', '5',
    '6', '-', '2', '4', '6', '8', '0', '2', '.', 't', 'l', 'd' };

char const dec_fn_tld_b[] = "987654-135791.tld";
unsigned char const enc_fn_tld_b[] = { '\x11', '\x00', '9', '8', '7', '6', '5',
    '4', '-', '1', '3', '5', '7', '9', '1', '.', 't', 'l', 'd' };

char const dec_fn_tld_c[] = "284713-845982.tld";
unsigned char const enc_fn_tld_c[] = { '\x11', '\x00', '2', '8', '4', '7', '1',
    '3', '-', '8', '4', '5', '9', '8', '2', '.', 't', 'l', 'd' };
