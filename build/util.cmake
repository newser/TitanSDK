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

include(CheckCCompilerFlag)

# add source files under "dir" to "src_list" and group them as "group"
# @dir: source file directory
# @group: group name
# @src_list: source file list
function(add_group_src dir group src_list)
    # collect files
    aux_source_directory(${dir} FILE)

    # make group
    source_group(${group} FILES ${FILE})

    # add to source list
    set(${src_list} ${${src_list}} ${FILE} PARENT_SCOPE)
endfunction(add_group_src)

# add header files under "dir" to "hdr_list" and group them as "group"
# and these header files will be put under "framework_dir" if it's
# building mac framework
# @dir: source file directory
# @group: group name
# @framework_dir: where to put header file in a mac framework
# @hdr_list: header file list
function(add_group_hdr dir group hdr_list)
    # collect files
    file(GLOB FILE "${dir}/*.h")

    # make group
    source_group(${group} FILES ${FILE})

    # add to source list
    set(${hdr_list} ${${hdr_list}} ${FILE} PARENT_SCOPE)
endfunction(add_group_hdr)

# choose option from "option_list" and add to "compiler_flag"
#  @compiler_flag: choosed flag
#  @mandatory: if none of flags are valid, cmake fail
#  @option_list: flags to choose from
#  - former option has higher priority]
#  - do not add quotes around option list, use this function like
#    choose_compiler_flag(CMAKE_C_FLAGS TRUE -opt1 -opt2) but not "-opt1 -opt2"
function(choose_compiler_flag compiler_flag mandatory option_list)
    set(ok FALSE)
    foreach(option ${option_list})
        message(STATUS "option [${option}] in [${option_list}]")

        string(REGEX REPLACE "[-=]" "_" name ${option})
        check_c_compiler_flag(${option} has_${name})
        if (has_${name})
            set(${compiler_flag} "${${compiler_flag}} ${option}" PARENT_SCOPE)
            message(STATUS "adding compiler flag[${option}] to ${compiler_flag}")

            set(ok TRUE)
            break()
        else ()
            message(STATUS "compiler flag is not supported[${option}]")
        endif ()
    endforeach()

    if (${mandatory} AND NOT ok)
        message(FATAL_ERROR "all compiler flag are not supported[${option_list}]")
    endif ()
endfunction(choose_compiler_flag)

# set an option's value and then hide it
macro(hide name type value)
    set(${name} ${value} CACHE ${type} "hidden" FORCE)
    mark_as_advanced(${name})
endmacro(hide)
