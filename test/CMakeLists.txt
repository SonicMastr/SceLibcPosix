cmake_minimum_required(VERSION 3.7)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(Test)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

file(GLOB_RECURSE sources *.c)

add_executable(test $<TARGET_OBJECTS:crt0_main>
    ${sources}
)

add_dependencies(test SceLibcPosix)

target_compile_options(test PRIVATE -fno-lto -fpermissive)

target_include_directories(test PRIVATE ../src/include ../src/internal)

target_link_libraries(test PRIVATE
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    SceLibcPosix_stub_weak
    SceRtc_stub
    SceNet_stub_weak
    SceProcessmgr_stub
    SceKernelThreadMgr_stub
    SceLibcExt_stub_weak
    SceFios2_stub_weak
    SceLibKernel_stub
)

vita_create_self(eboot.bin test PRIVATE
    UNSAFE
)

add_custom_target(devkit-launch
    DEPENDS eboot.bin-self
    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/sce_module"
    COMMAND cp "$ENV{VITASDK}/arm-vita-eabi/modules/SceLibcPosix.suprx" "${CMAKE_CURRENT_BINARY_DIR}/sce_module/SceLibcPosix.suprx"
    COMMAND psp2run.exe /c:process /debug /kill /workingDirectory:"${CMAKE_CURRENT_BINARY_DIR}" /elf "${CMAKE_CURRENT_BINARY_DIR}/eboot.bin"
    COMMENT "Launching ${CMAKE_CURRENT_BINARY_DIR}/eboot.bin"
)

