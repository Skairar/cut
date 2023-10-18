#include "string.h"

#include <stdlib.h>
#include <stdio.h>

size_t buffer_size_for_format(
  const char* format,
  ...
) {
  va_list arguments;
  va_start(arguments, format);
  int buff_size = vsnprintf(NULL, 0, format, arguments);
  if (buff_size < 0) {
    return 0;
  }
  va_end(arguments);
  return (size_t)buff_size + 1;
}

size_t buffer_size_for_format_v(
  const char* format,
  va_list arguments
) {
  int buff_size = vsnprintf(NULL, 0, format, arguments);
  if (buff_size < 0) {
    return 0;
  }
  return (size_t)buff_size + 1;
}

char* format_to_new_string(
  const char* format,
  ...
) {
  //need two lists, for two iterations
  va_list arguments1, arguments2;
  va_start(arguments1, format);
  va_copy(arguments2, arguments1);
  int buff_size = buffer_size_for_format_v(format, arguments1);
  va_end(arguments1);
  if (buff_size == 0) {
    va_end(arguments2);
    return NULL;
  }

  char* buffer = malloc(sizeof(char) * buff_size);
  if (buffer == NULL) {
    va_end(arguments2);
    return NULL;
  }

  int status_flag = vsprintf(buffer, format, arguments2);
  va_end(arguments2);

  if (status_flag < 0) {
    free(buffer);
    return NULL;
  }
  return buffer;
}

char* format_to_new_string_v(
  const char* format,
  va_list arguments
) {
  va_list arguments_cpy;
  va_copy(arguments_cpy, arguments);
  int buff_size = buffer_size_for_format_v(format, arguments_cpy);
  va_end(arguments_cpy);
  if (buff_size == 0) {
    return NULL;
  }

  char* buffer = malloc(sizeof(char) * buff_size);
  if (buffer == NULL) {
    return NULL;
  }

  int status_flag = vsprintf(buffer, format, arguments);

  if (status_flag < 0) {
    free(buffer);
    return NULL;
  }
  return buffer;
}