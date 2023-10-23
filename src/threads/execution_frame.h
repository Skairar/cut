#ifndef SKAI_THREADS_EXECUTION_FRAME_H
#define SKAI_THREADS_EXECUTION_FRAME_H

#include "utilities/time.h"

int execution_frame(void* context);

void execution_frame_sleep_until(timepoint_t);

#endif
