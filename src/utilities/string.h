#ifndef SKAI_UTILITIES_STRING_H
#define SKAI_UTILITIES_STRING_H

#include <stdarg.h>
#include <stdlib.h>

size_t buffer_size_for_format(
  const char* format,
  ...
);

size_t buffer_size_for_format_v(
  const char* format,
  va_list arguments
);

char* format_to_new_string(
  const char* format,
  ...
);

char* format_to_new_string_v(
  const char* format,
  va_list arguments
);


#endif
