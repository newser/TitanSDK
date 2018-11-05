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
set(REL_SRC http-parser/http_parser.h
            http-parser/http_parser.c)

set(SRC)
foreach(f ${REL_SRC})
    list(APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/${f})
endforeach()

set(HTTP_SRC ${SRC} PARENT_SCOPE)
