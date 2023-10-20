add_executable(
  utilities_time_test
  utilities/time_test.c
  ../utilities/time.c
)

add_test(
  NAME Time-Test
  COMMAND utilities_time_test
)

add_executable(
  queue_test
  data_structures/queue_test.c
  ../data_structures/queue.c
)

add_test(
  NAME Queue-Test
  COMMAND queue_test
)

