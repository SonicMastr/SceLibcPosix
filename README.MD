# SceLibcPosix
### Posix (and extra) compatibility layer for SceLibc

# Features over Newlib
 - # Much faster cached IO over Newlib, with FIOS2 Posix IO
    - With FIOS2 and SceLibc stdio as the driving IO backends, cached reads are miles apart
    - Random 4KiB is 48.9x faster using SceLibcPosix, and Reread 4MiB is, and I'm not kidding, over 4000x faster using SceLibcPosix. (Benchmark results in repo)
 - # SceLibc Auto-Extending Heap for Malloc
    - No more guessing how much memory is necessary, simply add 
        ```
        unsigned int sceLibcHeapSize = -1; # Upper Limit for Extended Alloc (-1 is unrestricted)
        unsigned int sceLibcHeapExtendedAlloc = 1;
 - # Proper Dynamic Library Support
    - Surpx's built against SceLibcPosix's crt0_module.o will be able to utilize a shared heap. This means full Malloc support in modules and no new C runtime instance.
 - # Compatiblity with SceLibc dependant modules
 - # MUSL's structure for contibutions
    - Newlib is, let's face it, a mess when it comes to structure and iterations of features/fixes. I chose musl's structure to allow better readability and ease of development. Most features can be added with a single file, and building simply requires a VitaSDK installation

# This is a full replacement for the Newlib C Standard Library
## How to use in your projects
Until furthur improvements and integration, there are some extra steps to use this library instead of Newlib

1. `-nostdlib -nostartfiles -nodefaultlibs` flags need to be used to avoid linking Newlib
2. Link `crt0_main.o` for Eboots, and `crt0_module.o` for Suprx Modules (This does not support Skprx, obviously)
3. Remove `_newlib_heap_size_user` and replace it with `unsigned int sceLibcHeapSize`
4. Bundle `SceLibcPosix.suprx` in `sce_module/SceLibcPosix.suprx` in your VPK
   - If pthreads are needed (C++ or explicitly) link `pthread_stub_weak`, bundle `pthread.suprx` in `sce_module/pthread.suprx`, and add `SCE_USER_MODULE_LIST("app0:sce_module/pthread.suprx");` anywhere in your source to ensure it is loaded on application startup.
5. Removing newlib means removing default links as well, so you'd need to restore them, at a minimum
    ```
    target_link_libraries(myapp
        -nostdlib
        -nostartfiles
        -nodefaultlibs
        $ENV{VITASDK}/arm-vita-eabi/lib/crt0_main.o #crt0_module.o for suprx
        SceLibcPosix_stub_weak
        SceLibcExt_stub_weak
        SceFios2_stub_weak
        SceLibKernel_stub
        SceIofilemgr_stub
        SceKernelThreadMgr_stub
        SceProcessmgr_stub
        SceSysmem_stub
        SceRtc_stub
        gcc
    )
6. You're good to go!

## How to Build and Install
1. Install (VitaSDK)[https://vitasdk.org] 
2. `git clone https://github.com/SonicMastr/SceLibcPosix.git --recurse`
3. `cd SceLibcPosix`
4. `cmake -B build -S . && cmake --build build && cmake --install build`