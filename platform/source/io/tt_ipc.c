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

#include <io/tt_ipc.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_atomic.h>
#include <os/tt_thread.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_atomic_s32_t __ipc_num;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __ipc_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static void __ipc_component_exit(IN tt_component_t *comp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_ipc_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __ipc_component_init, __ipc_component_exit,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_IPC, "IPC", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void tt_ipc_status_dump(IN tt_u32_t flag)
{
    if (flag & TT_IPC_STATUS_COUNT) {
        tt_printf("%s[%d ipc] are opened\n",
                  TT_COND(flag & TT_IPC_STATUS_PREFIX, "<<IPC>> ", ""),
                  tt_atomic_s32_get(&__ipc_num));
    }

    if (flag & TT_IPC_STATUS_NATIVE) {
        tt_ipc_status_dump_ntv(flag);
    }
}

tt_ipc_t *tt_ipc_create(IN OPT const tt_char_t *addr,
                        IN OPT tt_ipc_attr_t *attr)
{
    tt_ipc_t *ipc;
    tt_ipc_attr_t __attr;

    ipc = tt_malloc(sizeof(tt_ipc_t));
    if (ipc == NULL) {
        TT_ERROR("no mem for ipc");
        return NULL;
    }

    if (attr == NULL) {
        tt_ipc_attr_default(&__attr);
        attr = &__attr;
    }

    if (!TT_OK(tt_ipc_create_ntv(&ipc->sys_ipc, addr, attr))) {
        tt_free(ipc);
        return NULL;
    }

    tt_buf_init(&ipc->buf, &attr->recv_buf_attr);

    tt_atomic_s32_inc(&__ipc_num);
    return ipc;
}

void tt_ipc_destroy(IN tt_ipc_t *ipc)
{
    tt_s32_t n;

    TT_ASSERT(ipc != NULL);

    tt_ipc_destroy_ntv(&ipc->sys_ipc);

    tt_buf_destroy(&ipc->buf);

    tt_free(ipc);

    n = tt_atomic_s32_dec(&__ipc_num);
    TT_ASSERT(n >= 0);
}

void tt_ipc_attr_default(IN tt_ipc_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_buf_attr_default(&attr->recv_buf_attr);
    attr->recv_buf_attr.min_extend = (1 << 13); // 8K
    attr->recv_buf_attr.max_extend = (1 << 17); // 128K
}

tt_result_t tt_ipc_connect(IN tt_ipc_t *ipc, IN const tt_char_t *addr)
{
    TT_ASSERT(ipc != NULL);
    TT_ASSERT(addr != NULL);

    return tt_ipc_connect_ntv(&ipc->sys_ipc, addr);
}

tt_result_t tt_ipc_connect_retry(IN tt_ipc_t *ipc,
                                 IN const tt_char_t *addr,
                                 IN tt_u32_t interval_ms,
                                 IN tt_u32_t retry_count)
{
    tt_u32_t n;
    tt_result_t result;

    TT_ASSERT(ipc != NULL);
    TT_ASSERT(addr != NULL);

    n = 0;
    while (!TT_OK(result = tt_ipc_connect_ntv(&ipc->sys_ipc, addr)) &&
           (++n < retry_count)) {
        tt_sleep(interval_ms);
    }

    return result;
}

tt_result_t tt_ipc_accept(IN tt_ipc_t *ipc,
                          IN OPT tt_ipc_attr_t *new_attr,
                          OUT tt_ipc_t **new_ipc,
                          OUT tt_fiber_ev_t **p_fev,
                          OUT struct tt_tmr_s **p_tmr)
{
    tt_ipc_attr_t __attr;
    tt_result_t result;

    TT_ASSERT(ipc != NULL);

    if (new_attr == NULL) {
        tt_ipc_attr_default(&__attr);
        new_attr = &__attr;
    }

    result = tt_ipc_accept_ntv(&ipc->sys_ipc, new_attr, new_ipc, p_fev, p_tmr);
    if (TT_OK(result) && (*new_ipc != NULL)) {
        tt_atomic_s32_inc(&__ipc_num);
    }
    return result;
}

tt_result_t tt_ipc_local_addr(IN tt_ipc_t *ipc,
                              OUT OPT tt_char_t *addr,
                              IN tt_u32_t size,
                              OUT OPT tt_u32_t *len)
{
    TT_ASSERT(ipc != NULL);

    return tt_ipc_local_addr_ntv(&ipc->sys_ipc, addr, size, len);
}

tt_result_t tt_ipc_remote_addr(IN tt_ipc_t *ipc,
                               OUT OPT tt_char_t *addr,
                               IN tt_u32_t size,
                               OUT OPT tt_u32_t *len)
{
    TT_ASSERT(ipc != NULL);

    return tt_ipc_remote_addr_ntv(&ipc->sys_ipc, addr, size, len);
}

tt_result_t __ipc_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    // init low level socket system
    if (!TT_OK(tt_ipc_component_init_ntv(profile))) {
        TT_ERROR("fail to initialize ipc system native");
        return TT_FAIL;
    }

    tt_atomic_s32_set(&__ipc_num, 0);

    return TT_SUCCESS;
}

void __ipc_component_exit(IN tt_component_t *comp)
{
    tt_ipc_component_exit_ntv();

    tt_ipc_status_dump(TT_IPC_STATUS_ALL);
}
