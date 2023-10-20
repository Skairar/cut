#include "time.h"

struct timespec timespec_norm(
  struct timespec time
) {
  long long int nsec = time.tv_nsec % NS_PER_SEC;
  long long int sec = time.tv_sec + (time.tv_nsec / NS_PER_SEC);
  bool is_sign_different = (sec > 0 && nsec < 0) || (sec < 0 && nsec > 0);
  time.tv_sec = is_sign_different ? (sec + (nsec > 0) - (nsec < 0)) : sec;
  time.tv_nsec = is_sign_different ? 
    ((nsec > 0) ? (nsec - NS_PER_SEC) : (nsec + NS_PER_SEC)) : 
    nsec;
  return time;
}

struct timespec timespec_norm_abs(
  struct timespec time
) {
  return timespec_abs(timespec_norm(time));
}

struct timespec timespec_abs(
  struct timespec time
) {
  time.tv_sec = (time.tv_sec >= 0) ? time.tv_sec : -time.tv_sec;
  time.tv_nsec = (time.tv_nsec >= 0) ? time.tv_nsec : -time.tv_nsec;
  return time;
}

timepoint_t timepoint_s_ns(
  long long int seconds,
  long long int nanoseconds
) {
  timepoint_t timepoint = {
    .tv_sec = seconds,
    .tv_nsec = nanoseconds
  };
  return timepoint;
}

timepoint_t timepoint_now() {
  timepoint_t timepoint;
  timespec_get(&timepoint, TIME_UTC);
  return timepoint;
}

timepoint_t timepoint_after(
  timepoint_t timepoint,
  timespan_t delta
) {
  long long int nsec = timepoint.tv_nsec + delta.tv_nsec;
  timepoint.tv_sec = (timepoint.tv_sec + delta.tv_sec) + 
                     (nsec >= NS_PER_SEC) -
                     (nsec < 0);
  timepoint.tv_nsec = (nsec < 0) ? (NS_PER_SEC + nsec) : (nsec % NS_PER_SEC);
  return timepoint;
}

bool timepoint_gt(
  timepoint_t left,
  timepoint_t right
) {
  if (left.tv_sec == right.tv_sec) {
    return left.tv_nsec > right.tv_nsec;
  }
  return left.tv_sec > right.tv_sec;
}

timespan_t timespan_s_ns(
  long long int seconds,
  long long int nanoseconds
) {
  timespan_t timespan = {
    .tv_sec = seconds,
    .tv_nsec = nanoseconds
  };
  return timespan;
}

timespan_t timespan_ms(
  long long int microseconds
) {
  timespan_t timespan = {
    .tv_sec = microseconds / MS_PER_SEC,
    .tv_nsec = (microseconds % MS_PER_SEC) * NS_PER_MS
  };
  return timespan;  
}

timespan_t timespan_dur(
  timepoint_t start,
  timepoint_t end
) {
  return timespec_norm(timespan_s_ns(
    end.tv_sec - start.tv_sec,
    end.tv_nsec - start.tv_nsec
  ));
}

timespan_t timespan_dur_abs(
  timepoint_t start,
  timepoint_t end
) {
  return timespec_norm_abs(timespan_s_ns(
    end.tv_sec - start.tv_sec,
    end.tv_nsec - start.tv_nsec
  ));
}

timespan_t timespan_time_left(
  timepoint_t start,
  timepoint_t planned_end,
  timespan_t actual_duration
) {
  timespan_t planned_duration = timespan_dur(start, planned_end);
  return timespec_norm(timespan_s_ns(
    planned_duration.tv_sec - actual_duration.tv_sec,
    planned_duration.tv_nsec - actual_duration.tv_nsec
  ));
}