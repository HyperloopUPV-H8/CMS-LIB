cmake_minimum_required (VERSION 3.4)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(arm-none-eabi.cmake)

project (CMS-LIB ASM C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY cms-lib)
set(STLIB_DIR "$ENV{STLIB_PATH}")
  
file(GLOB_RECURSE SOURCE_C ${CMAKE_SOURCE_DIR}/ *.c)
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_SOURCE_DIR}/ *.cpp)
file(GLOB_RECURSE SOURCE_H ${CMAKE_SOURCE_DIR}/ *.h)
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_SOURCE_DIR}/ *.hpp)

if(${NUCLEO})
  add_definitions(-DNUCLEO)
  add_definitions(-DHSE_VALUE=8000000)
else()
  add_definitions(-DBOARD)
  add_definitions(-DHSE_VALUE=25000000)
endif()

if(${RELEASE} STREQUAL "Release")
  set(OPTIMIZATION -O3)
  set(DEBUG_CONFIGURATION -g0)
elseif(${RELEASE} STREQUAL "ReleaseDebug")
  set(OPTIMIZATION -O3)
  set(DEBUG_CONFIGURATION -g3)
else()
  set(OPTIMIZATION -O0)
  set(DEBUG_CONFIGURATION -g3)
endif()

message("\n\nPROJECT CONFIGURATION:\n")
if(${ETHERNET})
  set(ETH STLIB_ETH)
  message(STATUS "ETHERNET is ENABLED ")
else()
message(STATUS "ETHERNET is DISABLED ")
endif()
message(STATUS "[DEBUG, RELEASE, RELEASE_DEBUG]:")

if(${RELEASE} STREQUAL "Release")
  message( "\tRELEASE")
elseif(${RELEASE} STREQUAL "ReleaseDebug")
  message( "\tRELEASE_DEBUG")
else()
  message( "\tDEBUG")
endif()

message(STATUS "Optimization value: " ${OPTIMIZATION})
message(STATUS "Debug configuration value: " ${DEBUG_CONFIGURATION})

message(STATUS "[BOARD, NUCLEO]:")

if(${NUCLEO})
  message("\tNUCLEO")
else()
  message("\tBOARD")
endif()
message("\n\nEND OF PROJECT CONFIGURATION\n\n")

add_library(${LIBRARY} STATIC
  ${SOURCE_H}
  ${SOURCE_C}
  ${SOURCE_HPP}
  ${SOURCE_CPP}
)

target_compile_definitions(${LIBRARY} PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32H723xx
  -DDEBUG
  ${ETH}
  -DUSING_CMAKE
)

target_compile_options(${LIBRARY} PUBLIC
  -mcpu=cortex-m7
  -mfpu=fpv5-d16
  -mfloat-abi=hard
  -mthumb
  ${OPTIMIZATION}
  ${DEBUG_CONFIGURATION}
  -ffunction-sections
  -fdata-sections
  -fno-exceptions
  ## disable Warnings for C files, this is Driver code from ST
  $<$<COMPILE_LANGUAGE:C>:-w>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-Wall>
  $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
  $<$<COMPILE_LANGUAGE:CXX>:-Werror>
  -Wno-psabi
  -specs=nosys.specs
)

target_include_directories(${LIBRARY} PUBLIC
  ${STLIB_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
  ${STLIB_DIR}/Drivers/CMSIS/Include
  ${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc
  ${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
  ${STLIB_DIR}/Inc
  ${STLIB_DIR}/Inc/HALAL
  ${STLIB_DIR}/Inc/HALAL/Models
  ${STLIB_DIR}/Inc/HALAL/Services
  ${STLIB_DIR}/Inc/ST-LIB_LOW
  ${STLIB_DIR}/Inc/ST-LIB_HIGH
  Inc
  Inc/Models
  Inc/Models/Communication
  Inc/Models/Communication/CAN
  Inc/C++Utilities
)
