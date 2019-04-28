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

if (NOT DEFINED PLATFORM_PATH)
    message(FATAL_ERROR "PLATFORM_PATH undefined")
endif ()

#
# component
#

#
# header file path
#

# platform
include_directories(${PLATFORM_PATH}/include)
include_directories(${PLATFORM_PATH}/native/${PLATFORM_ENV})
include_directories(${PLATFORM_PATH}/native/${PLATFORM_ENV}/include)

# library: fcontext
include_directories(${PLATFORM_PATH}/library/fcontext/include)
include_directories(${PLATFORM_PATH}/library/fcontext/boost)

# library: tls
include_directories(${PLATFORM_PATH}/library/tls/include)
include_directories(${PLATFORM_PATH}/library/tls/mbedtls/include/mbedtls)

# library: dns
include_directories(${PLATFORM_PATH}/library/dns/c-ares)
include_directories(${PLATFORM_PATH}/library/dns/include)

# library: zip
include_directories(${PLATFORM_PATH}/library/zip/zlib)
include_directories(${PLATFORM_PATH}/library/zip/libzip/lib)
include_directories(${PLATFORM_PATH}/library/zip/include)

#
# source file
#

set(SRC_FILE unit_test.cpp)
set(SRC_FILE ${SRC_FILE} unit_test_process.c)

#
# build
#

add_executable(unit_test ${SRC_FILE})

#
# link
#

# link platform module
target_link_libraries(unit_test platform)
add_dependencies(unit_test platform)

