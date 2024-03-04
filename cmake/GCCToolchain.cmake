set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(GCC_PREFIX aarch64-linux-gnu- CACHE STRING "Prefix added to each GNU binutils and GCC invocations")
set(GCC_SUFFIX -13 CACHE STRING "Suffix added to each GNU binutils and GCC invocations")
set(CMAKE_ASM_COMPILER ${GCC_PREFIX}gcc${GCC_SUFFIX})
set(CMAKE_C_COMPILER ${GCC_PREFIX}gcc${GCC_SUFFIX})
set(CMAKE_CXX_COMPILER ${GCC_PREFIX}g++${GCC_SUFFIX})
set(CMAKE_OBJCOPY ${GCC_PREFIX}objcopy)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_LINKER ${GCC_PREFIX}ld)
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
