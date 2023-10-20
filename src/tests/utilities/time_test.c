#include <assert.h>
#include "utilities/time.h"

#include <stdio.h>

/**
 * @file Tests of operations on time
 * 
 * Very simple tests that were needed to get rid of some paranoia.
 */

const long long int hundred_million = NS_PER_SEC / 10;

int main(void) {
  
  timepoint_t sample_point;
  timespan_t sample_dur;

  sample_dur = timespec_norm(timespan_s_ns(1, hundred_million * 5));
  assert(
    ((sample_dur.tv_sec == 1) && (sample_dur.tv_nsec == hundred_million * 5 )) &&
    "{1s, 500,000,000ns} -> 1.500,000,000"
  );

  sample_dur = timespec_norm(timespan_s_ns(-1, -hundred_million * 5));
  assert(
    ((sample_dur.tv_sec == -1) && (sample_dur.tv_nsec == -hundred_million * 5 )) &&
    "{-1s, -500,000,000ns} -> -1.500,000,000"
  );

  sample_dur = timespec_norm(timespan_s_ns(-2, 2 * NS_PER_SEC));
  assert(
    ((sample_dur.tv_sec == 0) && (sample_dur.tv_nsec == 0)) &&
    "{-2s, 2,000,000,000ns} -> 0.0"
  );

  sample_dur = timespec_norm(timespan_s_ns(4, -4ll * NS_PER_SEC));
  assert(
    ((sample_dur.tv_sec == 0) && (sample_dur.tv_nsec == 0)) &&
    "{4s, -4,000,000,000ns} -> 0.0"
  );

  sample_dur = timespec_norm(timespan_s_ns(1, -hundred_million * 3));
  fprintf(stderr, "%llis %llins\n", (long long int)sample_dur.tv_sec, (long long int)sample_dur.tv_nsec);
  assert(
    ((sample_dur.tv_sec == 0) && (sample_dur.tv_nsec == hundred_million * 7)) &&
    "{1s, -300,000,000ns} -> 0.700,000,000"
  );

  sample_dur = timespec_norm(timespan_s_ns(-3, hundred_million * 8));
  assert(
    ((sample_dur.tv_sec == -2) && (sample_dur.tv_nsec == -hundred_million * 2)) &&
    "{-3s, 800,000,000ns} -> -2.200,000,000"
  );

  timepoint_t zero_point = timepoint_s_ns(0, 0);
  timepoint_t zero_nine_repeat_point = timepoint_s_ns(0, NS_PER_SEC - 1);
  timepoint_t five_and_forty_point = timepoint_s_ns(5, (NS_PER_SEC / 10) * 4);

  //timespan_t sample_dur;
  //timespan_t zero_dur = timespan_s_ns(0, 0);
  timespan_t one_and_half_dur = timespan_s_ns(1, NS_PER_SEC / 2);
  timespan_t one_and_eighty_dur = timespan_s_ns(1, (NS_PER_SEC / 10) * 8);

  timespan_t negative_one_and_sixty_dur = 
    timespan_s_ns(-1, -((NS_PER_SEC / 10) * 6));


  //timepoint_after test 1, basic
  sample_point = timepoint_after(
    zero_point,
    one_and_eighty_dur
  );
  assert(
    (sample_point.tv_sec == one_and_eighty_dur.tv_sec) &&
    (sample_point.tv_nsec == one_and_eighty_dur.tv_nsec) &&
    "From timepoint 0 to 1.8"
  );
  
  //timepoint_after test 2, nsec overflow and sec carry
  sample_point = timepoint_after(
    zero_nine_repeat_point,
    one_and_half_dur
  );
  assert(
    (sample_point.tv_sec == 2) &&
    (sample_point.tv_nsec == ((NS_PER_SEC / 2) - 1)) &&
    "From timepoint 0.999,999,999 through duration 1.5 to 2.499,999,999"
  );

  //timepoint_after test 3, negative delta
  sample_point = timepoint_after(
    five_and_forty_point,
    negative_one_and_sixty_dur
  );
  assert(
    (sample_point.tv_sec == 3) &&
    (sample_point.tv_nsec == ((NS_PER_SEC) / 10) * 8) &&
    "From timepoint 5.4 through duration -1.6 to 3.8"
  );


  //timespan_dur test 1
  sample_dur = timespan_dur(zero_nine_repeat_point, five_and_forty_point);
  assert(
    ((sample_dur.tv_sec == 4) && (sample_dur.tv_nsec == 400000001)) &&
    "dur(0.999,999,999, 5.400,000,000) -> 4.400,000,001"
  );

  //timespan_dur test 2, negative result 1
  sample_dur = timespan_dur(zero_nine_repeat_point, zero_point);
  assert(
    ((sample_dur.tv_sec == 0) && (sample_dur.tv_nsec == -999999999)) &&
    "dur(0.999,999,999, 0.0) -> -0.999,999,999"
  );


  //timespan_time_left test
  sample_dur = timespan_time_left(
    zero_nine_repeat_point,
    five_and_forty_point,
    one_and_eighty_dur
  );
  assert(
    ((sample_dur.tv_sec == 2) && (sample_dur.tv_nsec == 600000001)) &&
    "time_left(0.999,999,999, 5.4, 1.8) -> 2.600,000,001"
  );
  


  return 0;
}