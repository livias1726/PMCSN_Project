cmake_minimum_required(VERSION 3.16)
project(PMCSN_Project)

set(CMAKE_C_STANDARD 11)
set(SRC_INCLUDE src/utils.c lib/rngs.c lib/rvms.c src/headers/init.h src/init.c
        src/random_gen_functions.c src/headers/rgf.h src/sim.c src/headers/sim.h lib/rvgs.c src/stats.c
        src/events/patient_arrival.c
        src/events/matching.c
        src/events/organ_arrival.c
        src/events/activation.c
        src/events/transplant.c
        src/events/organ_renege.c
        src/events/patient_renege.c
        src/events/event_utils.c)

if (IMPROVEMENT)
    add_definitions(-DIMPROVEMENT=1)
endif()

if (ABO_ID)
    add_definitions(-DABO_ID=1)
endif()

if (AUDIT)
    add_definitions(-DAUDIT=1)
endif ()

add_executable(PMCSN_Project src/main.c ${SRC_INCLUDE})
target_link_libraries(PMCSN_Project m)