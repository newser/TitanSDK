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
# definition
#

set(DNS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/c-ares)

macro(hide name type value)
  set(${name} ${value} CACHE ${type} "hidden" FORCE)
  mark_as_advanced(${name})
endmacro(hide)

#
# customization
#

# lib type depends on platform configuration
hide(CARES_STATIC BOOL ON)
hide(CARES_SHARED BOOL OFF)
hide(CARES_INSTALL BOOL OFF)
hide(CARES_STATIC_PIC BOOL OFF)

# customize configuration
add_definitions(-DHAVE_CONFIG_H)

add_subdirectory(${DNS_PATH})

#
# result
#

set(DNS_LIB c-ares PARENT_SCOPE)
