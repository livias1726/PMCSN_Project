project(PMCSN_Project)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 11)
set(SRC_INCLUDE src/utils.c src/handle_events.c lib/rngs.c lib/rvms.c src/headers/init.h src/init.c
        src/random_gen_functions.c src/headers/rgf.h src/sim.c src/headers/sim.h lib/rvgs.c src/stats.c)

if (FINITE_HORIZON)
    add_definitions(-DFINITE_HORIZON=1)
endif()

if (IMPROVEMENT)
    add_definitions(-DIMPROVEMENT=1)
endif()

if (ABO_ID)
    add_definitions(-DABO_ID=1)
endif()

if (AUDIT)
    add_definitions(-DAUDIT=1)
endif ()

if (REAL)
    add_definitions(-DREAL=1)
endif ()

if (ACTIVATION)
    add_definitions(-DACTIVATION=1)
endif ()

add_executable(PMCSN_Project src/main.c ${SRC_INCLUDE})
target_link_libraries(PMCSN_Project m)