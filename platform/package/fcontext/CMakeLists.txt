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

# collect files
set(SRC tt_fiber_wrapper.c)

if ((PLATFORM_ENV_OS STREQUAL macos) OR PLATFORM_IOS_SIMULATOR)
  enable_language(ASM)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SRC ${SRC} source/latest/jump_x86_64_sysv_macho_gas.S source/latest/make_x86_64_sysv_macho_gas.S)
  else ()
    set(SRC ${SRC} source/latest/jump_i386_sysv_macho_gas.S source/latest/make_i386_sysv_macho_gas.S)
  endif ()

elseif (PLATFORM_ENV_OS STREQUAL ios)
  enable_language(ASM)

  if (PLATFORM_ARM64)
    set(SRC ${SRC} source/latest/jump_arm64_aapcs_macho_gas.S source/latest/make_arm64_aapcs_macho_gas.S)
  else ()
    set(SRC ${SRC} source/latest/jump_arm_aapcs_macho_gas.S source/latest/make_arm_aapcs_macho_gas.S)
  endif ()

elseif (PLATFORM_ENV_OS STREQUAL linux)
  enable_language(ASM)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SRC ${SRC} source/latest/jump_x86_64_sysv_elf_gas.S source/latest/make_x86_64_sysv_elf_gas.S)
  else ()
    set(SRC ${SRC} source/latest/jump_i386_sysv_elf_gas.S source/latest/make_i386_sysv_elf_gas.S)
  endif ()

elseif (PLATFORM_ENV_OS STREQUAL windows)
  enable_language(ASM_MASM)

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SRC ${SRC} source/latest/jump_x86_64_ms_pe_masm.asm source/latest/make_x86_64_ms_pe_masm.asm )
  else ()
    set(SRC ${SRC} source/latest/jump_i386_ms_pe_masm.asm  source/latest/make_i386_ms_pe_masm.asm )
  endif ()

endif ()

# add to library
add_library(fcontext ${SRC})
