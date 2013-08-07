# the name of the target operating system
SET(CMAKE_SYSTEM_NAME QNX)

# which compilers to use for C and C++
SET(arch gcc_ntoarmv7le)
SET(CMAKE_C_COMPILER qcc -V${arch})
SET(CMAKE_CXX_COMPILER QCC -V${arch})

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH $ENV{QNX_TARGET}/armle-v7 $ENV{QNX_TARGET})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
