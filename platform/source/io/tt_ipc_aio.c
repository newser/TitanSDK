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

#include <io/tt_ipc_aio.h>

#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

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

tt_result_t tt_async_ipc_create(IN tt_ipc_t *ipc,
                                IN OPT const tt_char_t *local_addr,
                                IN OPT tt_ipc_attr_t *attr,
                                IN tt_ipc_exit_t *ipc_exit)
{
    TT_ASSERT(ipc != NULL);
    TT_ASSERT(ipc_exit != NULL);
    TT_ASSERT(ipc_exit->on_destroy != NULL);

    if (attr != NULL) {
        tt_memcpy(&ipc->attr, attr, sizeof(tt_ipc_attr_t));
    } else {
        tt_ipc_attr_default(&ipc->attr);
    }

    if (local_addr != NULL) {
        ipc->listening = TT_TRUE;
    }

    return tt_async_ipc_create_ntv(&ipc->sys_ipc,
                                   local_addr,
                                   &ipc->attr,
                                   ipc_exit);
}

void tt_async_ipc_destroy(IN tt_ipc_t *ipc, IN tt_bool_t immediate)
{
    TT_ASSERT(ipc != NULL);

    tt_async_ipc_destroy_ntv(&ipc->sys_ipc, immediate);
}

void tt_ipc_attr_default(IN tt_ipc_attr_t *attr)
{
    // use 8K by default
    attr->recv_buf_size = 1 << 13;

    attr->from_alloc = TT_FALSE;
}

tt_result_t tt_ipc_connect_async(IN tt_ipc_t *ipc,
                                 IN const tt_char_t *remote_addr,
                                 IN tt_ipc_on_connect_t on_connect,
                                 IN OPT void *cb_param)
{
    TT_ASSERT(ipc != NULL);
    TT_ASSERT(remote_addr != NULL);
    TT_ASSERT(on_connect != NULL);

    if (ipc->listening) {
        TT_ERROR("can not connect on a listen ipc");
        return TT_BAD_PARAM;
    }

    return tt_ipc_connect_async_ntv(&ipc->sys_ipc,
                                    remote_addr,
                                    on_connect,
                                    cb_param);
}

tt_result_t tt_ipc_accept_async(IN tt_ipc_t *listening_ipc,
                                IN tt_ipc_t *new_ipc,
                                IN OPT tt_ipc_attr_t *new_ipc_attr,
                                IN tt_ipc_exit_t *new_ipc_exit,
                                IN tt_ipc_on_accept_t on_accept,
                                IN OPT void *cb_param)
{
    TT_ASSERT(listening_ipc != NULL);
    TT_ASSERT(new_ipc != NULL);
    TT_ASSERT(new_ipc_exit != NULL);
    TT_ASSERT(new_ipc_exit->on_destroy != NULL);
    TT_ASSERT(on_accept != NULL);

    if (!listening_ipc->listening) {
        TT_ERROR("not a listen ipc");
        return TT_BAD_PARAM;
    }

    return tt_ipc_accept_async_ntv(&listening_ipc->sys_ipc,
                                   new_ipc,
                                   new_ipc_attr,
                                   new_ipc_exit,
                                   on_accept,
                                   cb_param);
}

tt_result_t tt_ipc_send_async(IN tt_ipc_t *ipc,
                              IN tt_ev_t *pev,
                              IN tt_ipc_on_send_t on_send,
                              IN OPT void *cb_param)
{
    TT_ASSERT(ipc != NULL);
    TT_ASSERT(pev != NULL);
    TT_ASSERT(on_send != NULL);

    if (ipc->listening) {
        TT_ERROR("can not send on a listen ipc");
        return TT_BAD_PARAM;
    }

    return tt_ipc_send_async_ntv(&ipc->sys_ipc, pev, on_send, cb_param);
}

tt_result_t tt_ipc_recv_async(IN tt_ipc_t *ipc,
                              IN tt_ipc_on_recv_t on_recv,
                              IN OPT void *cb_param)
{
    TT_ASSERT(ipc != NULL);
    TT_ASSERT(on_recv != NULL);

    if (ipc->listening) {
        TT_ERROR("can not recv on a listen ipc");
        return TT_BAD_PARAM;
    }

    return tt_ipc_recv_async_ntv(&ipc->sys_ipc, on_recv, cb_param);
}
