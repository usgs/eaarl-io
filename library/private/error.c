#include "eaarlio/error.h"
#include <stdarg.h>

char const *eaarlio_error_name(eaarlio_error err)
{
    switch(err) {

/* Using a utility macro. This prevents the possibility of making a
             * typo in the string that does not match the symbol name. */
#define CASE(ERR)                                                              \
    case ERR:                                                                  \
        return #ERR

        /* -- General use */
        CASE(EAARLIO_SUCCESS);
        CASE(EAARLIO_NULL);
        CASE(EAARLIO_BUFFER_SHORT);
        CASE(EAARLIO_VALUE_OUT_OF_RANGE);
        CASE(EAARLIO_CORRUPT);
        CASE(EAARLIO_STRING_UNTERMINATED);

        /* -- Specific to flight */
        CASE(EAARLIO_FLIGHT_INVALID);
        CASE(EAARLIO_FLIGHT_RASTER_INVALID);

        /* -- Specific to edb */
        CASE(EAARLIO_EDB_FILENAME_TOO_LONG);

        /* -- Specific to tld */
        CASE(EAARLIO_TLD_TYPE_UNKNOWN);
        CASE(EAARLIO_TLD_OPENER_INVALID);

        /* -- Specific to memory */
        CASE(EAARLIO_MEMORY_INVALID);
        CASE(EAARLIO_MEMORY_ALLOC_FAIL);

        /* -- Specific to stream */
        CASE(EAARLIO_STREAM_INVALID);
        CASE(EAARLIO_STREAM_NOT_IMPL);
        CASE(EAARLIO_STREAM_OPEN_ERROR);
        CASE(EAARLIO_STREAM_CLOSE_ERROR);
        CASE(EAARLIO_STREAM_READ_ERROR);
        CASE(EAARLIO_STREAM_READ_SHORT);
        CASE(EAARLIO_STREAM_WRITE_ERROR);
        CASE(EAARLIO_STREAM_WRITE_SHORT);
        CASE(EAARLIO_STREAM_SEEK_ERROR);
        CASE(EAARLIO_STREAM_SEEK_INVALID);
        CASE(EAARLIO_STREAM_TELL_ERROR);

#undef CASE

        /* -- Uh oh!
         * This should never be reached. If it is reached, it either means a new
         * error code was added to the enum that wasn't added here, or the
         * caller passed an invalid integer that isn't in the enum.
         */

        case EAARLIO_ERROR_UNKNOWN:
        default:
            return "(unknown)";
    }
}

char const *eaarlio_error_message(eaarlio_error err)
{
    switch(err) {

        /* -- General use */

        case EAARLIO_SUCCESS:
            return "Success (no error was encountered)";

        case EAARLIO_NULL:
            return "An unexpected null pointer was encountered";

        case EAARLIO_BUFFER_SHORT:
            return "A buffer's allocated size was too small";

        case EAARLIO_VALUE_OUT_OF_RANGE:
            return "A value was encountered that was out of range for its type";

        case EAARLIO_CORRUPT:
            return "Unable to complete an operation due to suspected corrupted "
                   "data";

        case EAARLIO_STRING_UNTERMINATED:
            return "Encountered a string that appeared unterminated";

        /* -- Specific to flight */

        case EAARLIO_FLIGHT_INVALID:
            return "Invalid flight configuration";

        case EAARLIO_FLIGHT_RASTER_INVALID:
            return "An invalid raster number was specified";

        /* -- Specific to edb */

        case EAARLIO_EDB_FILENAME_TOO_LONG:
            return "Encountered a filename whose length exceeds what EDB "
                   "supports";

        /* -- Specific to tld */

        case EAARLIO_TLD_TYPE_UNKNOWN:
            return "Encountered an unknown TLD record type";

        case EAARLIO_TLD_OPENER_INVALID:
            return "Invalid TLD opener";

        /* -- Specific to memory */

        case EAARLIO_MEMORY_INVALID:
            return "Invalid memory handler";

        case EAARLIO_MEMORY_ALLOC_FAIL:
            return "Unable to allocate memory";

        /* -- Specific to stream */

        case EAARLIO_STREAM_INVALID:
            return "Invalid stream configuration";

        case EAARLIO_STREAM_NOT_IMPL:
            return "Attempted to use a stream operation that was not "
                   "implemented";

        case EAARLIO_STREAM_OPEN_ERROR:
            return "Unable to open file";

        case EAARLIO_STREAM_CLOSE_ERROR:
            return "Unable to close file";

        case EAARLIO_STREAM_READ_ERROR:
            return "Attempt to read failed";

        case EAARLIO_STREAM_READ_SHORT:
            return "Fewer bytes read than expected";

        case EAARLIO_STREAM_WRITE_ERROR:
            return "Attempt to write failed";

        case EAARLIO_STREAM_WRITE_SHORT:
            return "Fewer bytes written than expected";

        case EAARLIO_STREAM_SEEK_ERROR:
            return "Unable to seek to position in file";

        case EAARLIO_STREAM_SEEK_INVALID:
            return "Request to seek used invalid whence";

        case EAARLIO_STREAM_TELL_ERROR:
            return "Unable to tell position in file";

        /* -- Uh oh!
         * This should never be reached. If it is reached, it either means a new
         * error code was added to the enum that wasn't added here, or the
         * caller passed an invalid integer that isn't in the enum.
         */

        case EAARLIO_ERROR_UNKNOWN:
        default:
            return "Unknown error";
    }
}

int eaarlio_error_check(eaarlio_error err, char *format, ...)
{
    int exitcode = 0;
    va_list args;
    va_start(args, format);

    if(err != EAARLIO_SUCCESS) {
        exitcode = 1;
        if(format) {
            vfprintf(stderr, format, args);
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "Error code %s: %s\n", eaarlio_error_name(err),
            eaarlio_error_message(err));
    }

    va_end(args);
    return exitcode;
}
