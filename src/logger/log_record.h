#ifndef SKAI_LOGGER_LOG_RECORD_H
#define SKAI_LOGGER_LOG_RECORD_H

#include <threads.h>
#include <time.h>

#include "severity.h"
#include "utilities/time.h"

/**
 * @brief Structure representing a single log record awaiting for being further
 * formatted and printed into sinks by the logger.
 */
typedef struct log_record {
  thrd_t thread_id;             /**<ID number of the record producer's thread*/
  timepoint_t timestamp;             /**<Time of record's creation*/
  enum log_severity severity;   /**<Severity of described event*/
  char* message;                /**<Attached message, assumed ownership*/
} log_record_t;

/**
 * @brief Returns newly allocated record with provided values assigned.
 * 
 * @param thread_id ID of the thread issuing the record
 * @param timestamp Time point of record creation
 * @param severity Event's severity, might be used to filter records from sinks
 * @param message Attached message, record takes ownership of it and frees its
 * memory after processing
 * @return log_record_t* on success or NULL in case of heap allocation failure.
 */
log_record_t* log_record_new(
  thrd_t thread_id,
  timepoint_t timestamp,
  enum log_severity severity,
  char* message
);

/**
  * @brief Returns newly allocated record with provided values assigned and
  * provided message copied to newly allocated buffer owned by the object.
  * 
  * @param thread_id ID of the thread issuing the record
  * @param timestamp Time point of record creation
  * @param severity Event's severity, might be used to filter records from sinks
  * @param message Attached message, copied to newly allocated buffer.
  * @return log_record_t* on success or NULL in case of heap allocation failure.
  */
log_record_t* log_record_new_cpy_buf(
  thrd_t thread_id,
  timepoint_t timestamp,
  enum log_severity severity,
  const char* message
);

/**
 * @brief Frees the memory reserved for the record and its message
 * 
 * @param record Record to be freed
 */
void log_record_free(log_record_t* record);

/**
 * @brief Function of void(void*) signature forwarding to log_record_free
 * 
 * @param record 
 */
void log_record_deleter(void* record);


#endif
