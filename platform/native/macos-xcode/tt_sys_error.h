/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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
@file tt_sys_error.h
@brief show system error information

APIs to show system error information
*/

#ifndef __TT_SYS_ERROR__
#define __TT_SYS_ERROR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log.h>
#include <log/tt_log_manager.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ERROR_NTV(...)                                                      \
    do {                                                                       \
        TT_ERROR(__VA_ARGS__);                                                 \
        tt_sys_err_show(__FUNCTION__);                                         \
    } while (0)

#define TT_NET_ERROR_NTV TT_ERROR_NTV

#define TT_ERROR_NTV_DUMP(ptr, owner, member, dump_func, dump_opt, ...)        \
    do {                                                                       \
        TT_ERROR(__VA_ARGS__);                                                 \
        tt_sys_err_show(__FUNCTION__);                                         \
        if (ptr != NULL) {                                                     \
            dump_func(TT_CONTAINER((ptr), owner, member), (dump_opt));         \
        }                                                                      \
    } while (0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_u32_t tt_get_sys_err()
{
    return errno;
}

extern void tt_sys_err_show(IN const tt_char_t *function);

#endif /* __TT_SYS_ERROR__ */
