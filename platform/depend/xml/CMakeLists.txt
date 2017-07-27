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
set(SRC pugixml/pugixml.cpp 
        pugixml/pugixml.hpp
        include/pugiconfig.hpp)

# add to library
if (PLATFORM_BUILD_SHARED)
  add_library(xml SHARED ${SRC})
else ()
  add_library(xml STATIC ${SRC})
endif ()

set_target_properties(xml PROPERTIES MACOSX_RPATH ON)

set_target_properties(xml PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/static
                      LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/shared
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLATFORM_OUTPATH}/debug/program)
set_target_properties(xml PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/static
                      LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/shared
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLATFORM_OUTPATH}/release/program)
