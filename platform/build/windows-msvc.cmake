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
# system header files
#

#
# compiler options
#

#
# linker options
#

# windows libraries
set(TTCM_PLATFORM_DEP_LIB winmm.lib ws2_32.lib iphlpapi.lib PARENT_SCOPE)

# libraries required by platform
function(ttcm_platform_link_libraries)
  # multimedia
  target_link_libraries(platform winmm)
  
  # winsock2
  target_link_libraries(platform ws2_32)
  
  # network interface
  target_link_libraries(platform iphlpapi)

  if (TTCM_PLATFORM_SSL_ENABLE)
    # crypt
    target_link_libraries(platform crypt32)

    # crypt
    target_link_libraries(platform Ncrypt)
  endif ()

  if (TTCM_PLATFORM_CRYPTO_ENABLE)
    # crypt
    target_link_libraries(platform crypt32)

    # crypt
    target_link_libraries(platform Ncrypt)
  endif ()

endfunction(ttcm_platform_link_libraries)

# platform properties
function(ttcm_platform_set_properties)
  # define TS_PLATFORM_DYNAMIC_LIB_BUILDING during building
  set_target_properties(platform PROPERTIES
                        COMPILE_FLAGS -DTS_PLATFORM_DYNAMIC_LIB_BUILDING)

  # titansdk.lib/dll
  set_target_properties(platform PROPERTIES OUTPUT_NAME titansdk)

  # version
  #set_target_properties(platform PROPERTIES 
  #                      VERSION ${TTCM_VERSION_MAJOR}.${TTCM_VERSION_MINOR})
  set_target_properties(platform PROPERTIES 
                        LINK_FLAGS "/VERSION:\"${TTCM_VERSION_MAJOR}.${TTCM_VERSION_MINOR}\"")

endfunction(ttcm_platform_set_properties)