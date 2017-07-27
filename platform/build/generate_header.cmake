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

set(HEADER_PATH ${PLATFORM_OUTPATH}/include)

function(copy_header src_dir)
    file(GLOB f RELATIVE "${src_dir}" "${src_dir}/*.h")
    add_custom_command(TARGET generate_header POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/temp.tar
                       COMMAND ${CMAKE_COMMAND} -E tar cf ${CMAKE_BINARY_DIR}/temp.tar ${f}
                       WORKING_DIRECTORY ${src_dir})
    add_custom_command(TARGET generate_header POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_BINARY_DIR}/temp.tar
                       COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/temp.tar
                       WORKING_DIRECTORY ${HEADER_PATH})
endfunction()

#
# platform
#

# common
add_custom_target(generate_header
                  COMMAND ${CMAKE_COMMAND} -E remove_directory ${HEADER_PATH}
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLATFORM_PATH}/include ${HEADER_PATH}
                  COMMAND ${CMAKE_COMMAND} -E remove ${HEADER_PATH}/.gitignore
                  COMMENT "Generating platform header files")
add_dependencies(generate_header platform)

# native
copy_header(${PLATFORM_PATH}/native/${PLATFORM_ENV})

#
# depends: dns
#

set(PATH ${PLATFORM_PATH}/depend/dns)

add_custom_command(TARGET generate_header
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/c-ares/ares.h ${HEADER_PATH}
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/c-ares/ares_version.h ${HEADER_PATH}
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/c-ares/ares_dns.h ${HEADER_PATH}
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/c-ares/ares_rules.h ${HEADER_PATH}
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/include/ares_build.h ${HEADER_PATH}
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/include/ares_config.h ${HEADER_PATH})

#
# depends: fcontext
#

set(PATH ${PLATFORM_PATH}/depend/fcontext)

if (PLATFORM_ENV STREQUAL windows-msvc)
    add_custom_command(TARGET generate_header
                       COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/include/tt_fiber_windows_wrapper.h ${HEADER_PATH}
                       COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/boost/tt_fcontext.h ${HEADER_PATH})
else ()
    add_custom_command(TARGET generate_header
                       COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/include/tt_fiber_wrapper.h ${HEADER_PATH}
                       COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/boost/tt_fcontext.h ${HEADER_PATH})
endif ()

#
# depends: tls
#

set(PATH ${PLATFORM_PATH}/depend/tls)

copy_header(${PATH}/mbedtls/include/mbedtls)

add_custom_command(TARGET generate_header
                   COMMAND ${CMAKE_COMMAND} -E copy ${PATH}/include/tt_mbedtls_config.h ${HEADER_PATH})

#
# depends: xml
#

# none to generate
