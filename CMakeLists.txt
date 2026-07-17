cmake_minimum_required(VERSION 3.7)

option(BUILD_TESTS "Build test programs" OFF)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

project(SceLibcPosix)

find_program(VITA_LIBS_GEN vita-libs-gen)
if(NOT VITA_LIBS_GEN)
  message(FATAL_ERROR "vita-libs-gen not found in PATH; is VITASDK installed?")
endif()

set(SCELIBCEXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SceLibcExt)
set(SCELIBCEXT_YML ${SCELIBCEXT_DIR}/exports.yml)
set(SCELIBCEXT_GEN ${SCELIBCEXT_DIR}/CMakeLists.txt)

file(GLOB SCELIBCEXT_STUBS ${SCELIBCEXT_DIR}/*.S)
if(NOT EXISTS ${SCELIBCEXT_GEN} OR NOT SCELIBCEXT_STUBS OR ${SCELIBCEXT_YML} IS_NEWER_THAN ${SCELIBCEXT_GEN})
  message(STATUS "Generating SceLibcExt stubs from ${SCELIBCEXT_YML}")
  execute_process(
    COMMAND ${VITA_LIBS_GEN} -c ${SCELIBCEXT_YML} ${SCELIBCEXT_DIR}
    RESULT_VARIABLE SCELIBCEXT_GEN_RESULT
  )
  if(NOT SCELIBCEXT_GEN_RESULT EQUAL 0)
    message(FATAL_ERROR "vita-libs-gen failed (${SCELIBCEXT_GEN_RESULT}) generating SceLibcExt stubs")
  endif()

  file(GLOB SCELIBCEXT_STUBS ${SCELIBCEXT_DIR}/*.S)
endif()

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
  ${SCELIBCEXT_GEN}
  ${SCELIBCEXT_STUBS}
)

add_subdirectory(SceLibcExt)
add_subdirectory(crt)
add_subdirectory(src)

SET(MODULE ON CACHE BOOL "Build as SUPRX for PS Vita")
SET(STANDALONE_BUILD OFF CACHE BOOL "Build without SceLibcPosix (Only if building a Module)")
add_subdirectory(external/pthread-embedded)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
