#ifndef EAARLIO_FLIGHT_H
#define EAARLIO_FLIGHT_H

/**
 * @file
 * @brief Interface for working with a flight
 *
 * This header defines an interface for working with a flight, which in this
 * context means an EDB file and its associated TLD files.
 */

#include "eaarlio/edb.h"
#include "eaarlio/error.h"
#include "eaarlio/memory.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld_opener.h"

/**
 * Flight data
 *
 * This structure contains the data necessary for working with a flight, which
 * in this context means a set of TLD files and their associated EDB index. The
 * structure contains the EDB data, which calling code can refer to directly.
 * The structure can be used with ::eaarlio_flight_read_raster to retrieve
 * raster data given a raster number.
 *
 * Implementations must do the following:
 * - You must populate ::eaarlio_flight::edb with the data as loaded from an
 *   EDB file.
 * - You must populate ::eaarlio_flight::tld_opener with a TLD opener that
 *   will open streams for the TLD files defined in the EDB data.
 * - You must call ::eaarlio_flight_init to initialize internal state data
 *   prior to calling ::eaarlio_flight_read_raster.
 * - You must call ::eaarlio_flight_free to release internal state data when
 *   finished using the ::eaarlio_flight.
 */
struct eaarlio_flight {
    /**
     * EDB data for the flight
     *
     * @warning After calling ::eaarlio_flight_init, you must not modify
     *      ::eaarlio_edb::files or the ::eaarlio_edb_record::file_index
     *      for any entry in ::eaarlio_edb::records. Doing so will result in
     *      undefined behavior for ::eaarlio_flight_read_raster.
     */
    struct eaarlio_edb edb;

    /**
     * A TLD opener for the flight
     *
     * @warning After calling ::eaarlio_flight_init, you should not modify
     *      ::eaarlio_flight::tld_opener. Doing so will result in undefined
     *      behavior for ::eaarlio_flight_read_raster.
     */
    struct eaarlio_tld_opener tld_opener;

    /**
     * Internal data pointer used for tracking state
     *
     * Calling code should not interact with this directly.
     */
    void *internal;
};

/**
 * Empty ::eaarlio_flight value
 *
 * All numeric fields will contain zero values. All pointers will be null.
 */
#define eaarlio_flight_empty()                                                 \
    (struct eaarlio_flight)                                                    \
    {                                                                          \
        eaarlio_edb_empty(), eaarlio_tld_opener_empty(), NULL                  \
    }

/**
 * Initialize an ::eaarlio_flight
 *
 * @param[in,out] flight A flight to initialize
 * @param[in] memory A memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * @pre @p flight->edb must already be populated with valid data.
 * @pre @p flight->tld_opener must already be populated with a valid TLD
 *      opener.
 *
 * @post On success, @p flight->internal will be populated with an opaque
 *      pointer for internal use.
 * @post If @p flight->internal is not @c NULL, then it represets newly
 *      allocated memory. That memory should later be released with
 *      ::eaarlio_flight_free.
 *
 * @warning @p flight keeps an internal reference to @p memory, which is used
 *      by ::eaarlio_flight_read_raster for allocating memory and by
 *      ::eaarlio_flight_free for releasing memory. If you provide a memory
 *      handler, you must ensure it remains valid until ::eaarlio_flight_free
 *      is called.
 */
eaarlio_error eaarlio_flight_init(struct eaarlio_flight *flight,
    struct eaarlio_memory *memory);

/**
 * Retrieve data for a raster
 *
 * @param[in] flight Flight to use to retrieve the raster
 * @param[out] raster Pointer to raster to be populated
 * @param[out] time_offset Pointer to time offset to be populated. This can be
 *      @c null if you do not want the time_offset.
 * @param[in] raster_number Raster number to retrieve
 * @param[in] include_pulses Should pulse data be read? 1 = yes, 0 = no
 * @param[in] include_waveforms Should waveform data be read? 1 = yes, 0 = no
 *
 * @returns_eaarlio_error
 *
 * @pre ::eaarlio_flight_init must have been called to initialize @p flight.
 * @pre @p flight was not modified after calling ::eaarlio_flight_init.
 *
 * @post On success, @p raster is populated with the data for the raster
 *      requested.
 * @post On success, if @p time_offset was null null, then it is populated with
 *      the time offset detected in the EDB data. The time offset is the
 *      difference in time between the EDB timestamp and the TLD timestamp for
 *      the raster. This needs to be added to ::eaarlio_raster::time_seconds
 *      if you want the correct time.
 * @post On success, any non-null pointers in @p raster represent newly
 *      allocated memory.
 *
 * @warning You must not change the contents of @p flight after calling
 *      ::eaarlio_flight_init or between calls to ::eaarlio_flight_read_raster.
 *      If you do, then the behavior of this function is undefined.
 *
 * @remark Internally, the flight keeps a stream open to the last TLD file
 *      accessed. This permits subsequent calls to ::eaarlio_flight_read_raster
 *      to re-use the same stream.
 *
 * @remark Unlike other functions that access streams, this function is not
 *      guaranteed to operate sequentially. If you attempt to retrieve an
 *      earlier raster number that is within the currently open stream, then
 *      the function will seek backwards in the file to read that raster.
 */
eaarlio_error eaarlio_flight_read_raster(struct eaarlio_flight *flight,
    struct eaarlio_raster *raster,
    int32_t *time_offset,
    uint32_t raster_number,
    int include_pulses,
    int include_waveforms);

/**
 * Release resources held by ::eaarlio_flight
 *
 * @param[in] flight Flight with resources to release
 *
 * @returns_eaarlio_error
 *
 * @remark Unlike similar functions, this function does not accept a memory
 *      handler parameter because it already has an internal reference to a
 *      memory handler.
 */
eaarlio_error eaarlio_flight_free(struct eaarlio_flight *flight);

#endif
