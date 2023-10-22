#ifndef SKAI_LOGGER_H
#define SKAI_LOGGER_H

#include <stdio.h>
#include <stdbool.h>

#include "severity.h"
#include "output_sinks.h"


/**
 * @brief Initializes logger, should be called at the very beginning of the
 * program.
 * 
 * @return 0 on success, error code on failure. 
 */
int log_init(void);

/**
 * @brief Frees all resources owned by the logger, should be called at the
 * end of program.
 * 
 * As the logger might have ownership over some file streams, not letting it
 * clean up properly might result in undefined behaviour.
 */
void log_destroy(void);

/**
 * @brief Sets minimum severity used by the logger for filtering
 * 
 * Logger will deny creation of new records with severity lesser than provided
 * here
 * 
 * @param severity 
 */
void log_set_min_severity(
  enum log_severity severity
);

/**
 * @brief Adds file sink.
 * 
 * @param output_stream 
 * @param owning 
 */
void log_add_sink_f(
  FILE output_stream[static 1],
  bool owning,
  sink_printer printer
);

void log_puts(
  enum log_severity severity,
  const char* message
);


void log_printf(
  enum log_severity severity,
  const char* format,
  ...
);


void log_process_some_dur(timespan_t duration);

void log_process_all();


#endif