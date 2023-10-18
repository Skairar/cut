#ifndef SKAI_LOGGER_SEVERITY_H
#define SKAI_LOGGER_SEVERITY_H

/**
 * @brief Values identifying severities used by logger
 */
enum log_severity {
  log_trace,
  log_debug,
  log_info,
  log_warning,
  log_error,
  log_fatal
};

const char* log_severity_str(
  enum log_severity severity
);

#endif
