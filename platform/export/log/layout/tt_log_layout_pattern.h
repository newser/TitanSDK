/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_log_layout_pattern.h
@brief log layout pattern

this file defines log layout pattern
*/

#ifndef __TT_LOG_LAYOUT_PATTERN__
#define __TT_LOG_LAYOUT_PATTERN__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/layout/tt_log_layout.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_loglyt_t *tt_loglyt_pattern_create(IN OPT const tt_char_t *logger,
                                             IN const tt_char_t *pattern);

#endif /* __TT_LOG_LAYOUT_PATTERN__ */
