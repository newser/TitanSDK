# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.    See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.    You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project(TitanSDK)

cmake_minimum_required(VERSION 3.1.3)

#
# definition
#

# set paths
set(ROOT_PATH ${CMAKE_CURRENT_LIST_DIR})

# BUILD_PATH is the directory storing cmake helpers
set(BUILD_PATH ${ROOT_PATH}/build)
include(${BUILD_PATH}/environment.cmake)
include(${BUILD_PATH}/${PLATFORM_ENV}.cmake)

# applications to build
set(PROGRAM_UNIT_TEST 1 CACHE BOOL "app: unit test")
set(PROGRAM_COMMAND_LINE 0 CACHE BOOL "app: command line")

set(SANITIZE_ADDRESS 0 CACHE BOOL "enable address sanitizer")

#
# component
#

# platform
set(PLATFORM_PATH ${ROOT_PATH}/platform)
add_subdirectory(${PLATFORM_PATH}/build platform)

# program: unit test
if (PROGRAM_UNIT_TEST)
    if (PLATFORM_ENV STREQUAL ios-xcode)
        add_subdirectory(${ROOT_PATH}/program/ios_unit_test unit_test)
    elseif (PLATFORM_ENV STREQUAL android-ndk)
        
    else ()
        add_subdirectory(${ROOT_PATH}/program/unit_test unit_test)
    endif()
endif ()

#
# header file path
#

#
# source file
#

#
# build
#

#
# link
#

#
# package
#
