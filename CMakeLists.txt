cmake_minimum_required (VERSION 3.14)

if(DEFINED CMSLIB_NAME_SUFFIX)
  project (CMS-LIB-${CMSLIB_NAME_SUFFIX} C CXX)
  set(CMSLIB_LIBRARY cms-lib-${CMSLIB_NAME_SUFFIX})
else()
  project (CMS-LIB C CXX)
  set(CMSLIB_LIBRARY cms-lib)
endif()
set(CMSLIB_LIBRARY ${CMSLIB_LIBRARY} PARENT_SCOPE)

file(GLOB_RECURSE SOURCE_H ${CMAKE_CURRENT_LIST_DIR}/Inc/*.h)
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_CURRENT_LIST_DIR}/Inc/*.hpp)
file(GLOB_RECURSE SOURCE_C ${CMAKE_CURRENT_LIST_DIR}/Src/*.c)
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_CURRENT_LIST_DIR}/Src/*.cpp)

add_library(${CMSLIB_LIBRARY} STATIC
  ${SOURCE_H}
  ${SOURCE_C}
  ${SOURCE_HPP}
  ${SOURCE_CPP}
)

set_target_properties(${CMSLIB_LIBRARY} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED C_STANDARD 11 C_STANDARD_REQUIRED)

target_compile_options(${CMSLIB_LIBRARY} PRIVATE
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mcpu=cortex-m7>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfpu=fpv5-d16>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfloat-abi=hard>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mthumb>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-specs=nosys.specs>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-ffunction-sections>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-fdata-sections>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-fno-exceptions>
  
  -Wno-psabi
  
  $<$<COMPILE_LANGUAGE:C>:-w>
  
  $<$<COMPILE_LANGUAGE:CXX>:-Wall>
  $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
  $<$<COMPILE_LANGUAGE:CXX>:-Werror>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

target_include_directories(${CMSLIB_LIBRARY} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Inc
  ${CMAKE_CURRENT_LIST_DIR}/Inc/Models
  ${CMAKE_CURRENT_LIST_DIR}/Inc/C++Utilities
)

if (PROJECT_IS_TOP_LEVEL)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()
