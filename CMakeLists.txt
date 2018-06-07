cmake_minimum_required(VERSION 2.8.12)

# switch RPATH use off
set(CMAKE_MACOSX_RPATH 0)
# force make to print the command lines
set(CMAKE_VERBOSE_MAKEFILE on)
# path to Csound cmake module
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
"${CMAKE_SOURCE_DIR}/cmake/Modules/")
# set C++ compilation flags
set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS}  "-std=c++11 -g")

# executables to build
add_executable(devlist src/tests/devlist.cpp  src/hid.c)
add_executable(trackpad  src/tests/trackpad.cpp src/hid.c)
add_executable(joystick  src/tests/joystick.cpp src/hid.c)

# add include directory for hidapi.h
include_directories(src)

# MacOS required libraries
if(APPLE)
 find_library(IOKIT IOKit)
 find_library(COREFOUNDATION CoreFoundation)
 target_link_libraries(devlist ${IOKIT} ${COREFOUNDATION})
 target_link_libraries(trackpad ${IOKIT} ${COREFOUNDATION})
 target_link_libraries(joystick ${IOKIT} ${COREFOUNDATION})
endif(APPLE)

# Csound opcode build
find_package(Csound)
if(CSOUND_FOUND)
  message(STATUS "Building HID opcodes")
  set(srcs "src/opcodes/hidops.cpp" "src/hid.c")
  if(APPLE)
        add_library(hidops SHARED ${srcs})
    else()
        add_library(hidops MODULE ${srcs})
  endif()
  target_include_directories(hidops PRIVATE
  ${CSOUND_INCLUDE_DIRS})
  target_link_libraries(hidops ${IOKIT} ${COREFOUNDATION})
endif()

