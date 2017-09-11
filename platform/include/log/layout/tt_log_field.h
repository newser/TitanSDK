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
@file tt_log_field.h
@brief log field

this file defines log field
*/

#ifndef __TT_LOG_FIELD__
#define __TT_LOG_FIELD__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <log/tt_log_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

typedef struct tt_logfld_s
{
    tt_lnode_t node;
    tt_logfld_type_t type;
    tt_char_t *format;
} tt_logfld_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// a log field must be reentrant
tt_export tt_logfld_t *tt_logfld_create(IN const tt_char_t *start,
                                        IN const tt_char_t *end);

tt_export void tt_logfld_destroy(IN tt_logfld_t *lf);

tt_export tt_result_t tt_logfld_check(IN const tt_char_t *start,
                                      IN const tt_char_t *end);

tt_export tt_result_t tt_logfld_output(IN tt_logfld_t *lf,
                                       IN tt_log_entry_t *entry,
                                       OUT struct tt_buf_s *outbuf);

#endif /* __TT_LOG_FIELD__ */
