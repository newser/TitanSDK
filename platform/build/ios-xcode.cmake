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

set(PLATFORM_DEP_LIB iconv PARENT_SCOPE)

# libraries required by platform
function(ttcm_platform_link_libraries)
  # charset
  target_link_libraries(platform iconv)

  # as it may switch between ios and ios simulator, we need do
  # finding path every time

  # security
  if (PLATFORM_SSL_ENABLE)
    target_link_libraries(platform "-framework Security")
  endif()

  # crypto
  if (PLATFORM_CRYPTO_ENABLE)
    target_link_libraries(platform "-framework Security")
  endif()

  # core foundation
  target_link_libraries(platform "-framework CoreFoundation")
  
endfunction(ttcm_platform_link_libraries)

# platform properties
function(ttcm_platform_set_properties)
  if (PLATFORM_BUILD_DYNAMIC)
    # TitanSDK.framework
    set_target_properties(platform PROPERTIES FRAMEWORK TRUE)
    set_target_properties(platform PROPERTIES OUTPUT_NAME TitanSDK)
  else ()
    # libtitansdk.a
    set_target_properties(platform PROPERTIES OUTPUT_NAME titansdk)    
  endif()

  # version
  set_target_properties(platform PROPERTIES 
                        VERSION ${PLATFORM_VERSION_MAJOR}.${PLATFORM_VERSION_MINOR})

endfunction(ttcm_platform_set_properties)
