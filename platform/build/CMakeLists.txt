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
set(PLATFORM_VERSION_MAJOR 0 CACHE STRING "major version number")
mark_as_advanced(FORCE PLATFORM_VERSION_MAJOR)
# minor version number
set(PLATFORM_VERSION_MINOR 1 CACHE STRING "minor version number")
mark_as_advanced(FORCE PLATFORM_VERSION_MINOR)
# revision version number
set(PLATFORM_VERSION_REVISION 0 CACHE STRING "revision version number")
mark_as_advanced(FORCE PLATFORM_VERSION_REVISION)
message(STATUS "version: ${PLATFORM_VERSION_MAJOR}.${PLATFORM_VERSION_MINOR}.${PLATFORM_VERSION_REVISION}")

include(${PLATFORM_PATH}/build/${PLATFORM_ENV}.cmake)
include(${PLATFORM_BUILD_PATH}/util.cmake)

# do some configuration here

# platform debug
set(PLATFORM_ENABLE_DEBUG 1 CACHE BOOL "set to non zero to enable platform debug")
message(STATUS "debug mode: ${PLATFORM_ENABLE_DEBUG}")

# build platform as a dynamic library
set(PLATFORM_BUILD_SHARED 0 CACHE BOOL "build platform dynamic library")
message(STATUS "build dynamic: ${PLATFORM_BUILD_SHARED}")

# numa enable
set(PLATFORM_ENABLE_NUMA 0 CACHE BOOL "set to non zero to enable numa support")
message(STATUS "numa enable: ${PLATFORM_ENABLE_NUMA}")

# generate header file
configure_file(${PLATFORM_ROOT_PATH}/platform/include/config/tt_customization_config.h.in
               ${PLATFORM_ROOT_PATH}/platform/include/config/tt_customization_config.h
               @ONLY)

#
# component
#

#
# header file path
#

# platform
include_directories(${PLATFORM_PATH}/include)
include_directories(${PLATFORM_PATH}/native/${PLATFORM_ENV})

# depend: fcontext
include_directories(${PLATFORM_PATH}/depend/fcontext/include)
include_directories(${PLATFORM_PATH}/depend/fcontext/boost)

# depend: tls
include_directories(${PLATFORM_PATH}/depend/tls/include)
include_directories(${PLATFORM_PATH}/depend/tls/mbedtls/include/mbedtls)

# depend: xml
include_directories(${PLATFORM_PATH}/depend/xml/include)
include_directories(${PLATFORM_PATH}/depend/xml/pugixml)

# depend: dns
include_directories(${PLATFORM_PATH}/depend/dns/c-ares)
include_directories(${PLATFORM_PATH}/depend/dns/include)

#
# source file
#

file(GLOB SRC_FILE "${PLATFORM_PATH}/include/*.h")
set(BASIC_HDR ${SRC_FILE})

# source code
add_group_hdr(${PLATFORM_PATH}/include/algorithm algorithm algorithm SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/algorithm algorithm SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/algorithm/ptr algorithm algorithm/ptr SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/algorithm/ptr algorithm SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/init init init SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/init init SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/io io io SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/io io SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/os os os SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/os os SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/memory memory memory SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/memory memory SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/timer timer timer SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/timer timer SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/misc misc misc SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/misc misc SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/unit_test unit_test unit_test SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/unit_test unit_test SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/log log log SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log log SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/log/layout log log/layout SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log/layout log SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/log/io log log/io SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log/io log SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/crypto crypto crypto SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/crypto crypto SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/xml xml xml SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/xml xml SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network network network SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network network SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network/dns dns network/dns SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/dns dns SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network/ssl ssl network/ssl SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/ssl ssl SRC_FILE)

#add_group_src(${PLATFORM_PATH}/source/network/ssh ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/message ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/client ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/server ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/context ssh SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/cli cli cli SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/cli cli SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/cli/shell cli cli/shell SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/cli/shell shell SRC_FILE)

# test cases
add_group_src(${PLATFORM_PATH}/source/algorithm/test_case test_case SRC_FILE)
add_group_src(${PLATFORM_PATH}/test_case test_case SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/cli/test_case cli SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/xml/test_case xml SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/crypto/test_case crypto SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/ssl/test_case ssl SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/math/mp/test_case mp SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/dns/test_case dns SRC_FILE)

# native
add_group_hdr(${PLATFORM_PATH}/native/${PLATFORM_ENV} native "" SRC_FILE)
add_group_src(${PLATFORM_PATH}/native/${PLATFORM_ENV} native SRC_FILE)

#
# build
#

# define target
if (PLATFORM_BUILD_SHARED)
  message(STATUS "building platform dynamic lib")
  add_library(platform SHARED ${SRC_FILE})
else ()
  message(STATUS "building platform static lib")
  add_library(platform STATIC ${SRC_FILE})
endif ()

# force to generate Headers link in mac framework
set_target_properties(platform PROPERTIES PUBLIC_HEADER "${BASIC_HDR}")

# set specific properties
platform_set_properties()

# link specific libraries
platform_link_libraries()

#
# link
#

# depends
add_subdirectory(${PLATFORM_PATH}/depend/fcontext fcontext)
target_link_libraries(platform fcontext)

# depends
set(TLS_LIB)
set(X509_LIB)
set(CRYPTO_LIB)
add_subdirectory(${PLATFORM_PATH}/depend/tls tls)
target_link_libraries(platform ${CRYPTO_LIB} ${X509_LIB} ${TLS_LIB})

# depends
add_subdirectory(${PLATFORM_PATH}/depend/xml xml)
target_link_libraries(platform xml)

# depends
set(DNS_LIB)
add_subdirectory(${PLATFORM_PATH}/depend/dns dns)
target_link_libraries(platform ${DNS_LIB})
