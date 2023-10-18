#include "severity.h"

const char* log_severity_str(
  enum log_severity severity
) {
  static const char* severity_strs[] = {
    [log_trace] = "trace",
    [log_debug] = "debug",
    [log_info] = "info",
    [log_warning] = "warning",
    [log_error] = "error",
    [log_fatal] = "fatal"
  };
  return severity_strs[severity];
}