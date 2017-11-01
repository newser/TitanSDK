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

#include <log/io/tt_log_io_async.h>

#include <algorithm/tt_buffer_common.h>
#include <algorithm/tt_buffer_format.h>
#include <log/io/tt_log_io.h>
#include <os/tt_fiber_event.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __F_NAME "logio async"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __EV_EXIT,
    __EV_DATA,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __lio_async_destroy(IN tt_logio_t *lio);

static void __lio_async_output(IN tt_logio_t *lio,
                               IN const tt_char_t *data,
                               IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_async_itf = {
    TT_LOGIO_ASYNC,

    NULL,
    __lio_async_destroy,
    __lio_async_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __logio_async(IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_async_create(IN OPT tt_logio_async_attr_t *attr)
{
    tt_logio_async_attr_t __attr;
    tt_logio_t *lio;
    tt_logio_async_t *lio_async;
    tt_thread_t *t;
    tt_thread_log_t l;

    tt_u32_t __done = 0;
#define __LA_IOQ (1 << 0)
#define __LA_IBUF (1 << 1)
#define __LA_OBUF (1 << 2)
#define __LA_LOCK (1 << 3)
#define __LA_TASK (1 << 4)
#define __LA_TASK_RUN (1 << 5)

    if (attr == NULL) {
        tt_logio_async_attr_default(&__attr);
        attr = &__attr;
    }

    lio = tt_logio_create(sizeof(tt_logio_async_t), &tt_s_logio_async_itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_async = TT_LOGIO_CAST(lio, tt_logio_async_t);

    t = tt_current_thread();
    l = tt_thread_set_log(t, TT_THREAD_LOG_PRINTF);

    tt_ptrq_init(&lio_async->io_q, &attr->io_q_attr);
    __done |= __LA_IOQ;

    tt_buf_init(&lio_async->ibuf, &attr->ibuf_attr);
    __done |= __LA_IBUF;

    tt_buf_init(&lio_async->obuf, &attr->obuf_attr);
    __done |= __LA_OBUF;

    TT_DO_G(fail, tt_spinlock_create(&lio_async->lock, &attr->lock_attr));
    __done |= __LA_LOCK;

    TT_DO_G(fail, tt_task_create(&lio_async->task, &attr->task_attr));
    __done |= __LA_TASK;
    TT_DO_G(fail,
            tt_task_add_fiber(&lio_async->task,
                              __F_NAME,
                              __logio_async,
                              lio_async,
                              NULL));
    TT_DO_G(fail, tt_task_run(&lio_async->task));
    __done |= __LA_TASK_RUN;

    // find the io fiber
    TT_DONN_G(fail,
              lio_async->io_f = tt_task_find_fiber(&lio_async->task, __F_NAME));

    tt_thread_set_log(t, l);
    return lio;

fail:

    if (__done & __LA_TASK_RUN) {
        tt_task_exit(&lio_async->task);
    }

    if (__done & __LA_TASK) {
        tt_task_wait(&lio_async->task);
    }

    if (__done & __LA_LOCK) {
        tt_spinlock_destroy(&lio_async->lock);
    }

    if (__done & __LA_OBUF) {
        tt_buf_destroy(&lio_async->obuf);
    }

    if (__done & __LA_IBUF) {
        tt_buf_destroy(&lio_async->ibuf);
    }

    if (__done & __LA_IOQ) {
        tt_ptrq_destroy(&lio_async->io_q);
    }

    tt_free(lio);
    tt_thread_set_log(t, l);
    return NULL;
}

void tt_logio_async_attr_default(IN tt_logio_async_attr_t *attr)
{
    tt_ptrq_attr_default(&attr->io_q_attr);
    tt_buf_attr_default(&attr->ibuf_attr);
    tt_buf_attr_default(&attr->obuf_attr);
    tt_spinlock_attr_default(&attr->lock_attr);
    tt_task_attr_default(&attr->task_attr);
}

tt_result_t tt_logio_async_append(IN tt_logio_t *lio_async,
                                  IN TO tt_logio_t *lio)
{
    if (lio_async->itf->type != TT_LOGIO_ASYNC) {
        return TT_E_BADARG;
    }

    if (TT_OK(
            tt_ptrq_push_tail(&TT_LOGIO_CAST(lio_async, tt_logio_async_t)->io_q,
                              lio))) {
        tt_logio_ref(lio);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void __lio_async_destroy(IN tt_logio_t *lio)
{
    tt_logio_async_t *lio_async = TT_LOGIO_CAST(lio, tt_logio_async_t);
    tt_fiber_ev_t fev;
    tt_logio_t *l;

    tt_fiber_ev_init(&fev, __EV_EXIT);
    tt_fiber_send_ev(lio_async->io_f, &fev, TT_TRUE);
    tt_task_wait(&lio_async->task);

    while ((l = (tt_logio_t *)tt_ptrq_pop_head(&lio_async->io_q)) != NULL) {
        tt_logio_release(l);
    }

    tt_buf_destroy(&lio_async->ibuf);
    tt_buf_destroy(&lio_async->obuf);
    tt_spinlock_destroy(&lio_async->lock);
}

void __lio_async_output(IN tt_logio_t *lio,
                        IN const tt_char_t *data,
                        IN tt_u32_t data_len)
{
    tt_logio_async_t *lio_async = TT_LOGIO_CAST(lio, tt_logio_async_t);
    tt_buf_t *ibuf = &lio_async->ibuf;
    tt_fiber_ev_t *fev;

    // for distinguishing different entries, we push length and total log
    // content(including terminating null) into ibuf
    tt_spinlock_acquire(&lio_async->lock);
    tt_buf_put_u32(ibuf, data_len);
    tt_buf_put(ibuf, (tt_u8_t *)data, data_len + 1);
    tt_spinlock_release(&lio_async->lock);

    fev = tt_fiber_ev_create(__EV_DATA, 0);
    if (fev != NULL) {
        tt_fiber_send_ev(lio_async->io_f, fev, TT_FALSE);
    } else {
        tt_printf("log may be delayed: %s\n", data);
    }
}

tt_result_t __logio_async(IN void *param)
{
    tt_logio_async_t *lio_async = (tt_logio_async_t *)param;
    tt_fiber_t *f;
    tt_buf_t *obuf;
    tt_fiber_ev_t *fev;

    tt_thread_set_log(tt_current_thread(), TT_THREAD_LOG_PRINTF);

    f = tt_current_fiber();
    obuf = &lio_async->obuf;
    while ((fev = tt_fiber_recv_ev(f, TT_TRUE)) != NULL) {
        if (fev->ev == __EV_EXIT) {
            tt_fiber_finish(fev);
            break;
        }

        // __EV_DATA
        tt_spinlock_acquire(&lio_async->lock);
        tt_buf_swap(obuf, &lio_async->ibuf);
        tt_spinlock_release(&lio_async->lock);

        while (!tt_buf_empty(obuf)) {
            tt_u32_t len;
            tt_ptrq_iter_t iter;
            tt_logio_t *lio;

            if (!TT_OK(tt_buf_get_u32(obuf, &len)) ||
                (TT_BUF_RLEN(obuf) < (len + 1))) {
                tt_printf("inconsistent log\n");
                break;
            }

            tt_ptrq_iter(&lio_async->io_q, &iter);
            while ((lio = (tt_logio_t *)tt_ptrq_iter_next(&iter)) != NULL) {
                tt_logio_output(lio, (tt_char_t *)TT_BUF_RPOS(obuf), len);
            }

            tt_buf_inc_rp(obuf, len + 1);
        }
        tt_buf_clear(obuf);

        tt_fiber_finish(fev);
    }

    return TT_SUCCESS;
}
