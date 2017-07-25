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

#set(PLATFORM_DEP_LIB pthread rt numa PARENT_SCOPE)

# libraries required by platform
function(ttcm_platform_link_libraries)
  # pthread
  target_link_libraries(platform pthread)

  # realtime
  target_link_libraries(platform rt)

  # numa support
  if (PLATFORM_ENABLE_NUMA)
    target_link_libraries(platform numa)
  endif ()

  target_link_libraries(platform dl)

endfunction(ttcm_platform_link_libraries)

# platform properties
function(ttcm_platform_set_properties)
  # libtitansdk.a/so
  set_target_properties(platform PROPERTIES OUTPUT_NAME titansdk)

endfunction(ttcm_platform_set_properties)

