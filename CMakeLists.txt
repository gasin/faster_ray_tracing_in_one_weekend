#---------------------------------------------------------------------------------------------------
# CMake Build Configuration for the Ray Tracing Weekend Series
#
# See README.md for guidance.
#---------------------------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.1.0...3.27.0 )

project ( RTWeekend LANGUAGES CXX )

# Set to C++20
set ( CMAKE_CXX_STANDARD          20 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

# Source

set ( SOURCE_ONE_WEEKEND
  src/camera.h
  src/color.h
  src/hittable.h
  src/hittable_list.h
  src/interval.h
  src/material.h
  src/ray.h
  src/rtweekend.h
  src/sphere.h
  src/vec3.h

  src/main.cc
)

include_directories(src)

# Specific compiler flags below. We're not going to add options for all possible compilers, but if
# you're new to CMake (like we are), the following may be a helpful example if you're using a
# different compiler or want to set different compiler options.

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})

add_compile_options(-Wnon-virtual-dtor) # Class has virtual functions, but its destructor is not virtual
add_compile_options(-Wreorder) # Data member will be initialized after [other] data member
add_compile_options(-Wmaybe-uninitialized) # Variable improperly initialized
add_compile_options(-Wunused-variable) # Variable is defined but unused
add_compile_options(-Ofast -march=native)

# Executables
add_executable(inOneWeekend      ${EXTERNAL} ${SOURCE_ONE_WEEKEND})

# Profiling
add_link_options(-pg -no-pie)
add_compile_options(-pg -no-pie)

add_executable(inOneWeekend_prof ${EXTERNAL} ${SOURCE_ONE_WEEKEND})
