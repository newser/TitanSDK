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
# system header files
#

#
# compiler options
#

#
# linker options
#

# libraries required by platform
function(platform_link_libraries)
    # multimedia
    target_link_libraries(platform winmm)
    
    # winsock2
    target_link_libraries(platform ws2_32)
    
    # network interface
    target_link_libraries(platform iphlpapi)

    # process
    target_link_libraries(platform psapi)

endfunction(platform_link_libraries)

# platform properties
function(platform_set_properties)
    if (PLATFORM_BUILD_SHARED)
        target_compile_definitions(platform PRIVATE "-DTT_PLATFORM_BUILD_SHARED_EXPORT")
    endif ()

endfunction(platform_set_properties)
