cmake_minimum_required(VERSION 3.10)
project(xlibc)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(XLIBC_LIBRARY_TYPE "STATIC"
    CACHE STRING "Build static library or shared library")
set_property(CACHE XLIBC_LIBRARY_TYPE PROPERTY
    STRINGS "STATIC;SHARED")

add_library(xlibc ${XLIBC_LIBRARY_TYPE}
    xarray.c
    xhash.c
    xlist.c
    xrbtree.c
    xstring.c
    xvector.c
)

install(TARGETS xlibc
    DESTINATION lib
    EXPORT xlibc-targets)
install(EXPORT xlibc-targets
    DESTINATION lib/cmake/xlibc)

set(XLIBC_ENABLE_TESTS On
    CACHE BOOL "Enable test programs")

if (XLIBC_ENABLE_TESTS)
    add_executable(xarray_test xarray_test.c)
    target_link_libraries(xarray_test xlibc)

    add_executable(xhash_test xhash_test.c)
    target_link_libraries(xhash_test xlibc)

    add_executable(xlist_test xlist_test.c)
    target_link_libraries(xlist_test xlibc)

    add_executable(xrbtree_test xrbtree_test.c)
    target_link_libraries(xrbtree_test xlibc)

    add_executable(xstring_test xstring_test.c)
    target_link_libraries(xstring_test xlibc)

    add_executable(xvector_test xvector_test.c)
    target_link_libraries(xvector_test xlibc)

    add_executable(stl_test stl_test.cpp)
endif (XLIBC_ENABLE_TESTS)
