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
 * the C11 standard type timespec they have a purely semantic role,
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

/**
 * @brief Normalization of timespec value
 * 
 * First it eliminates overflow in tv_nsec and carries the proper number into
 * seconds.
 * Then, it makes sure tv_nsec has the same sign as tv_sec.
 * 
 * For instance, number like {1, -300kk} will be normalized to {0, 7kkk} and
 * {-4, 5kkk} to {1, 0}. 
 * 
 * @param time 
 * @return Normalized timespec value 
 */
struct timespec timespec_norm(
  struct timespec time
);

/**
 * @brief Normalizes the timespec and returns its absolute value
 * 
 * Convenience form for timespec_abs(timespec_norm(time)).
 * 
 * @param time 
 * @return struct timespec 
 */
struct timespec timespec_norm_abs(
  struct timespec time
);

/**
 * @brief Absolute value of a timespec
 * 
 * Doesn't perform normalization, only changes signs if necessary.
 * 
 * @param time 
 * @return timespec with absolute values of provided timespec.
 */
struct timespec timespec_abs(
  struct timespec time
);

/**
 * @brief Get timepoint object in place from seconds and nanoseconds.
 * 
 * Negative values are generally undefined behaviour as of now
 * 
 * @param seconds 
 * @param nanoseconds 
 * @return timepoint_t 
 */
timepoint_t timepoint_s_ns(
  long long int seconds,
  long long int nanoseconds
);

/**
 * @brief Get timepoint object in place with current time.
 * 
 * @return timepoint_t 
 */
timepoint_t timepoint_now();

/*timepoint_t timepoint_normalize(
  timepoint_t time
);*/

/**
 * @brief Calculate future timepoint from provided timepoint and delta
 * 
 * @param timepoint tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @param delta abs(tv_nsec) in [0..NS_PER_SEC-1] and both fields with same sign
 * @return timepoint_t 
 */
timepoint_t timepoint_after(
  timepoint_t timepoint,
  timespan_t delta
);

/**
 * @brief Check whether left variable is bigger than right
 * 
 * @param left v_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @param right v_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @return returns true if left represents further timepoint, false otherwise
 */
bool timepoint_gt(
  timepoint_t left,
  timepoint_t right
);

/**
 * @brief Get timespan object in place from seconds and nanoseconds.
 * 
 * Same as timepoint_s_ns, but provided for semantic distinction.
 * 
 * @param seconds 
 * @param nanoseconds 
 * @return timespan_t 
 */
timespan_t timespan_s_ns(
  long long int seconds,
  long long int nanoseconds
);

/**
 * @brief Get timespan object in place from microseconds
 * 
 * @param microseconds 
 * @return timespan_t 
 */
timespan_t timespan_ms(
  long long int microseconds
);

/**
 * @brief Calculates duration from start to end
 * 
 *  
 * @param start tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @param end tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @return timespan_t duration, negative if start > end
 */
timespan_t timespan_dur(
  timepoint_t start,
  timepoint_t end
);

/**
 * @brief Calculates absolute duration from start to end
 * 
 *  
 * @param start tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @param end tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @return timespan_t duration, always positive
 */
timespan_t timespan_dur_abs(
  timepoint_t start,
  timepoint_t end
);

/**
 * @brief Calculates leftover time based on the point of start, planned point
 * of end and duration that passed between the two.
 * 
 * First, it calculates duration between the two timepoints, then it
 * subtracts actual_duration from the calculated one, result is normalized.
 * 
 * @param start tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative and
 * smaller than end
 * @param end tv_nsec in [0..NS_PER_SEC-1] and both fields non-negative
 * @param actual_duration
 * @return Leftover duration
 */
timespan_t timespan_time_left(
  timepoint_t start,
  timepoint_t planned_end,
  timespan_t actual_duration
);


#endif