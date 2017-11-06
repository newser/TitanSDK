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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_backtrace_native.h>

#include <algorithm/tt_buffer_format.h>

#include <dlfcn.h>
#include <unwind.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BT_SIZE 128

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct __unwind_param_s
{
    uintptr_t addr[__BT_SIZE];
    tt_u32_t num;
} __unwind_param_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static _Unwind_Reason_Code __unwind_cb(struct _Unwind_Context *ctx,
                                       void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_backtrace_ntv(IN tt_buf_t *buf,
                             IN OPT const tt_char_t *prefix,
                             IN OPT const tt_char_t *suffix)
{
    tt_u32_t plen, slen, i;
    __unwind_param_t up;
    tt_result_t result = TT_E_NOMEM;

    if (prefix == NULL) {
        prefix = "";
    }
    plen = (tt_u32_t)tt_strlen(prefix);

    if (suffix == NULL) {
        suffix = "";
    }
    slen = (tt_u32_t)tt_strlen(suffix);

    tt_memset(&up, 0, sizeof(__unwind_param_t));
    _Unwind_Backtrace(__unwind_cb, &up);
    for (i = 0; i < up.num; ++i) {
        Dl_info di;
        const char *name;

        if (dladdr((void *)up.addr[i], &di) && (di.dli_sname != NULL)) {
            name = di.dli_sname;
        } else {
            name = "?";
        }

        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)prefix, plen));
        TT_DO_G(done, tt_buf_putf(buf, "%3d %p %s", i, up.addr[i], name));
        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)suffix, slen));
    }
    result = TT_SUCCESS;

done:
    return result;
}

_Unwind_Reason_Code __unwind_cb(struct _Unwind_Context *ctx, void *param)
{
    __unwind_param_t *up = (__unwind_param_t *)param;
    uintptr_t ip = _Unwind_GetIP(ctx);
    if (ip != 0) {
        if (up->num < __BT_SIZE) {
            up->addr[up->num] = ip;
            up->num += 1;
        } else {
            return _URC_END_OF_STACK;
        }
    }
    return _URC_NO_REASON;
}