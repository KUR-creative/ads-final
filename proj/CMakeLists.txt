cmake_minimum_required(VERSION 3.0)

PROJECT (ads_final)
#SET (CMAKE_VERBOSE_MAKEFILE true)
#ADD_COMPILE_OPTIONS(-g -Wall)
#ADD_COMPILE_OPTIONS(-g -Wall -Werror)

# main
add_executable(main main.c bst.c common.c solve.c)

# bst
add_library(bst SHARED bst.c)
