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

include(${TTCM_BUILD_PATH}/util.cmake)

#
# xcode specific
#

set(TTCM_IOS_SIMULATOR 0 CACHE BOOL "build for iphone simulator")
set(TTCM_IOS_SDK_VERSION Latest CACHE STRING "ios sdk version, set to 0 to use latest")

# xcode path
set(__xcode_path)
execute_process(COMMAND xcode-select -print-path
                OUTPUT_VARIABLE __xcode_path
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if (NOT EXISTS "${__xcode_path}")
  message(FATAL_ERROR "not found xcode path")
endif()
message(STATUS "xcode path: ${__xcode_path}")

# sdk path
set(__ios_dev_root)
set(__ios_sdk_root)
set(__ios_sdk_ver 0.0)
if (TTCM_IOS_SIMULATOR)
  set(__ios_dev_root "${__xcode_path}/Platforms/iPhoneSimulator.platform/Developer")

  if ("${TTCM_IOS_SDK_VERSION}" STREQUAL "Latest")
    file(GLOB __ios_installed_sdk ${__ios_dev_root}/SDKs/*)
    foreach(__file "${__ios_installed_sdk}")
      string(REGEX MATCH "[0-9]+\\.[0-9]+" __sdk_ver ${__file})
      if (__sdk_ver VERSION_GREATER __ios_sdk_ver)
        set(__ios_sdk_ver ${__sdk_ver})
      endif ()
    endforeach()
  else()
    set(__ios_sdk_ver ${TTCM_IOS_SDK_VERSION})
  endif()
  set(__ios_sdk_root "${__ios_dev_root}/SDKs/iPhoneSimulator${__ios_sdk_ver}.sdk")

  # without setting CMAKE_XCODE_EFFECTIVE_PLATFORMS, generated project can not
  # locate built library correctly
  set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
  
else ()
  set(__ios_dev_root "${__xcode_path}/Platforms/iPhoneOS.platform/Developer")

  if ("${TTCM_IOS_SDK_VERSION}" STREQUAL "Latest")
    file(GLOB __ios_installed_sdk ${__ios_dev_root}/SDKs/*)
    foreach(__file "${__ios_installed_sdk}")
      string(REGEX MATCH "[0-9]+\\.[0-9]+" __sdk_ver ${__file})
      if (__sdk_ver VERSION_GREATER __ios_sdk_ver)
        set(__ios_sdk_ver ${__sdk_ver})
      endif ()
    endforeach()
  else()
    set(__ios_sdk_ver ${TTCM_IOS_SDK_VERSION})
  endif()
  set(__ios_sdk_root "${__ios_dev_root}/SDKs/iPhoneOS${__ios_sdk_ver}.sdk")

  set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")

endif()
message(STATUS "dev root: ${__ios_dev_root}")
message(STATUS "sdk root: ${__ios_sdk_root}")
message(STATUS "sdk version: ${__ios_sdk_ver}")
if(NOT EXISTS "${__ios_sdk_root}")
  message(FATAL_ERROR "not found sdk root: ${__ios_sdk_root}")
endif ()

# paths
set(CMAKE_OSX_SYSROOT "${__ios_sdk_root}")

set(CMAKE_FIND_ROOT_PATH "${__ios_sdk_root}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# arch
set(__ios_arch)
if (TTCM_IOS_SIMULATOR)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
    #set(CMAKE_OSX_ARCHITECTURES i386)  
else ()

  if (${__ios_sdk_ver} VERSION_EQUAL "5.0" OR ${__ios_sdk_ver} VERSION_GREATER "5.0")
    set(CMAKE_OSX_ARCHITECTURES armv6 armv7)
    set(__ios_arch "-arch armv6 -arch armv7")
  endif()

  if (${__ios_sdk_ver} VERSION_EQUAL "6.0" OR ${__ios_sdk_ver} VERSION_GREATER "6.0")
    set(CMAKE_OSX_ARCHITECTURES armv7 armv7s)
    set(__ios_arch "-arch armv7 -arch armv7s")
  endif()

  if (${__ios_sdk_ver} VERSION_EQUAL "7.0" OR ${__ios_sdk_ver} VERSION_GREATER "7.0")
    set(CMAKE_OSX_ARCHITECTURES armv7 armv7s arm64)
    set(__ios_arch "-arch armv7 -arch armv7s -arch arm64")
  endif()

endif()

#
# system header files
#

#
# compiler options
#

# disable c++ exception and rtti
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -fno-exceptions)
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -fno-rtti)

# warn setting
ttcm_choose_compiler_flag(CMAKE_C_FLAGS FALSE -Wall)
ttcm_choose_compiler_flag(CMAKE_C_FLAGS FALSE -Werror)
ttcm_choose_compiler_flag(CMAKE_C_FLAGS FALSE -Wno-unused-function)
ttcm_choose_compiler_flag(CMAKE_C_FLAGS FALSE -Wno-unused-variable)

ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Wall)
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Werror)
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Wno-unused-function)
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Wno-unused-variable)

# atomic instrucitons need specify -march
#if (TTCM_ENV_DETECT_DETAIL)
#  set(__march_flags -march=native)
#else ()
#  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
#    set(__march_flags -march=x86-64)
#  else ()
#    set(__march_flags -march=i686)
#  endif ()
#endif ()
.cmake_choose_compiler_flag(CMAKE_C_FLAGS TRUE ${__march_flags})
.cmake_choose_compiler_flag(CMAKE_CXX_FLAGS TRUE ${__march_flags})

# always use -O2 rather than -O3 in release mode
#string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
#string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

# enable inline function
ttcm_choose_compiler_flag(CMAKE_C_FLAGS_RELEASE FALSE -finline-functions)
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS_RELEASE FALSE -finline-functions)

#
# linker options
#