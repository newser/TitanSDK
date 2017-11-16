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

set(ZLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
set(LIBZIP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libzip)

#
# customization
#

# assembler code seems not stable
#if (PLATFORM_ENV_CPU STREQUAL x86)
#    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
#        set(ZLIB_ASM686 OFF)
#        set(ZLIB_AMD64 ON)
#    else ()
#        set(ZLIB_ASM686 ON)
#        set(ZLIB_AMD64 OFF)
#    endif ()
#else ()
#    set(ZLIB_ASM686 OFF)
#    set(ZLIB_AMD64 OFF)
#endif ()
set(ZLIB_ASM686 OFF)
set(ZLIB_AMD64 OFF)
hide(ASM686 BOOL ${ZLIB_ASM686})
hide(AMD64 BOOL ${ZLIB_AMD64})

hide(INSTALL_BIN_DIR PATH "${CMAKE_INSTALL_PREFIX}/bin")
hide(INSTALL_LIB_DIR PATH "${CMAKE_INSTALL_PREFIX}/lib")
hide(INSTALL_INC_DIR PATH "${CMAKE_INSTALL_PREFIX}/include")
hide(INSTALL_MAN_DIR PATH "${CMAKE_INSTALL_PREFIX}/share/man")
hide(INSTALL_PKGCONFIG_DIR PATH "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")

add_subdirectory(${ZLIB_PATH})
add_subdirectory(${LIBZIP_PATH})

set_target_properties(zlibstatic zip PROPERTIES
                      MACOSX_RPATH ON
                      POSITION_INDEPENDENT_CODE ON)

if (NOT PLATFORM_BUILD_SHARED)
    set_target_properties(zlibstatic zip PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/static
                          LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/shared
                          RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/program)
    set_target_properties(zlibstatic zip PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/static
                          LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/shared
                          RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/program)
endif ()

#
# result
#

set(ZLIB_LIB zlibstatic PARENT_SCOPE)
set(LIBZIP_LIB zip PARENT_SCOPE)
