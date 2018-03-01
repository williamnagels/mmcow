project (Linker)
include( CTest )
set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required (VERSION 3.9.1)

set (CMAKE_CXX_STANDARD 17)
if (WIN32)
	set (CMAKE_CXX_FLAGS "/std:c++latest /EHsc")
endif()

set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.65.0 REQUIRED unit_test_framework date_time filesystem) 


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS}) 

add_executable(mmcow_tests 
	src/unittest_main.cpp
) 

add_custom_command(TARGET mmcow_tests
                   POST_BUILD
                   COMMAND ctest -C $<CONFIGURATION> -VV --output-on-failure)

target_link_libraries(mmcow_tests ${Boost_LIBRARIES})
add_test(mmcow mmcow_tests)
