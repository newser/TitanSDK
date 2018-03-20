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

include(${BUILD_PATH}/util.cmake)

#
# system header files
#

#
# compiler options
#

# disable c++ exception
choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -fno-exceptions)

# warn setting
choose_compiler_flag(CMAKE_C_FLAGS FALSE -Wall)
#choose_compiler_flag(CMAKE_C_FLAGS FALSE -Werror)
choose_compiler_flag(CMAKE_C_FLAGS FALSE -Wno-unused-function)
choose_compiler_flag(CMAKE_C_FLAGS FALSE -Wno-unused-variable)

choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Wall)
choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Werror)
choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Wno-unused-function)
choose_compiler_flag(CMAKE_CXX_FLAGS FALSE -Wno-unused-variable)

# always use -O2 rather than -O3 in release mode
#string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
#string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

# enable inline function
choose_compiler_flag(CMAKE_C_FLAGS_RELEASE FALSE -finline-functions)
choose_compiler_flag(CMAKE_CXX_FLAGS_RELEASE FALSE -finline-functions)

#if (CMAKE_BUILD_TYPE STREQUAL Debug)
# .cmake_choose_compiler_flag(CMAKE_C_FLAGS TRUE -g)
# .cmake_choose_compiler_flag(CMAKE_CXX_FLAGS TRUE -g)
#endif ()

# more debug info
if (SANITIZE_ADDRESS)
    choose_compiler_flag(CMAKE_C_FLAGS_DEBUG FALSE -fno-omit-frame-pointer)
    choose_compiler_flag(CMAKE_CXX_FLAGS_DEBUG FALSE -fno-omit-frame-pointer)

    choose_compiler_flag(CMAKE_C_FLAGS_DEBUG FALSE -fno-optimize-sibling-calls)
    choose_compiler_flag(CMAKE_CXX_FLAGS_DEBUG FALSE -fno-optimize-sibling-calls)

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
endif ()

#
# linker options
#
