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

#
# definition
#

# major version number
set(TSCM_VERSION_MAJOR 0 CACHE STRING "major version number")
mark_as_advanced(FORCE TSCM_VERSION_MAJOR)
# minor version number
set(TSCM_VERSION_MINOR 1 CACHE STRING "minor version number")
mark_as_advanced(FORCE TSCM_VERSION_MINOR)
# revision version number
set(TSCM_VERSION_REVISION 0 CACHE STRING "revision version number")
mark_as_advanced(FORCE TSCM_VERSION_REVISION)
message(STATUS "version: ${TSCM_VERSION_MAJOR}.${TSCM_VERSION_MINOR}.${TSCM_VERSION_REVISION}")

include(${TSCM_PLATFORM_PATH}/build/${TSCM_ENV}.tscm)
include(${TSCM_BUILD_PATH}/util.tscm)

# do some configuration here

# platform debug
set(TSCM_PLATFORM_DEBUG_ENABLE 1 CACHE BOOL "set to non zero to enable platform debug")
message(STATUS "debug mode: ${TSCM_PLATFORM_DEBUG_ENABLE}")

# build platform as a dynamic library
set(TSCM_PLATFORM_BUILD_DYNAMIC 0 CACHE BOOL "build platform dynamic library")
message(STATUS "build dynamic: ${TSCM_PLATFORM_BUILD_DYNAMIC}")

# ssl enable
set(TSCM_PLATFORM_SSL_ENABLE 0 CACHE BOOL "set to non zero to enable SSL")
message(STATUS "ssl enable: ${TSCM_PLATFORM_SSL_ENABLE}")

# numa enable
set(TSCM_PLATFORM_NUMA_ENABLE 0 CACHE BOOL "set to non zero to enable numa support")
message(STATUS "numa enable: ${TSCM_PLATFORM_NUMA_ENABLE}")

# crypto enable
set(TSCM_PLATFORM_CRYPTO_ENABLE 0 CACHE BOOL "set to non zero to enable crypto")
message(STATUS "crypto enable: ${TSCM_PLATFORM_CRYPTO_ENABLE}")

# generate header file
configure_file(${TSCM_ROOT_PATH}/platform/export/config/tt_customization_config.h.in
               ${TSCM_ROOT_PATH}/platform/export/config/tt_customization_config.h
               @ONLY)

#
# component
#

#
# header file path
#

# platform
include_directories(${TSCM_PLATFORM_PATH}/export)
include_directories(${TSCM_PLATFORM_PATH}/native/${TSCM_ENV})
include_directories(${TSCM_PLATFORM_PATH}/native/${TSCM_ENV_CPU})

#
# source file
#

# source code
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/algorithm algorithm SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/event event SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/init init SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/init/config_shell config_shell SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/io io SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/os os SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/memory memory SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/mode mode SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/timer timer SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/misc misc SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/log log SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/unit_test unit_test SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/native/${TSCM_ENV} native SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/native/${TSCM_CPU} cpu SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/test_case test_case SRC_FILE)

tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network network SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/adns adns SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/adns/test_case adns SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssl ssl SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssl/test_case ssl SRC_FILE)

tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssh ssh SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssh/message ssh SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssh/client ssh SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssh/server ssh SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssh/context ssh SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/network/ssh/test_case ssh SRC_FILE)

tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/crypto crypto SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/crypto/test_case crypto SRC_FILE)

tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/math/mp mp SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/math/mp/test_case mp SRC_FILE)

tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/xml xml SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/xml/parse xml SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/xml/test_case xml SRC_FILE)

tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/cli cli SRC_FILE)
tscm_add_group_src(${TSCM_PLATFORM_PATH}/source/cli/test_case cli SRC_FILE)

#
# build
#

# define target
if (TSCM_PLATFORM_BUILD_DYNAMIC)
  message(STATUS "building platform dynamic lib")
  add_library(platform SHARED ${SRC_FILE})
else ()
  message(STATUS "building platform static lib")
  add_library(platform STATIC ${SRC_FILE})
endif ()

# link specific libraries
tscm_platform_link_libraries()

# set specific properties
tscm_platform_set_properties()

#
# link
#
