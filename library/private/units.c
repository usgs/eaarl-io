#include "eaarlio/units.h"

double eaarlio_units_edb_time(struct eaarlio_edb_record *record)
{
    if(!record)
        return 0.;
    return record->time_seconds
        + record->time_fraction * EAARLIO_UNITS_TIME_FRACTION_SECONDS;
}

double eaarlio_units_raster_time(struct eaarlio_raster *raster)
{
    if(!raster)
        return 0.;
    return raster->time_seconds
        + raster->time_fraction * EAARLIO_UNITS_TIME_FRACTION_SECONDS;
}

double eaarlio_units_pulse_time(struct eaarlio_raster *raster,
    uint16_t pulse_number)
{
    if(!raster)
        return 0.;
    if(!raster->pulse)
        return 0.;
    if(!raster->pulse_count)
        return 0.;
    if(!pulse_number)
        return 0.;
    if(pulse_number > raster->pulse_count)
        return 0.;

    return raster->time_seconds
        + EAARLIO_UNITS_TIME_FRACTION_SECONDS
        * (raster->time_fraction + raster->pulse[pulse_number - 1].time_offset);
}

double eaarlio_units_pulse_scan_angle(struct eaarlio_pulse *pulse)
{
    if(!pulse)
        return 0.;
    return pulse->scan_angle_counts * EAARLIO_UNITS_SCAN_ANGLE_COUNTS_DEGREES;
}
