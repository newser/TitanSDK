# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# unit test for TitanSDK
project(TitanSDK)

# cmake version 3.0 above
cmake_minimum_required(VERSION 3.0)

#
# definition
#

# set paths
set(TSCM_ROOT_PATH ${CMAKE_CURRENT_LIST_DIR})

# TSCM_BUILD_PATH is the directory storing cmake helpers
set(TSCM_BUILD_PATH ${TSCM_ROOT_PATH}/build)
include(${TSCM_BUILD_PATH}/environment.tscm)
include(${TSCM_BUILD_PATH}/${TSCM_ENV}.tscm)

# applications to build
set(APP_UNIT_TEST 1 CACHE BOOL "app: unit test")
set(APP_COMMAND_LINE 0 CACHE BOOL "app: command line")

#
# component
#

# platform
set(TSCM_PLATFORM_PATH ${TSCM_ROOT_PATH}/platform)
add_subdirectory(${TSCM_PLATFORM_PATH}/build platform)

#
# header file path
#

#
# source file
#

#
# build
#

if (APP_UNIT_TEST)
  if (TSCM_ENV STREQUAL ios-xcode)
    add_subdirectory(${TSCM_BUILD_PATH}/application/ios_unit_test unit_test)
  else ()
    add_subdirectory(${TSCM_BUILD_PATH}/application/unit_test unit_test)
  endif()
endif ()

#
# link
#

