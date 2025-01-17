# Copyright(c)	2024 Christopher Taylor
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.25)

include(CheckCXXCompilerFlag)

project(oshmp)

CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)

if(COMPILER_SUPPORTS_CXX20)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++ -O3")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSHMEM REQUIRED openshmem)

set(TEST_SRCS
  communicator.hpp
  test.cpp
)

add_executable(test ${TEST_SRCS})
target_link_libraries(test ${OPENSHMEM_LIBRARIES})
target_include_directories(test ${OPENSHMEM_INCLUDE_DIRS})
target_compile_options(test PUBLIC ${OPENSHMEM_CFLAGS_OTHER})
