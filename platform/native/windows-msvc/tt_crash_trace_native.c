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

#include <tt_crash_trace_native.h>

#include <init/tt_profile.h>
#include <misc/tt_backtrace.h>
#include <time/tt_date_format.h>

#include <tt_wchar.h>

#include <dbghelp.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

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

static LONG CALLBACK __on_crash(IN PEXCEPTION_POINTERS ExceptionInfo);

static const tt_char_t *__ecode2str(IN DWORD ExceptionCode);

static void __addr2sym(IN tt_uintptr_t addr,
                       IN tt_char_t *buf,
                       IN tt_u32_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_crash_trace_component_init_ntv(IN tt_profile_t *profile)
{
#ifdef TT_PLATFORM_ENABLE_BACKTRACE
    if (AddVectoredExceptionHandler(1, __on_crash) == NULL) {
        TT_ERROR("fail to add exception handler");
    }
#endif

    return TT_SUCCESS;
}

LONG CALLBACK __on_crash(IN PEXCEPTION_POINTERS ExceptionInfo)
{
    tt_date_t d;
    tt_char_t buf[256];
    const tt_char_t *ecstr;
    tt_uintptr_t addr;

    TT_INFO("========================================");

    // crash time
    tt_date_now(&d);
    tt_memset(buf, 0, sizeof(buf));
    tt_date_render(&d, "%Y-%N-%D %H:%M:%S", buf, sizeof(buf) - 1);
    TT_INFO("crashed at %s", buf);

    // signal
    ecstr = __ecode2str(ExceptionInfo->ExceptionRecord->ExceptionCode);
    if (ecstr == NULL) {
        tt_memset(buf, 0, sizeof(buf));
        tt_snprintf(buf,
                    sizeof(buf) - 1,
                    "%d",
                    ExceptionInfo->ExceptionRecord->ExceptionCode);
        ecstr = buf;
    }
    TT_INFO("%s", ecstr);

// address
#if TT_ENV_IS_64BIT
    addr = ExceptionInfo->ContextRecord->Rip;
#else
    addr = ExceptionInfo->ContextRecord->Eip;
#endif
    tt_memset(buf, 0, sizeof(buf));
    __addr2sym(addr, buf, sizeof(buf) - 1);
    TT_INFO("fault address: %s", buf);

    // backtrace
    TT_INFO("backtrace:");
    TT_INFO("%s", tt_backtrace("    ", "\n"));
    TT_INFO("========================================");

    return EXCEPTION_CONTINUE_SEARCH;
}

const tt_char_t *__ecode2str(IN DWORD ExceptionCode)
{
#define __SE(e)                                                                \
    case e:                                                                    \
        return #e
    switch (ExceptionCode) {
        __SE(EXCEPTION_ACCESS_VIOLATION);
        __SE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
        __SE(EXCEPTION_BREAKPOINT);
        __SE(EXCEPTION_DATATYPE_MISALIGNMENT);
        __SE(EXCEPTION_FLT_DENORMAL_OPERAND);
        __SE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
        __SE(EXCEPTION_FLT_INEXACT_RESULT);
        __SE(EXCEPTION_FLT_INVALID_OPERATION);
        __SE(EXCEPTION_FLT_OVERFLOW);
        __SE(EXCEPTION_FLT_STACK_CHECK);
        __SE(EXCEPTION_FLT_UNDERFLOW);
        __SE(EXCEPTION_ILLEGAL_INSTRUCTION);
        __SE(EXCEPTION_IN_PAGE_ERROR);
        __SE(EXCEPTION_INT_DIVIDE_BY_ZERO);
        __SE(EXCEPTION_INT_OVERFLOW);
        __SE(EXCEPTION_INVALID_DISPOSITION);
        __SE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
        __SE(EXCEPTION_PRIV_INSTRUCTION);
        __SE(EXCEPTION_SINGLE_STEP);
        __SE(EXCEPTION_STACK_OVERFLOW);
        default:
            return NULL;
    }
#undef __SE
}

void __addr2sym(IN tt_uintptr_t addr, IN tt_char_t *buf, IN tt_u32_t len)
{
    HANDLE proc;
    tt_u8_t tmp[sizeof(SYMBOL_INFO) + __SYM_SIZE + 1];
    PSYMBOL_INFO sym = (PSYMBOL_INFO)tmp;
    IMAGEHLP_LINEW64 line;
    DWORD64 addr_disp;
    DWORD disp2;
    tt_char_t *fname;

    proc = GetCurrentProcess();

    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = __SYM_SIZE;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    if (!SymFromAddr(proc, addr, &addr_disp, sym)) {
        tt_snprintf(buf, len - 1, "%p", addr);
    } else if (!SymGetLineFromAddrW64(proc, addr, &disp2, &line) ||
               ((fname = tt_utf8_create(line.FileName, NULL)) == NULL)) {
        tt_snprintf(buf,
                    len - 1,
                    "%p in %s(+0x%x)",
                    addr,
                    sym->Name,
                    addr_disp);
    } else {
        tt_snprintf(buf,
                    len - 1,
                    "%p in %s() at [%s:%d]",
                    addr,
                    sym->Name,
                    fname,
                    line.LineNumber);
        tt_free(fname);
    }
}
