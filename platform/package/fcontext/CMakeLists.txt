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

enable_language(ASM)

# collect files
set(SRC tt_fiber_wrapper.c)

if (TTCM_ENV_OS STREQUAL macos)
  set(SRC ${SRC} source/latest/jump_x86_64_sysv_macho_gas.S source/latest/make_x86_64_sysv_macho_gas.S)
elseif (TTCM_ENV_OS STREQUAL linux)
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SRC ${SRC} source/latest/jump_x86_64_sysv_elf_gas.S source/latest/make_x86_64_sysv_elf_gas.S)
  else ()
    set(SRC ${SRC} source/latest/jump_i386_sysv_elf_gas.S source/latest/make_i386_sysv_elf_gas.S)
  endif ()
endif ()

# add to library
add_library(fcontext ${SRC})
