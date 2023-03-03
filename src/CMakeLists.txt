cmake_minimum_required(VERSION 3.7)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(libSceLibcPosix)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

file(GLOB_RECURSE sources *.c)

add_executable(SceLibcPosix $<TARGET_OBJECTS:crt0_module>
    ${sources}
)

add_dependencies(SceLibcPosix SceLibcExt_stub_weak)

target_compile_options(SceLibcPosix PRIVATE -fno-lto)

target_include_directories(SceLibcPosix PRIVATE ../include include internal)

link_directories(${CMAKE_BINARY_DIR}/SceLibcExt)

target_link_libraries(SceLibcPosix PRIVATE
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    SceRtc_stub
    SceNetCtl_stub_weak
    SceNet_stub_weak
    SceProcessmgr_stub
    SceSysmodule_stub
    SceKernelThreadMgr_stub
    SceIofilemgr_stub_weak
    SceFios2_stub_weak
    SceLibcExt_stub_weak
    SceLibKernel_stub
    gcc
)

vita_create_self(SceLibcPosix.suprx SceLibcPosix PRIVATE
    CONFIG exports.yml
    UNSAFE
)

vita_create_stubs(stubs SceLibcPosix ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stubs/
    DESTINATION lib
    FILES_MATCHING PATTERN "*.a"
)

install(FILES ${CMAKE_BINARY_DIR}/SceLibcExt/libSceLibcExt_stub.a
    ${CMAKE_BINARY_DIR}/SceLibcExt/libSceLibcExt_stub_weak.a
    DESTINATION lib
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SceLibcPosix.suprx
    DESTINATION "$ENV{VITASDK}/arm-vita-eabi/modules"
)
