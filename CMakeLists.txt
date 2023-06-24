project(PMCSN_Project)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 11)
set(SRC_INCLUDE lib/rngs.c lib/rvms.c)

if (FINITE_HORIZON)
    add_definitions(-DFINITE_HORIZON=1)
endif()

if (IMPROVEMENT)
    add_definitions(-DIMPROVEMENT=1)
endif()

add_executable(run src/main.c src/utils.c ${SRC_INCLUDE})
