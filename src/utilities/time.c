#include "time.h"

timepoint_t timepoint_after(
  timepoint_t timepoint,
  timespan_t delta
) {
  timepoint.tv_sec = (timepoint.tv_sec + delta.tv_sec) + 
                     ((timepoint.tv_nsec + delta.tv_nsec) / NS_PER_SEC);
  timepoint.tv_nsec = (timepoint.tv_nsec + delta.tv_nsec) % NS_PER_SEC;
  return timepoint;
}

timespan_t timepoint_dur(
  timepoint_t start,
  timepoint_t end
) {
  struct timespec duration = {
    .tv_sec = 0,
    .tv_nsec = 0
  };
  if (
    (start.tv_sec > end.tv_sec) ||
    ((start.tv_sec == end.tv_sec) && (start.tv_nsec > end.tv_nsec)) 
  ) {
    return duration;
  }
  duration.tv_sec = (end.tv_sec - start.tv_sec) - (start.tv_nsec > end.tv_nsec);
  duration.tv_nsec = (start.tv_nsec > end.tv_nsec) ?
                     (NS_PER_SEC - (start.tv_nsec - end.tv_nsec)) : 
                     (end.tv_nsec - start.tv_nsec);

  return duration;
}

bool timespec_gt(
  struct timespec left,
  struct timespec right
) {
  if (left.tv_sec == right.tv_sec) {
    return left.tv_nsec > right.tv_nsec;
  }
  return left.tv_sec > right.tv_sec;
}