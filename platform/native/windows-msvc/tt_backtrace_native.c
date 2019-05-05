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
#include <init/tt_profile.h>

#include <tt_wchar.h>

#include <dbghelp.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BT_SIZE 128
#define __SYM_SIZE 256

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

tt_result_t tt_backtrace_component_init_ntv(IN tt_profile_t *profile)
{
#ifdef TT_PLATFORM_ENABLE_BACKTRACE
    SymInitialize(GetCurrentProcess(), NULL, TRUE);
    SymSetOptions(SYMOPT_LOAD_LINES);
#endif

    return TT_SUCCESS;
}

void tt_backtrace_component_exit_ntv()
{
#ifdef TT_PLATFORM_ENABLE_BACKTRACE
    SymCleanup(GetCurrentProcess());
#endif
}

tt_result_t tt_backtrace_ntv(IN tt_buf_t *buf, IN OPT const tt_char_t *prefix,
                             IN OPT const tt_char_t *suffix)
{
    tt_u32_t plen, slen, n, i;
    VOID *bt[__BT_SIZE] = {0};
    HANDLE proc;
    tt_result_t result = TT_E_NOMEM;

    if (prefix == NULL) { prefix = ""; }
    plen = (tt_u32_t)tt_strlen(prefix);

    if (suffix == NULL) { suffix = ""; }
    slen = (tt_u32_t)tt_strlen(suffix);

    n = CaptureStackBackTrace(0, __BT_SIZE, bt, NULL);
    if (n == 0) { return TT_FAIL; }
    proc = GetCurrentProcess();
    for (i = 0; i < n; ++i) {
        tt_u8_t tmp[sizeof(SYMBOL_INFO) + __SYM_SIZE + 1];
        PSYMBOL_INFO sym = (PSYMBOL_INFO)tmp;
        IMAGEHLP_LINEW64 line;
        DWORD64 addr_disp;
        DWORD disp2;

        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)prefix, plen));

        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = __SYM_SIZE;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (!SymFromAddr(proc, (DWORD64)bt[i], &addr_disp, sym)) {
            TT_DO_G(done, tt_buf_putf(buf, "#%d <%p> in ?", i, bt[i]));
        } else if (!SymGetLineFromAddrW64(proc, (DWORD64)bt[i], &disp2,
                                          &line)) {
            TT_DO_G(done, tt_buf_putf(buf, "#%d <%p> in %s(+0x%x)", i, bt[i],
                                      sym->Name, addr_disp));
        } else {
            tt_char_t *fname = tt_utf8_create(line.FileName, 0, NULL);
            if (fname != NULL) {
                tt_result_t result;
                result = tt_buf_putf(buf, "#%d <%p> in %s() at [%s:%d]", i,
                                     bt[i], sym->Name, fname, line.LineNumber);
                tt_free(fname);
                if (!TT_OK(result)) { goto done; }
            } else {
                TT_DO_G(done, tt_buf_putf(buf, "#%d <%p> in %s(+0x%x)", i,
                                          bt[i], sym->Name, addr_disp));
            }
        }

        TT_DO_G(done, tt_buf_put(buf, (tt_u8_t *)suffix, slen));
    }
    result = TT_SUCCESS;

done:
    return result;
}
