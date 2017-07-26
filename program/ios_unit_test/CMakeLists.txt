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

# depend: fcontext
include_directories(${PLATFORM_PATH}/depend/fcontext/include)
include_directories(${PLATFORM_PATH}/depend/fcontext/boost)

# depend: tls
include_directories(${PLATFORM_PATH}/depend/tls/include)
include_directories(${PLATFORM_PATH}/depend/tls/mbedtls/include/mbedtls)

# depend: dns
include_directories(${PLATFORM_PATH}/depend/dns/c-ares)
include_directories(${PLATFORM_PATH}/depend/dns/include)

#
# source file
#

set(SRC_FILE AppDelegate.m main.m ViewController.m
             unit_test_process.c unit_test.c)
set(RESOURCE_FILE Main.storyboard)

#
# build
#

add_executable(unit_test MACOSX_BUNDLE ${SRC_FILE} ${RESOURCE_FILE})

set_target_properties(unit_test PROPERTIES
                      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/titansdk-ios-unit-test-Info.plist
                      RESOURCE "${RESOURCE_FILE}"
                      XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES
                      XCODE_ATTRIBUTE_GCC_PREFIX_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/titansdk-ios-unit-test-Prefix.pch
                      XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
                      XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_CURRENT_SOURCE_DIR}/unit_test.entitlements)

#
# link
#

# link platform module
target_link_libraries(unit_test platform)
add_dependencies(unit_test platform)

# core foundation
target_link_libraries(unit_test
                      "-framework CoreGraphics"
                      "-framework Foundation"
                      "-framework UIKit")
