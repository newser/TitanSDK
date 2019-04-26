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

#
# definition
#

# major version number
set(PLATFORM_VERSION_MAJOR 0 CACHE STRING "major version number")
mark_as_advanced(FORCE PLATFORM_VERSION_MAJOR)
# minor version number
set(PLATFORM_VERSION_MINOR 3 CACHE STRING "minor version number")
mark_as_advanced(FORCE PLATFORM_VERSION_MINOR)
# revision version number
set(PLATFORM_VERSION_REVISION 0 CACHE STRING "revision version number")
mark_as_advanced(FORCE PLATFORM_VERSION_REVISION)
message(STATUS "version: ${PLATFORM_VERSION_MAJOR}.${PLATFORM_VERSION_MINOR}.${PLATFORM_VERSION_REVISION}")

include(${PLATFORM_PATH}/build/${PLATFORM_ENV}.cmake)
include(${BUILD_PATH}/util.cmake)

# do some configuration here

# platform debug
set(PLATFORM_ENABLE_DEBUG 0 CACHE BOOL "set to non zero to enable platform debug")
message(STATUS "debug mode: ${PLATFORM_ENABLE_DEBUG}")

# build platform as a dynamic library
if (PLATFORM_ENV_OS STREQUAL android)
    # jni can only use shared lib
    set(PLATFORM_BUILD_SHARED 1)
else ()
    set(PLATFORM_BUILD_SHARED 0 CACHE BOOL "build platform shared library")
endif ()
message(STATUS "build dynamic: ${PLATFORM_BUILD_SHARED}")

# numa enable
set(PLATFORM_ENABLE_NUMA 0 CACHE BOOL "enable numa support")
message(STATUS "numa enable: ${PLATFORM_ENABLE_NUMA}")

# lite unit test
set(PLATFORM_UNIT_TEST_LITE 0 CACHE BOOL "lite unit test")
mark_as_advanced(FORCE PLATFORM_UNIT_TEST_LITE)

set(UNIT_TEST_CI 0 CACHE BOOL "nit test on CI")
mark_as_advanced(FORCE UNIT_TEST_CI)

set(PLATFORM_ENABLE_BACKTRACE 0 CACHE BOOL "unwind stack")

set(PLATFORM_ENABLE_MEMORY_TAG 0 CACHE BOOL "memory tag")

# generate header file
configure_file(${PLATFORM_PATH}/include/config/tt_customization_config.h.in
               ${PLATFORM_PATH}/include/config/tt_customization_config.h
               @ONLY)

if ((PLATFORM_ENV_OS STREQUAL android) AND NOT ANDROID_STUDIO)
    set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
elseif ((PLATFORM_ENV_OS STREQUAL ios) AND NOT PLATFORM_IOS_SIMULATOR)
    set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
endif ()
enable_language(ASM)

# have
include(CheckSymbolExists)
include(CheckIncludeFile)

check_symbol_exists(syslog syslog.h HAVE_SYSLOG)
check_symbol_exists(ReportEvent windows.h HAVE_WINDOWS_EVENT_LOG)
check_symbol_exists(os_log os/log.h HAVE_OSLOG)

#check_include_file(net/if_types.h HAVE_IFTYPES)

if (PLATFORM_ENABLE_BACKTRACE)
    if ((PLATFORM_ENV_OS STREQUAL windows) OR (PLATFORM_ENV_OS STREQUAL android))
        set(HAVE_LIBUNWIND 0)
        set(HAVE_BACKTRACE 0)
    elseif ((PLATFORM_ENV_OS STREQUAL ios) OR (PLATFORM_ENV_OS STREQUAL macos))
        set(HAVE_LIBUNWIND 1)
        set(HAVE_BACKTRACE 1)
    else ()
        check_symbol_exists(unw_getcontext libunwind.h HAVE_LIBUNWIND)
        check_symbol_exists(backtrace execinfo.h HAVE_BACKTRACE)
    endif ()
endif ()

configure_file(${ROOT_PATH}/platform/include/config/tt_have.h.in
               ${ROOT_PATH}/platform/include/config/tt_have.h
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
include_directories(${PLATFORM_PATH}/native/${PLATFORM_ENV}/include)

# library: fcontext
include_directories(${PLATFORM_PATH}/library/fcontext/include)
include_directories(${PLATFORM_PATH}/library/fcontext/boost)

# library: tls
include_directories(${PLATFORM_PATH}/library/tls/include)
include_directories(${PLATFORM_PATH}/library/tls/mbedtls/include/mbedtls)

# library: xml
include_directories(${PLATFORM_PATH}/library/xml/include)
include_directories(${PLATFORM_PATH}/library/xml/pugixml)

# library: dns
include_directories(${PLATFORM_PATH}/library/dns/c-ares)
include_directories(${PLATFORM_PATH}/library/dns/include)

# library: zip
include_directories(${PLATFORM_PATH}/library/zip/zlib)
include_directories(${PLATFORM_PATH}/library/zip/libzip/lib)
include_directories(${PLATFORM_PATH}/library/zip/include)

# library: http
include_directories(${PLATFORM_PATH}/library/http/http-parser)

# library: json
# including json header must begin with "json/xxx.h"
include_directories(${PLATFORM_PATH}/library/json/)

#
# source file
#

file(GLOB SRC_FILE "${PLATFORM_PATH}/include/*.h")

# source code
add_group_hdr(${PLATFORM_PATH}/include/algorithm algorithm SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/tt/algorithm algorithm SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/algorithm algorithm SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/algorithm/ptr algorithm SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/algorithm/ptr algorithm SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/config config SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/init init SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/init init SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/io io SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/io io SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/os os SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/os os SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/memory memory SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/tt/memory memory SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/memory memory SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/time time SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/time time SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/misc misc SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/tt/misc misc SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/misc misc SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/unit_test unit_test SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/unit_test unit_test SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/log log SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log log SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/log/layout log SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log/layout log SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/log/io log SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log/io log SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/log/filter log SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/log/filter log SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/crypto crypto SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/crypto crypto SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/xml xml SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/xml xml SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network/netif netif SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/netif netif SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network/dns dns SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/dns dns SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network/ssl ssl SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/ssl ssl SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/network/http http SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/http http SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/network/http/rule http SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/http/rule http SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/network/http/service http SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/http/service http SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/network/http/def http SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/http/def http SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/network/http/header http SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/http/header http SRC_FILE)

#add_group_src(${PLATFORM_PATH}/source/network/ssh ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/message ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/client ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/server ssh SRC_FILE)
#add_group_src(${PLATFORM_PATH}/source/network/ssh/context ssh SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/cli cli SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/cli cli SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/cli/shell shell SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/cli/shell shell SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/zip zip SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/zip zip SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/data data SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/data data SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/param param SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/param param SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/param/html param SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/param/html param SRC_FILE)
add_group_hdr(${PLATFORM_PATH}/include/param/html/bootstrap4 param SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/param/html/bootstrap4 param SRC_FILE)

add_group_hdr(${PLATFORM_PATH}/include/json json SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/json json SRC_FILE)

# test cases
add_group_src(${PLATFORM_PATH}/source/algorithm/test_case test_case SRC_FILE)
add_group_src(${PLATFORM_PATH}/test_case test_case SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/cli/test_case cli SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/xml/test_case xml SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/crypto/test_case crypto SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/ssl/test_case ssl SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/http/test_case http SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/network/dns/test_case dns SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/zip/test_case zip SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/data/test_case data SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/param/test_case param SRC_FILE)
add_group_src(${PLATFORM_PATH}/source/json/test_case json SRC_FILE)

# native
add_group_hdr(${PLATFORM_PATH}/native/${PLATFORM_ENV}/include native SRC_FILE)
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
set_target_properties(platform PROPERTIES 
                      VERSION ${PLATFORM_VERSION_MAJOR}.${PLATFORM_VERSION_MINOR}.${PLATFORM_VERSION_REVISION})

set(PLATFORM_OUTPATH
    ${CMAKE_BINARY_DIR}/output/platform/${PLATFORM_VERSION_MAJOR}.${PLATFORM_VERSION_MINOR}.${PLATFORM_VERSION_REVISION})

if (PLATFORM_ENV STREQUAL "windows-msvc")
    if (PLATFORM_BUILD_SHARED)
        set_target_properties(platform PROPERTIES
                              ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/shared
                              RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/shared)
        set_target_properties(platform PROPERTIES
                              ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/shared
                              RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/shared)
    else ()
        set_target_properties(platform PROPERTIES
                              ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/static)
        set_target_properties(platform PROPERTIES
                              ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/static)
    endif ()

elseif ((PLATFORM_ENV_OS STREQUAL android) AND ANDROID_STUDIO)
    # only build shared lib for android
    set_target_properties(platform PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
    set_target_properties(platform PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

else ()
    set_target_properties(platform PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/static
                          LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/shared)
    set_target_properties(platform PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/static
                          LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/shared)

endif ()

if (PLATFORM_UNIT_TEST_LITE)
    target_compile_definitions(platform PRIVATE "-D__UT_LITE__")
    message(STATUS "lite unit test")
endif ()

if (UNIT_TEST_CI)
    target_compile_definitions(platform PRIVATE "-D__UT_CI__")
    message(STATUS "unit test on CI")
endif ()

# set specific properties
platform_set_properties()

# link specific libraries
platform_link_libraries()

#
# link
#

# library: fcontext
set(FCONTEXT_SRC)
add_subdirectory(${PLATFORM_PATH}/library/fcontext fcontext)
source_group(fcontext FILES ${FCONTEXT_SRC})
target_sources(platform PRIVATE ${FCONTEXT_SRC})

# library: tls
set(TLS_LIB)
set(X509_LIB)
set(CRYPTO_LIB)
add_subdirectory(${PLATFORM_PATH}/library/tls tls)
target_link_libraries(platform ${CRYPTO_LIB} ${X509_LIB} ${TLS_LIB})

# library: xml
add_subdirectory(${PLATFORM_PATH}/library/xml xml)
target_link_libraries(platform xml)

# library: dns
set(DNS_LIB)
add_subdirectory(${PLATFORM_PATH}/library/dns dns)
target_link_libraries(platform ${DNS_LIB})

# library: zip
set(ZLIB_LIB)
set(LIBZIP_LIB)
add_subdirectory(${PLATFORM_PATH}/library/zip zip)
target_link_libraries(platform ${ZLIB_LIB} ${LIBZIP_LIB})

# library: http
set(HTTP_SRC)
add_subdirectory(${PLATFORM_PATH}/library/http http)
source_group(http FILES ${HTTP_SRC})
target_sources(platform PRIVATE ${HTTP_SRC})

# library: json
set(JSON_SRC)
add_subdirectory(${PLATFORM_PATH}/library/json json)
source_group(json FILES ${JSON_SRC})
target_sources(platform PRIVATE ${JSON_SRC})

#include(${PLATFORM_PATH}/build/generate_header.cmake)
#
## test link
#add_custom_target(test_link_debug
#                  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/test_link/test_link_debug
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_PATH}/program/test_link ${CMAKE_BINARY_DIR}/test_link_debug
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/debug/static ${CMAKE_BINARY_DIR}/test_link_debug/static
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/debug/shared ${CMAKE_BINARY_DIR}/test_link_debug/shared
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/debug/program ${CMAKE_BINARY_DIR}/test_link_debug/program
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/include ${CMAKE_BINARY_DIR}/test_link_debug/include)
#add_dependencies(test_link_debug generate_header)
#
#add_custom_target(test_link_release
#                  COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/test_link/test_link_release
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${ROOT_PATH}/program/test_link ${CMAKE_BINARY_DIR}/test_link_release
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/debug/static ${CMAKE_BINARY_DIR}/test_link_release/static
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/debug/shared ${CMAKE_BINARY_DIR}/test_link_release/shared
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/debug/program ${CMAKE_BINARY_DIR}/test_link_release/program
#                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_OUTPATH}/include ${CMAKE_BINARY_DIR}/test_link_release/include)
#add_dependencies(test_link_release generate_header)
