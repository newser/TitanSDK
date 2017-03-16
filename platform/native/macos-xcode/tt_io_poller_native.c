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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_io_poller_native.h>

#include <os/tt_fiber.h>

#include <tt_sys_error.h>
#include <tt_util_native.h>

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

// larger __KQ_EV_NUM could save time for calling kevent, but may
// descrease timer accuracy and vice vesa
#define __KQ_EV_NUM 1

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __POLLER_YIELD,
    __POLLER_EXIT,

    __POLLER_IO_NUM,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_io_handler_t __io_handler[TT_IO_NUM] = {
    NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_io_poller_create_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    int kq;
    struct kevent kev;

    kq = kqueue();
    if (kq < 0) {
        TT_ERROR_NTV("fail to create kqueue fd");
        return TT_FAIL;
    }

    EV_SET(&kev, TT_IO_POLLER, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, NULL);

again:
    if (kevent(kq, &kev, 1, NULL, 0, NULL) != 0) {
        if (errno == EINTR) {
            goto again;
        }

        TT_ERROR_NTV("fail to add rd thread ev mark");
        return TT_FAIL;
    }

    sys_iop->kq = kq;

    return TT_SUCCESS;
}

void tt_io_poller_destroy_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    __RETRY_IF_EINTR(close(sys_iop->kq) != 0);
}

tt_bool_t tt_io_poller_run_ntv(IN tt_io_poller_ntv_t *sys_iop,
                               IN tt_s64_t wait_ms)
{
    struct timespec t, *timeout;
    struct kevent kev[__KQ_EV_NUM];
    int nev;

    if (wait_ms != TT_TIME_INFINITE) {
        t.tv_sec = wait_ms / 1000;
        t.tv_nsec = (wait_ms % 1000) * 1000000;
        timeout = &t;
    } else {
        timeout = NULL;
    }

    nev = kevent(sys_iop->kq, NULL, 0, kev, __KQ_EV_NUM, timeout);
    if (nev > 0) {
        tt_u32_t i;
        for (i = 0; i < nev; ++i) {
            tt_u32_t io = kev[i].ident;
            if (io == TT_IO_POLLER) {
                tt_u32_t ev = (tt_u32_t)(tt_uintptr_t)kev[i].udata;
                if (ev == __POLLER_YIELD) {
                    tt_fiber_yield();
                } else if (ev == __POLLER_EXIT) {
                    return TT_FALSE;
                }
            } else {
                TT_ASSERT(TT_IO_VALID(io));
                __io_handler[io](kev[i].udata);
            }
        }
    } else if ((nev != 0) && (errno != EINTR)) {
        TT_ERROR_NTV("kevent fail");
        return TT_FALSE;
    }

    return TT_TRUE;
    ;
}

void tt_io_poller_yield_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    struct kevent kev;

    EV_SET(&kev, TT_IO_POLLER, EVFILT_USER, 0, NOTE_TRIGGER, 0, __POLLER_YIELD);

again:
    if (kevent(sys_iop->kq, &kev, 1, NULL, 0, NULL) != 0) {
        TT_ERROR_NTV("kevent failed");
        TT_ASSERT_ALWAYS(errno == EINTR);
        goto again;
    }
}

void tt_io_poller_exit_ntv(IN tt_io_poller_ntv_t *sys_iop)
{
    struct kevent kev;

    EV_SET(&kev, TT_IO_POLLER, EVFILT_USER, 0, NOTE_TRIGGER, 0, __POLLER_EXIT);

again:
    if (kevent(sys_iop->kq, &kev, 1, NULL, 0, NULL) != 0) {
        TT_ERROR_NTV("kevent failed");
        TT_ASSERT_ALWAYS(errno == EINTR);
        goto again;
    }
}
