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
@file tt_log_manager.h
@brief log manager

this file defines log manager
*/

#ifndef __TT_LOG_MANAGER__
#define __TT_LOG_MANAGER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log_field.h>
#include <log/tt_log_format.h>
#include <log/tt_log_io.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_spinlock_s;

typedef struct tt_logmgr_attr_s
{
    tt_u32_t reserved;
} tt_logmgr_attr_t;

typedef struct tt_logmgr_s
{
    tt_logmgr_attr_t attr;

    tt_logfmt_t *lfmt[TT_LOG_LEVEL_NUM];
} tt_logmgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_logmgr_create(IN tt_logmgr_t *lmgr,
                                    IN OPT tt_logmgr_attr_t *attr);

extern void tt_logmgr_destroy(IN tt_logmgr_t *lmgr);

extern void tt_logmgr_attr_default(IN tt_logmgr_attr_t *attr);

// set level to TT_LOG_LEVEL_DETAIL to enable all, or TT_LOG_LEVEL_NUM
// to disable all
extern void tt_logmgr_enable(IN tt_logmgr_t *lmgr, IN tt_log_level_t level);

extern tt_result_t tt_logmgr_create_format(IN tt_logmgr_t *lmgr,
                                           IN tt_log_level_t level,
                                           IN const tt_char_t *pattern,
                                           IN OPT tt_u32_t buf_size,
                                           IN OPT const tt_char_t *logger);

extern tt_result_t tt_logmgr_add_io(IN tt_logmgr_t *lmgr,
                                    IN tt_log_level_t level,
                                    IN struct tt_logio_s *lio);

#endif /* __TT_LOG_MANAGER__ */
