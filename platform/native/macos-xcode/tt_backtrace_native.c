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

#if defined(TT_HAVE_LIBUNWIND)
#include <libunwind.h>
#elif defined(TT_HAVE_BACKTRACE)
#include <execinfo.h>
#endif

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BT_SIZE 128

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

#if defined(TT_HAVE_LIBUNWIND)

tt_result_t tt_backtrace_ntv(IN tt_buf_t *buf,
                             IN OPT const tt_char_t *prefix,
                             IN OPT const tt_char_t *suffix)
{
    tt_u32_t plen, slen, i;
    unw_context_t ctx;
    unw_cursor_t cur;

    if (prefix == NULL) {
        prefix = "";
    }
    plen = (tt_u32_t)tt_strlen(prefix);

    if (suffix == NULL) {
        suffix = "";
    }
    slen = (tt_u32_t)tt_strlen(suffix);

    if ((unw_getcontext(&ctx) != 0) || (unw_init_local(&cur, &ctx) != 0)) {
        return TT_FAIL;
    }

    i = 0;
    while (unw_step(&cur) > 0) {
        unw_word_t ip, off;
        char name[128] = {0};

        if ((unw_get_reg(&cur, UNW_REG_IP, &ip) != 0) ||
            (unw_get_proc_name(&cur, name, sizeof(name) - 1, &off)) != 0) {
            break;
        }

        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)prefix, plen));
        TT_DO_G(done, tt_buf_putf(buf, "%3d %p %s(+0x%x)", i++, ip, name, off));
        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)suffix, slen));
    }
done:
    return TT_SUCCESS;
}

#elif defined(TT_HAVE_BACKTRACE)

tt_result_t tt_backtrace_ntv(IN tt_buf_t *buf,
                             IN OPT const tt_char_t *prefix,
                             IN OPT const tt_char_t *suffix)
{
    tt_u32_t plen, slen, n, i;
    void *addr[__BT_SIZE];
    char **sym;
    tt_result_t result = TT_E_NOMEM;

    if (prefix == NULL) {
        prefix = "";
    }
    plen = (tt_u32_t)tt_strlen(prefix);

    if (suffix == NULL) {
        suffix = "";
    }
    slen = (tt_u32_t)tt_strlen(suffix);

    if (((n = backtrace(addr, __BT_SIZE)) == 0) ||
        ((sym = backtrace_symbols(addr, n)) == NULL)) {
        return TT_FAIL;
    }
    for (i = 0; i < n; ++i) {
        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)prefix, plen));
        TT_DO_G(done, tt_buf_put_cstr(buf, sym[i]));
        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)suffix, slen));
    }
    result = TT_SUCCESS;

done:
    free(sym);
    return result;
}

#else

tt_result_t tt_backtrace_ntv(IN tt_buf_t *buf,
                             IN OPT const tt_char_t *prefix,
                             IN OPT const tt_char_t *suffix)
{
    return TT_FAIL;
}

#endif
