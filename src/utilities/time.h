#ifndef SKAI_TIME_H
#define SKAI_TIME_H

#include <stdbool.h>
#include <time.h>

/**
 * @file Small module with definitions and functions to ease working with time
 * values
 * 
 * Defines types timepoint_t and timespan_t, representing a specific point in
 * time and duration respectively, however as they're both just aliases for
 * the C11 standard type timespec they have mainly a purely semantic role,
 * which proves to be necessary as other standard functions treat timespec
 * as representing both concepts.
 * 
 * Functions defined in this header keep the timespec meant as timepoint_t
 * normalized, i.e. no negative values are permitted for both tv_sec and tv_nsec
 * fields and tv_nsec is kept in range [0...NS_PER_SEC].
 * 
 * timespan_t might have negative values depending on context, with each
 * function noting whether it's permissible, but even then it enforces a
 * normal form with both fields having the same sign and absolute value of the
 * tv_nsec still being in range. As standard library function might not like
 * negatively valued timespec, it should be avoided outside user defined context
 * that specifically have the need for it.
 */



/**
 * @brief Helper constants for time calculations.
 * 
 */
enum {
  MS_PER_SEC = 1000,
  NS_PER_MS = 1000000,
  NS_PER_SEC = 1000000000
};

/**
 * @brief Semantically represents timespec intended to mean a point in time.
 */
typedef struct timespec timepoint_t;

/**
 * @brief Semantically represents timespec intended to mean duration.
 * Both timepoint_t and timespan_t are just different names for the same type
 * to somewhat differentiate between operations and their intended uses.
 */
typedef struct timespec timespan_t;


timepoint_t timepoint_normalize(
  timepoint_t time
);


timepoint_t timepoint_after(
  timepoint_t timepoint,
  timespan_t delta
);

timespan_t timepoint_dur(
  timepoint_t start,
  timepoint_t end
);

bool timepoint_gt(
  timepoint_t left,
  timepoint_t right
);

#endif