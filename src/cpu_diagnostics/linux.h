#ifndef SKAI_CPU_DIAGNOSTICS_LINUX_H
#define SKAI_CPU_DIAGNOSTICS_LINUX_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @file Contains functions and data structures for processing the /proc/stat
 * file on linux system and calculating the cpu usage based on it.
 */

/**
 * @brief Stores the relevant information about the /proc/stat layout
 * 
 * Stores the number of lines that start from 'cpu' and the number of columns
 * containing numeric data in each of those lines.
 */
typedef struct stat_layout {
  size_t cpu_count;           /**< Number of lines starting with CPU,
                                   usually odd and greater by one from
                                   the actual number of cores to account for
                                   the line with summed data*/
  size_t cpu_column_count;    /**< Number of columns with numeric data*/
} stat_layout_t;

/**
 * @brief Constants for human readable access to fields in stat_cpu_row_t.
 * 
 * Based on total 10 known to the developer during the time of writing.
 */
enum cpu_column_id {
  user_proc_col,
  nice_proc_col,
  system_proc_col,
  idle_col,
  iowait_col,
  irq_col,
  softirq_col,
  steal_col,
  guest_col,
  guest_nice_col
};

/**
 * @brief Variable containing a numeric data from the cpu section of /proc/stat/
 * file.
 * 
 * note: Unsigned long long was considered, as those values are always written
 * as integers, but there existed uncertainty in developers mind regarding
 * how big those numbers can really get, so ultimately decision was made to
 * change it to float, as all the precision from storing it in an integer
 * would be lost anyway in the following calculations.
 */
typedef float stat_cpu_field_t;

/**
 * @brief Dynamic array representing a row of numeric fields from the cpu
 * section of /proc/stat/ file.
 * 
 * Should be handled with associated functions. It's size is based on
 * information from stat_layout_t object used for its creation.
 */
typedef stat_cpu_field_t* stat_cpu_row_t;

/**
 * @brief Dynamic array of individual rows from the cpu section of /proc/stat
 * file.
 * 
 * Should be handled with associated functions. It's size is based on
 * information from stat_layout_t object used for its creation.
 */
typedef stat_cpu_row_t* stat_cpu_array_t;




/**
 * @brief Sets layout fields to appropriate values based on source
 * 
 * The values are set based on counted spaces and newlines, intended to
 * read until it reaches the next section of the file('intr'), reaching an
 * EOF returns the non-zero value without checking for specific errors on the
 * source stream, so it's up to caller to find out what went wrong and handle
 * it accordingly. Depending on when EOF happens, some of the layout fields
 * might be left zeroed.
 * 
 * Works on assumption it won't encounter any character that isn't an
 * alphanumeric, space or newline, which shouldn't be the case when used
 * correctly. For instance, if a character like "tab" somehow gets into the
 * stream, resulting layout might be invalid.
 * 
 * @param layout Will be zeroed and set anew
 * @param source File stream pointing to /proc/stat file or with similarly
 * formatted data
 * @return 0 on success, non-0 on approaching the EOF, caller should check
 * the source stream for errors in case of non-0 value
 */
int stat_layout_set_lf(
  stat_layout_t layout[static 1],
  FILE source[static 1]
);

/**
 * @brief Similar to stat_layout_set_lf, but saves information into layout
 * object owned by implementation.
 * 
 * @param source File stream pointing to /proc/stat file or with similarly
 * formatted data
 * @return 0 on success, non-0 on approaching the EOF, caller should check
 * the source stream for errors in case of non-0 value
 */
int stat_layout_set_f(
  FILE source[static 1]
);

/**
 * @brief Returns global layout object.
 * 
 * @return stat_layout_t object currently stored by library
 */
stat_layout_t stat_layout_get(void);
/**
 * @brief Allocates new stat_cpu_array_t object, based on passed layout.
 * 
 * The used layout object is assumed to be invariable and should be used with
 * each operation on this stat_cpu_array_t that requires it. Trying to pass
 * a different layout or changing its values will result in memory leaks.
 * 
 * @param layout Describes the dimensions of newly created array.
 * @return stat_cpu_array_t* 
 */
stat_cpu_array_t stat_cpu_array_create_l(stat_layout_t layout[static 1]);

/**
 * @brief Allocates new stat_cpu_array_t object, based on globally stored
 * layout.
 * 
 * @return stat_cpu_array_t* 
 */
stat_cpu_array_t stat_cpu_array_create(void);

/**
 * @brief Frees the memory allocated for stat_cpu_array_t object. Requires
 * layout used for its creation.
 * 
 * @param array stat_cpu_array_t object to be freed.
 * @param layout Same as the one used for the creation of the array.
 */
void stat_cpu_array_free_l(
  stat_cpu_array_t array,
  stat_layout_t layout[static 1]
);

/**
 * @brief Frees the memory allocated for stat_cpu_array_t object.
 * 
 * @param array stat_cpu_array_t object to be freed that was created using the
 * global layout.
 */
void stat_cpu_array_free(stat_cpu_array_t array);

/**
 * @brief Read /proc/stat data regarding cpu load from the source to the
 * provided stat_cpu_array_t using provided stat_layout_t object.
 * 
 * Stops reading when 1st read failure occurs, in such case returns described
 * values for diagnostics.
 * 
 * @param array Created with the use of provided layout
 * @param source File stream pointing to /proc/stat or a file with same
 * formatting
 * @param layout Used for both bound checks and deducing how much data needs to
 * be processed
 * @return 0 on completion of all reads(i.e. success), number of successful
 * reads in case match failure occurs and EOF in case of general input failure.
 * Generally, non-0 value means something bad.
 */
int stat_cpu_array_read_fl(
  stat_cpu_array_t array,
  FILE source[static 1],
  stat_layout_t layout[static 1]
);

/**
 * @brief Read /proc/stat data regarding cpu load from the source to the
 * provided stat_cpu_array_t using global layout.
 * 
 * Stops reading when 1st read failure occurs, in such case returns described
 * values for diagnostics.
 * 
 * @param array Created with the use of global layout
 * @param source File stream pointing to /proc/stat or a file with same
 * formatting
 * @return 0 on completion of all reads(i.e. success), number of successful
 * reads in case match failure occurs and EOF in case of general input failure.
 * Generally, non-0 value means something bad.
 */
int stat_cpu_array_read_f(
  stat_cpu_array_t array,
  FILE source[static 1]
);

void stat_cpu_row_delta_l(
  stat_cpu_row_t old,
  stat_cpu_row_t curr,
  stat_cpu_row_t result,
  stat_layout_t layout[static 1]
);

void stat_cpu_row_delta(
  stat_cpu_row_t old,
  stat_cpu_row_t curr,
  stat_cpu_row_t result
);

float stat_cpu_row_percentage_8(
  stat_cpu_row_t delta
);


#endif
