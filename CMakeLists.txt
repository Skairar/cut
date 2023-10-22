cmake_minimum_required(VERSION 3.22)

project(CPU-Usage-Tracker LANGUAGES C VERSION 0.0.1)

include(CTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wfatal-errors
  )
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  add_compile_options(
    -Weverything
    #current standard allows for those, adhering to this rule would force the
    #split of initializations from declarations in several cases and
    #unnecessarily clutter the code
    -Wno-declaration-after-statement
    #temporary
    -Wno-documentation
    #Necessary as I use wrappers around stdio functions for formatting
    #messages for logger
    -Wno-format-nonliteral
  )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
      #for Valgrind
      -gdwarf-4
    )
  endif()
endif()

include_directories(src)

add_subdirectory(src/tests)

enable_testing()

add_library(
  utilities STATIC
  src/utilities/string.c
  src/utilities/time.c
)

add_library(
  queue STATIC
  src/data_structures/queue.c
  src/data_structures/message_queue.c
)

add_library(
  logger STATIC
  src/logger/logger.c
  src/logger/log_record.c
  src/logger/output_sinks.c
  src/logger/severity.c
)

target_link_libraries(logger queue utilities)

add_executable(
  main
  src/main.c
  src/cpu_diagnostics/linux.c
)

target_link_libraries(main logger)
