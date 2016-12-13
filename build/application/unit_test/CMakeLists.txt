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

if (NOT DEFINED TSCM_PLATFORM_PATH)
    message(FATAL_ERROR "TSCM_PLATFORM_PATH undefined")
endif ()

#
# component
#

#
# header file path
#

# platform
include_directories(${TSCM_PLATFORM_PATH}/export)
include_directories(${TSCM_PLATFORM_PATH}/native/${TSCM_ENV})
include_directories(${TSCM_PLATFORM_PATH}/native/${TSCM_ENV_CPU})

#
# source file
#

set(SRC_FILE app_unit_test.c)
set(SRC_FILE ${SRC_FILE} app_unit_test_process.c)

#
# build
#

add_executable(app_unit_test ${SRC_FILE})

#
# link
#

# link platform module
target_link_libraries(app_unit_test platform)
add_dependencies(app_unit_test platform)

