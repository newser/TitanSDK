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
# system header files
#

#
# compiler options
#

# disable c++ exception
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -fno-exceptions)

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
if (TTCM_ENV_DETECT_DETAIL)
  set(__march_flags -march=native)
else ()
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(__march_flags -march=x86-64)
  else ()
    set(__march_flags -march=i686)
  endif ()
endif ()
ttcm_choose_compiler_flag(CMAKE_C_FLAGS TRUE ${__march_flags})
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS TRUE ${__march_flags})

# always use -O2 rather than -O3 in release mode
#string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
#string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

# enable inline function
ttcm_choose_compiler_flag(CMAKE_C_FLAGS_RELEASE FALSE -finline-functions)
ttcm_choose_compiler_flag(CMAKE_CXX_FLAGS_RELEASE FALSE -finline-functions)

#if (CMAKE_BUILD_TYPE STREQUAL Debug)
# .cmake_choose_compiler_flag(CMAKE_C_FLAGS TRUE -g)
# .cmake_choose_compiler_flag(CMAKE_CXX_FLAGS TRUE -g)
#endif ()

#
# linker options
#

