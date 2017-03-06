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
# load environment info
#

# toolchain
set(TTCM_TOOLCHAIN "" 
    CACHE FILEPATH "specified toolchain file")
mark_as_advanced(TTCM_TOOLCHAIN)

set(TTCM_TOOLCHAIN_IOS_XCODE 0 
    CACHE BOOL  "use predefined ios-xcode toolchain")

# choose toolchain
if (TTCM_TOOLCHAIN STREQUAL "")
  if (TTCM_TOOLCHAIN_IOS_XCODE)
    set(TTCM_TOOLCHAIN "ios-xcode.toolchain.cmake")
  endif ()
endif()

# check and load toolchain
if (TTCM_TOOLCHAIN)
  if (EXISTS ${TTCM_BUILD_PATH}/${TTCM_TOOLCHAIN})
    include(${TTCM_BUILD_PATH}/${TTCM_TOOLCHAIN})
  else ()
    message(FATAL_ERROR "toolchain does not exist")
  endif ()
endif (TTCM_TOOLCHAIN)

# get application os
function(get_app_os app_os)
  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
      set(${app_os} windows PARENT_SCOPE)
  elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
      set(${app_os} linux PARENT_SCOPE)  
  elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
      set(${app_os} macos PARENT_SCOPE)
  elseif (CMAKE_SYSTEM_NAME STREQUAL iOS)
      set(${app_os} ios PARENT_SCOPE)
  else ()
    # more to be added
    message(FATAL_ERROR "unsupported os: ${${CMAKE_SYSTEM_NAME}}")
  endif ()
endfunction(get_app_os)

# get application cpu
function(get_app_cpu app_cpu)
  set(__cpu ${CMAKE_SYSTEM_PROCESSOR})
  string(TOLOWER ${__cpu} __cpu)
  # CMAKE_SYSTEM_PROCESSOR is exactly the cpu even in cross build mode
  
  if (__cpu MATCHES "(.*intel.*|.*amd.*|.*x64.*|.*x86.*|.*i.86)")
    set(${app_cpu} x86 PARENT_SCOPE)
  elseif (__cpu MATCHES "(.*arm.*)")
    set(${app_cpu} arm PARENT_SCOPE)
  else ()
    # more to be add
    message(FATAL_ERROR "unsupported cpu: ${__cpu}")
  endif ()

endfunction(get_app_cpu)

# get application toolchain
function(get_app_toolchain app_toolchain)
  if (CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
    # vs must use msvc as compiler
    set(${app_toolchain} msvc PARENT_SCOPE)
  elseif (CMAKE_GENERATOR MATCHES ".*Unix Makefiles.*" AND 
          CMAKE_C_COMPILER_ID MATCHES GNU AND
          CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(${app_toolchain} gnu PARENT_SCOPE)
  elseif (CMAKE_GENERATOR MATCHES ".*Xcode.*" AND
          CMAKE_C_COMPILER_ID MATCHES Clang AND
          CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(${app_toolchain} xcode PARENT_SCOPE)
  else ()
    message(FATAL_ERROR "unsupported toolchain: ${CMAKE_GENERATOR}, ${CMAKE_C_COMPILER_ID}")
  endif ()
endfunction(get_app_toolchain)

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

# app running os
message(STATUS "detecting app running os")
get_app_os(TTCM_ENV_OS)
set(TTCM_ENV_OS ${TTCM_ENV_OS} CACHE INTERNAL "app running os")
message(STATUS "app running os: ${TTCM_ENV_OS}")

# app running cpu
message(STATUS "detecting app running cpu")
get_app_cpu(TTCM_ENV_CPU)
if (TTCM_IOS_SIMULATOR)
  set(TTCM_ENV_CPU x86)
endif ()
set(TTCM_ENV_CPU ${TTCM_ENV_CPU} CACHE INTERNAL "app running cpu")
message(STATUS "app running cpu: ${TTCM_ENV_CPU}")

# app building toolchain
message(STATUS "detecting app building toochain")
get_app_toolchain(TTCM_ENV_TOOLCHAIN)
set(TTCM_ENV_TOOLCHAIN ${TTCM_ENV_TOOLCHAIN} CACHE INTERNAL "app building toochain")
message(STATUS "app building toochain: ${TTCM_ENV_TOOLCHAIN}")

# app building host
message(STATUS "detecting app building host")
get_host_os(TTCM_ENV_HOST)
set(TTCM_ENV_HOST ${TTCM_ENV_HOST} CACHE INTERNAL "app building host")
message(STATUS "app building host: ${TTCM_ENV_HOST}")

# make env name
#if (TTCM_ENV_OS STREQUAL TTCM_ENV_HOST)
#  set(TTCM_ENV "${TTCM_ENV_OS}-${TTCM_ENV_CPU}-${TTCM_ENV_TOOLCHAIN}")
#else ()
#  set(TTCM_ENV "${TTCM_ENV_OS}-${TTCM_ENV_CPU}-${TTCM_ENV_TOOLCHAIN}--${TTCM_ENV_HOST}")
#endif ()
# the toolchain implies host platform, e.g. vs implies building on windows
#set(TTCM_ENV "${TTCM_ENV_OS}-${TTCM_ENV_CPU}-${TTCM_ENV_TOOLCHAIN}")
set(TTCM_ENV "${TTCM_ENV_OS}-${TTCM_ENV_TOOLCHAIN}")

message(STATUS "DETECTING ENVIRONMENT DONE: ${TTCM_ENV}")

#
# detect environment detail
#

# detect environment detail
function(detect_env_detail param_name param_val)  
  try_run(__run_result
          __compile_result 
          ${CMAKE_BINARY_DIR}
          ${TTCM_ROOT_PATH}/platform/host/${TTCM_ENV}_detector.c
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
endfunction(detect_env_detail)

macro(detect_env_detail_helper param description)
  if ("${${param}}" STREQUAL "")
    message(STATUS "detecting ${description}")
    detect_env_detail("${param}" __val)
    set(${param} ${__val} CACHE INTERNAL "${description}")
  endif ()
  message(STATUS "${description}: ${${param}}")
endmacro(detect_env_detail_helper)

# detecting detail depends on configuration
set(TTCM_ENV_DETECT_DETAIL 0 
    CACHE BOOL "detecting environment detail for optimization")
if (TTCM_ENV_DETECT_DETAIL)
  message(STATUS "DETECTING ENVIRONMENT DETAIL ...")

  # detect app running os detail
  detect_env_detail_helper(TTCM_ENV_OS_DETAIL "app running os detail")
  detect_env_detail_helper(TTCM_ENV_OS_VER_DETAIL "app running os version detail")
  detect_env_detail_helper(TTCM_ENV_OS_FEATURE_DETAIL "app running os feature detail")
  
  # detect app running cpu detail
  detect_env_detail_helper(TTCM_ENV_CPU_DETAIL "app running cpu detail")
  detect_env_detail_helper(TTCM_ENV_CPU_FEATURE_DETAIL "app running cpu feature detail")

  # detect toolchain
  detect_env_detail_helper(TTCM_ENV_TOOLCHAIN_DETAIL "app building toolchain detail")
  detect_env_detail_helper(TTCM_ENV_TOOLCHAIN_VER_DETAIL "app building toolchain version detail")
  
  # host inforamtion are for cmake usage, no need to pass to application
  
  message(STATUS "DETECTING ENVIRONMENT DETAIL DONE")
else (TTCM_ENV_DETECT_DETAIL)
  # should assign a numberical value
  set(TTCM_ENV_OS_DETAIL TS_ENV_UNKNOWN_VAL)
  set(TTCM_ENV_OS_VER_DETAIL TS_ENV_UNKNOWN_VAL)
  set(TTCM_ENV_OS_FEATURE_DETAIL TS_ENV_UNKNOWN_VAL)

  set(TTCM_ENV_CPU_DETAIL TS_ENV_UNKNOWN_VAL)
  set(TTCM_ENV_CPU_FEATURE_DETAIL TS_ENV_UNKNOWN_VAL)
    
  set(TTCM_ENV_TOOLCHAIN_DETAIL TS_ENV_UNKNOWN_VAL)
  set(TTCM_ENV_TOOLCHAIN_VER_DETAIL TS_ENV_UNKNOWN_VAL)    
endif (TTCM_ENV_DETECT_DETAIL)

#
# configuration
#

# generate header file
configure_file(${TTCM_ROOT_PATH}/platform/export/config/tt_environment_config.h.in
               ${TTCM_ROOT_PATH}/platform/export/config/tt_environment_config.h
               @ONLY)

