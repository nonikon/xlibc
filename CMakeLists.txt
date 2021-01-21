cmake_minimum_required(VERSION 3.10)
project(xlibc)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(XLIBC_LIBRARY_TYPE "STATIC"
    CACHE STRING "Build static library or shared library")
set_property(CACHE XLIBC_LIBRARY_TYPE PROPERTY
    STRINGS "STATIC;SHARED")

set(XARRAY_ENABLE_CACHE Off
    CACHE BOOL "Enable XARRAY_ENABLE_CACHE")
set(XHASH_ENABLE_CACHE Off
    CACHE BOOL "Enable XHASH_ENABLE_CACHE")
set(XHASH_DEFAULT_SIZE "64"
    CACHE STRING "Value of XHASH_DEFAULT_SIZE")
set(XHASH_DEFAULT_LOADFACTOR "75"
    CACHE STRING "Value of XHASH_DEFAULT_LOADFACTOR")
set(XLIST_ENABLE_CACHE Off
    CACHE BOOL "Enable XLIST_ENABLE_CACHE")
set(XLIST_ENABLE_SORT On
    CACHE BOOL "Enable XLIST_ENABLE_SORT")
set(XLIST_ENABLE_CUT On
    CACHE BOOL "Enable XLIST_ENABLE_CUT")
set(XRBT_ENABLE_CACHE Off
    CACHE BOOL "Enable XBRT_ENABLE_CACHE")
set(XSTR_DEFAULT_CAPACITY "32"
    CACHE STRING "Value of XSTR_DEFAULT_CAPACITY")
set(XSTR_NO_EXTRA Off
    CACHE BOOL "Enable XSTR_NO_EXTRA")
set(XVEC_DEFAULT_CAPACITY "16"
    CACHE STRING "Value of XVEC_DEFAULT_CAPACITY")

configure_file(xconfig.h.in xconfig.h)

set(XLIBC_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/xconfig.h
    xarray.h
    xhash.h
    xlist.h
    xrbtree.h
    xstring.h
    xvector.h
)
add_library(xlibc ${XLIBC_LIBRARY_TYPE}
    xarray.c
    xhash.c
    xlist.c
    xrbtree.c
    xstring.c
    xvector.c
)
target_compile_definitions(xlibc PUBLIC HAVE_XCONFIG_H)
target_include_directories(xlibc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
)
set_target_properties(xlibc PROPERTIES
    PUBLIC_HEADER "${XLIBC_HEADERS}"
)

install(TARGETS xlibc EXPORT xlibc-targets
    PUBLIC_HEADER DESTINATION include/xlibc
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/xlibc
)
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
