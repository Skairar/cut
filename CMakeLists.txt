cmake_minimum_required(VERSION 3.22)

project(CPU-Usage-Tracker LANGUAGES C VERSION 0.0.1)

include(CTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_C_COMPILEsR_ID STREQUAL "GNU")
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
  )
endif()

include_directories(src)

#add_subdirectory(src/tests)

enable_testing()

add_executable(
  main
  src/main.c
  src/cpu_diagnostics/linux.c
)
