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
# load environment info
#

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    cmake_minimum_required(VERSION 3.7.2)

    message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
    message(STATUS "CMAKE_ANDROID_ARCH_ABI: ${CMAKE_ANDROID_ARCH_ABI}")
    message(STATUS "CMAKE_ANDROID_NDK: ${CMAKE_ANDROID_NDK}")

elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    # available only on mac
    set(PLATFORM_IOS 0 CACHE BOOL "use predefined ios-xcode toolchain")

    if (PLATFORM_IOS)
        include(${BUILD_PATH}/ios-xcode.toolchain.cmake)
    endif ()

endif ()

# get application os
function(get_os os)
    if (CMAKE_SYSTEM_NAME STREQUAL Windows)
        set(${os} windows PARENT_SCOPE)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
        set(${os} linux PARENT_SCOPE)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
        set(${os} macos PARENT_SCOPE)
    elseif (CMAKE_SYSTEM_NAME STREQUAL iOS)
        set(${os} ios PARENT_SCOPE)
    elseif (CMAKE_SYSTEM_NAME STREQUAL Android)
        set(${os} android PARENT_SCOPE)
    else ()
        # more to be added
        message(FATAL_ERROR "unsupported os: ${${CMAKE_SYSTEM_NAME}}")
    endif ()
endfunction(get_os)

# get application cpu
function(get_cpu cpu)
    set(__cpu ${CMAKE_SYSTEM_PROCESSOR})
    string(TOLOWER ${__cpu} __cpu)
    # CMAKE_SYSTEM_PROCESSOR is exactly the cpu even in cross build mode
    
    if (__cpu MATCHES "(.*intel.*|.*amd.*|.*x64.*|.*x86.*|.*i.86)")
        set(${cpu} x86 PARENT_SCOPE)
    elseif (__cpu MATCHES "(.*arm.*)" OR __cpu MATCHES "(.*aarch64.*)")
        set(${cpu} arm PARENT_SCOPE)
    elseif (__cpu MATCHES "(.*mips.*)")
        set(${cpu} mips PARENT_SCOPE)
    else ()
        # more to be add
        message(FATAL_ERROR "unsupported cpu: ${__cpu}")
    endif ()

endfunction(get_cpu)

# get application toolchain
function(get_toolchain toolchain)
    if (CMAKE_SYSTEM_NAME STREQUAL Android)
        set(${toolchain} ndk PARENT_SCOPE)
    elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
        # vs must use msvc as compiler
        set(${toolchain} msvc PARENT_SCOPE)
    elseif (CMAKE_GENERATOR MATCHES ".*Unix Makefiles.*" AND 
            CMAKE_C_COMPILER_ID MATCHES GNU AND
            CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set(${toolchain} gnu PARENT_SCOPE)
    elseif (CMAKE_GENERATOR MATCHES ".*Xcode.*" AND
            CMAKE_C_COMPILER_ID MATCHES Clang AND
            CMAKE_CXX_COMPILER_ID MATCHES Clang)
        set(${toolchain} xcode PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "unsupported toolchain: ${CMAKE_GENERATOR}, ${CMAKE_C_COMPILER_ID}")
    endif ()
endfunction(get_toolchain)

# get host
function(get_host_os host_os)
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        set(${host_os} windows PARENT_SCOPE)
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        set(${host_os} linux PARENT_SCOPE)    
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
        set(${host_os} macos PARENT_SCOPE)
    else ()
        # more to be added
        message(FATAL_ERROR "unsupported system: ${${CMAKE_HOST_SYSTEM_NAME}}")
    endif ()
endfunction(get_host_os)

# construct environment name
message(STATUS "DETECTING ENVIRONMENT ...")

# os
message(STATUS "detecting os")
get_os(PLATFORM_ENV_OS)
set(PLATFORM_ENV_OS ${PLATFORM_ENV_OS} CACHE INTERNAL "os")
message(STATUS "os: ${PLATFORM_ENV_OS}")

# cpu
message(STATUS "detecting cpu")
get_cpu(PLATFORM_ENV_CPU)
if (PLATFORM_IOS_SIMULATOR)
    set(PLATFORM_ENV_CPU x86)
endif ()
set(PLATFORM_ENV_CPU ${PLATFORM_ENV_CPU} CACHE INTERNAL "cpu")
message(STATUS "cpu: ${PLATFORM_ENV_CPU}")

# app building toolchain
message(STATUS "detecting toochain")
get_toolchain(PLATFORM_ENV_TOOLCHAIN)
set(PLATFORM_ENV_TOOLCHAIN ${PLATFORM_ENV_TOOLCHAIN} CACHE INTERNAL "toochain")
message(STATUS "toochain: ${PLATFORM_ENV_TOOLCHAIN}")

# host
message(STATUS "detecting host")
get_host_os(PLATFORM_ENV_HOST)
set(PLATFORM_ENV_HOST ${PLATFORM_ENV_HOST} CACHE INTERNAL "host")
message(STATUS "host: ${PLATFORM_ENV_HOST}")

# make env name
#if (PLATFORM_ENV_OS STREQUAL PLATFORM_ENV_HOST)
#    set(PLATFORM_ENV "${PLATFORM_ENV_OS}-${PLATFORM_ENV_CPU}-${PLATFORM_ENV_TOOLCHAIN}")
#else ()
#    set(PLATFORM_ENV "${PLATFORM_ENV_OS}-${PLATFORM_ENV_CPU}-${PLATFORM_ENV_TOOLCHAIN}--${PLATFORM_ENV_HOST}")
#endif ()
# the toolchain implies host platform, e.g. vs implies building on windows
#set(PLATFORM_ENV "${PLATFORM_ENV_OS}-${PLATFORM_ENV_CPU}-${PLATFORM_ENV_TOOLCHAIN}")
set(PLATFORM_ENV "${PLATFORM_ENV_OS}-${PLATFORM_ENV_TOOLCHAIN}")

include(TestBigEndian)
TEST_BIG_ENDIAN(PLATFORM_ENV_BIG_ENDIAN)

message(STATUS "DETECTING ENVIRONMENT DONE: ${PLATFORM_ENV}")

#
# detect environment detail
#

# detect environment detail
function(detect_env param_name param_val)    
    try_run(__run_result
            __compile_result 
            ${CMAKE_BINARY_DIR}
            ${ROOT_PATH}/platform/host/${PLATFORM_ENV}_detector.c
            COMPILE_OUTPUT_VARIABLE __compiler_output
            RUN_OUTPUT_VARIABLE __run_output
            ARGS ${param_name})
    
    # does compiling succeed?
    if (NOT __compile_result)
        message(FATAL_ERROR "compiling env detector failed: ${__compiler_output}")
        return()
    endif ()
    
    # does succeed in running?
    if (NOT __run_result EQUAL 0)
        message(FATAL_ERROR "running env detector failed: ${__run_result}")
        return()
    endif ()
    
    # return result
    set(${param_val} ${__run_output} PARENT_SCOPE)
endfunction(detect_env)

macro(detect_env_helper param description)
    if ("${${param}}" STREQUAL "")
        message(STATUS "detecting ${description}")
        detect_env("${param}" __val)
        set(${param} ${__val} CACHE INTERNAL "${description}")
    endif ()
    message(STATUS "${description}: ${${param}}")
endmacro(detect_env_helper)

# detecting detail depends on configuration
set(PLATFORM_ENVIRONMENT_DETAIL 0 
    CACHE BOOL "detecting environment detail for optimization")
mark_as_advanced(PLATFORM_ENVIRONMENT_DETAIL)
if (PLATFORM_ENVIRONMENT_DETAIL)
    message(STATUS "DETECTING ENVIRONMENT DETAIL ...")

    # detect os detail
    detect_env_helper(PLATFORM_ENV_OS_DETAIL "os detail")
    detect_env_helper(PLATFORM_ENV_OS_VER_DETAIL "os version detail")
    detect_env_helper(PLATFORM_ENV_OS_FEATURE_DETAIL "os feature detail")
    
    # detect cpu detail
    detect_env_helper(PLATFORM_ENV_CPU_DETAIL "cpu detail")
    detect_env_helper(PLATFORM_ENV_CPU_FEATURE_DETAIL "cpu feature detail")

    # detect toolchain
    detect_env_helper(PLATFORM_ENV_TOOLCHAIN_DETAIL "app building toolchain detail")
    detect_env_helper(PLATFORM_ENV_TOOLCHAIN_VER_DETAIL "app building toolchain version detail")
    
    # host inforamtion are for cmake usage, no need to pass to application
    
    message(STATUS "DETECTING ENVIRONMENT DETAIL DONE")
else (PLATFORM_ENVIRONMENT_DETAIL)
    # should assign a numberical value
    set(PLATFORM_ENV_OS_DETAIL TT_ENV_UNKNOWN_VAL)
    set(PLATFORM_ENV_OS_VER_DETAIL TT_ENV_UNKNOWN_VAL)
    set(PLATFORM_ENV_OS_FEATURE_DETAIL 0)

    set(PLATFORM_ENV_CPU_DETAIL TT_ENV_UNKNOWN_VAL)
    set(PLATFORM_ENV_CPU_FEATURE_DETAIL 0)
        
    set(PLATFORM_ENV_TOOLCHAIN_DETAIL TT_ENV_UNKNOWN_VAL)
    set(PLATFORM_ENV_TOOLCHAIN_VER_DETAIL TT_ENV_UNKNOWN_VAL)
endif (PLATFORM_ENVIRONMENT_DETAIL)

#
# configuration
#

# generate header file
configure_file(${ROOT_PATH}/platform/include/config/tt_environment_config.h.in
               ${ROOT_PATH}/platform/include/config/tt_environment_config.h
               @ONLY)
