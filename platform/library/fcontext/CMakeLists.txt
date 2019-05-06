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
if (PLATFORM_ENV_OS STREQUAL windows)
    set(REL_SRC include/tt_fiber_windows_wrapper.h source/tt_fiber_windows_wrapper.c)

elseif ((PLATFORM_ENV_OS STREQUAL macos) OR PLATFORM_IOS_SIMULATOR)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(REL_SRC include/tt_fiber_wrapper.h
                    tt/native/fcontext/fcontext_boost.h
                    fcontext_boost.cpp
                    source/tt_fiber_wrapper.c
                    boost/jump_x86_64_sysv_macho_gas.S
                    boost/make_x86_64_sysv_macho_gas.S)
    else ()
        set(REL_SRC include/tt_fiber_wrapper.h
                    tt/native/fcontext/fiber_boost.h
                    fiber_boost.cpp
                    source/tt_fiber_wrapper.c
                    boost/jump_i386_sysv_macho_gas.S
                    boost/make_i386_sysv_macho_gas.S)
    endif ()

elseif (PLATFORM_ENV_OS STREQUAL ios)
    if (PLATFORM_ARM_ARCH STREQUAL arm64)
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_arm64_aapcs_macho_gas.S
                    boost/make_arm64_aapcs_macho_gas.S)
    else ()
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_arm_aapcs_macho_gas.S
                    boost/make_arm_aapcs_macho_gas.S)
    endif ()

elseif (PLATFORM_ENV_OS STREQUAL linux)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_x86_64_sysv_elf_gas.S
                    boost/make_x86_64_sysv_elf_gas.S)
    else ()
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_i386_sysv_elf_gas.S
                    boost/make_i386_sysv_elf_gas.S)
    endif ()

elseif (PLATFORM_ENV_OS STREQUAL android)
    if (CMAKE_ANDROID_ARCH_ABI STREQUAL x86_64)
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_x86_64_sysv_elf_gas.S
                    boost/make_x86_64_sysv_elf_gas.S)
    elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL x86)
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_i386_sysv_elf_gas.S
                    boost/make_i386_sysv_elf_gas.S)
	elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL armeabi OR
			CMAKE_ANDROID_ARCH_ABI STREQUAL armeabi-v7a)
	    set(REL_SRC include/tt_fiber_wrapper.h
	                source/tt_fiber_wrapper.c
                    boost/jump_arm_aapcs_elf_gas.S
	                boost/make_arm_aapcs_elf_gas.S)
	elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL arm64-v8a)
        set(REL_SRC include/tt_fiber_wrapper.h
                    source/tt_fiber_wrapper.c
                    boost/jump_arm64_aapcs_elf_gas.S
                    boost/make_arm64_aapcs_elf_gas.S)
    endif ()

endif ()

set(SRC)
foreach(f ${REL_SRC} ${HDR})
    list(APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/${f})
endforeach()

set(FCONTEXT_SRC ${SRC} PARENT_SCOPE)
