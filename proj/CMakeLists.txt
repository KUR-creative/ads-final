cmake_minimum_required(VERSION 3.0)

PROJECT (ads_final)
#SET (CMAKE_VERBOSE_MAKEFILE true)
#ADD_COMPILE_OPTIONS(-g -Wall)
ADD_COMPILE_OPTIONS(-g -Wall -Werror)

# unit test
add_executable(test rg_tree.c test.cpp)
target_link_libraries(test gtest)

# prop test
add_subdirectory("rapidcheck")
add_executable(ptest ptest.cpp)
target_link_libraries(ptest rapidcheck)

# main
add_executable(main main.c rg_tree.c common.c)

# pyprop
add_executable(pyprop pyprop.c rg_tree.c common.c)

# bst
add_library(bst SHARED bst.c)
