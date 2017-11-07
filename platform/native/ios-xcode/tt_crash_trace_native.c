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

#include <algorithm/tt_buffer_format.h>
#include <init/tt_profile.h>
#include <misc/tt_backtrace.h>
#include <time/tt_date_format.h>

#define _XOPEN_SOURCE
#include <signal.h>
#include <ucontext.h>

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

static void __on_crash(IN int signum, IN struct __siginfo *info, IN void *p);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_crash_trace_component_init_ntv(IN tt_profile_t *profile)
{
    struct sigaction sa;

    sa.sa_sigaction = __on_crash;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

#ifdef TT_PLATFORM_ENABLE_BACKTRACE
    if (sigaction(SIGABRT, &sa, NULL) != 0) {
        TT_ERROR("fail to set action of SIGABRT");
    }
    if (sigaction(SIGSEGV, &sa, NULL) != 0) {
        TT_ERROR("fail to set action of SIGSEGV");
    }
    if (sigaction(SIGBUS, &sa, NULL) != 0) {
        TT_ERROR("fail to set action of SIGBUS");
    }
    if (sigaction(SIGILL, &sa, NULL) != 0) {
        TT_ERROR("fail to set action of SIGILL");
    }
    if (sigaction(SIGFPE, &sa, NULL) != 0) {
        TT_ERROR("fail to set action of SIGFPE");
    }
    if (sigaction(SIGPIPE, &sa, NULL) != 0) {
        TT_ERROR("fail to set action of SIGPIPE");
    }
#endif

    return TT_SUCCESS;
}

void __on_crash(IN int signum, IN siginfo_t *info, IN void *p)
{
    tt_date_t d;
    tt_char_t buf[32];
    const tt_char_t *sigstr;
    ucontext_t *uctx;
    tt_uintptr_t addr;

    TT_INFO("========================================");

    // crash time
    tt_date_now(&d);
    tt_memset(buf, 0, sizeof(buf));
    tt_date_render(&d, "%Y-%N-%D %H:%M:%S", buf, sizeof(buf) - 1);
    TT_INFO("crashed at %s", buf);

    // signal
    sigstr = strsignal(signum);
    if (sigstr == NULL) {
        tt_memset(buf, 0, sizeof(buf));
        tt_snprintf(buf, sizeof(buf) - 1, "%d", signum);
        sigstr = buf;
    }
    TT_INFO("%s", sigstr);

    // address
    uctx = (ucontext_t *)p;
#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    addr = uctx->uc_mcontext->__es.__faultvaddr;
#else
    addr = uctx->uc_mcontext->__es.__far;
#endif
    TT_INFO("fault address: %p", addr);

    // backtrace
    TT_INFO("backtrace:");
    TT_INFO("%s", tt_backtrace("    ", "\n"));
    TT_INFO("========================================");

    exit(signum);
}
