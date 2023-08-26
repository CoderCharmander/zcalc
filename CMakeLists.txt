cmake_minimum_required(VERSION 3.19) # CMP0069 NEW
cmake_policy(SET CMP0069 NEW)
include(FetchContent)

# Defaults, change as needed
set(PICO_SDK_FETCH_FROM_GIT ON CACHE STRING "")
set(PICO_BOARD pico_w CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE STRING "")

include(pico_sdk_import.cmake)

get_filename_component(SRC_ABSOLUTE src/ ABSOLUTE)
set(FREERTOS_CONFIG_FILE_DIRECTORY ${SRC_ABSOLUTE} CACHE STRING "")
set(FREERTOS_PORT GCC_RP2040 CACHE STRING "")
FetchContent_Declare(FreeRTOS-Kernel URL https://github.com/FreeRTOS/FreeRTOS-Kernel/releases/download/V10.5.1/FreeRTOS-KernelV10.5.1.zip)
FetchContent_Populate(FreeRTOS-Kernel)
FetchContent_GetProperties(FreeRTOS-Kernel SOURCE_DIR FREERTOS_KERNEL_PATH)

include(FreeRTOS_Kernel_import.cmake)
project(zcalc C CXX ASM)
add_subdirectory("${freertos-kernel_SOURCE_DIR}" "${freertos-kernel_BINARY_DIR}")
enable_testing()

set(PICO_CXX_ENABLE_RTTI OFF)
set(PICO_CXX_ENABLE_EXCEPTIONS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
pico_sdk_init()


FetchContent_Declare(u8g2 GIT_REPOSITORY https://github.com/olikraus/u8g2.git GIT_TAG f22a5a84209645f65c6ece0661c7f7518e7c8e9e)
FetchContent_MakeAvailable(u8g2)

# Lord forgive me for deterministically seeding the random number generator (it's not being used anyway)
add_compile_options(-D NO_ASM -DCLN_RAND_SEED_HI=0 -DCLN_RAND_SEED_LO=0)
#set(CLN_USE_GMP FALSE CACHE INTERNAL "Use GMP")
include(CheckIPOSupported)
check_ipo_supported()

function(fetch_cas)
    # CLN generates an incorrect #define when CMAKE_SYSTEM_PROCESSOR contains dashes (such as `cortex-m0`)
    # Make a function so changes are scoped
    set(CMAKE_SYSTEM_PROCESSOR "arm")
cmake_policy(SET CMP0069 NEW)
    FetchContent_Declare(CLN GIT_REPOSITORY https://gitlab.com/CoderCharmander/cln-embedded.git GIT_TAG 4699370f13de1bbbaf2e979fa6ba34b54be61a84)
    FetchContent_MakeAvailable(CLN)
#    target_compile_options(cln PRIVATE -fno-rtti)
#    FetchContent_Declare(ginac GIT_REPOSITORY https://gitlab.com/CoderCharmander/ginac-embedded.git GIT_TAG 468895b58596176027f4f88e2c3770c97745236a)
#    FetchContent_MakeAvailable(ginac)
#    set_target_properties(ginac PROPERTIES COMPILE_OPTIONS "${ginac_opts}")
endfunction()

fetch_cas()

#add_library(no_rtti INTERFACE)
add_subdirectory(src)
# NOTE: without LTO, u8g2 overflows and the build fails
# ...but the Pico SDK crashes and burns when LTO is applied to it
# Additionally, if CLN is LTO-d, the linker starts taking up all the RAM and violently swapping
# TODO use a good linker
set_property(TARGET u8g2 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
#target_compile_options(no_rtti INTERFACE -fno-rtti)
#target_link_libraries(cln PRIVATE no_rtti)