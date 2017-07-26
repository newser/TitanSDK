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

/**
@file tt_ssh_server.h
@brief ssh server
*/

#ifndef __TT_SSH_SERVER__
#define __TT_SSH_SERVER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_list.h>
#include <io/tt_socket.h>
#include <io/tt_socket_addr.h>
#include <network/ssh/tt_ssh_channel_cb.h>
#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sshsvr_s;

typedef tt_result_t (*tt_sshsvr_on_hello_t)(IN struct tt_sshsvr_s *sshsvr,
                                            IN tt_buf_t *buf);

typedef void (*tt_sshsvr_on_destroy_t)(IN struct tt_sshsvr_s *sshsvr);

typedef struct
{
    tt_sshsvr_on_hello_t on_hello;
    tt_sshsvr_on_destroy_t on_destroy;
} tt_sshsvr_cb_t;

typedef struct
{
    tt_u32_t conn_buf_init_size;
    tt_buf_attr_t conn_buf_attr;
    tt_u32_t max_conn_num;
    tt_u32_t concurrent_conn_num;
    tt_u32_t channel_per_conn;

    // rsa key
    // tt_rsa_format_t rsapub_format;
    tt_blob_t rsapub_key_data;
    // tt_rsa_attr_t rsapub_attr;

    // tt_rsa_format_t rsapriv_format;
    tt_blob_t rsapriv_key_data;
    // tt_rsa_attr_t rsapriv_attr;
} tt_sshsvr_attr_t;

typedef struct tt_sshsvr_s
{
    tt_sktaddr_t address;
    tt_sshsvr_attr_t attr;
    tt_sshsvr_cb_t cb;
    void *opaque;
    tt_sshch_cb_t sshch_cb;

    tt_list_t conn_list;
    tt_skt_t skt;

    tt_bool_t skt_destroyed : 1;

    // rsa key
    tt_rsa_t *rsapub;
    tt_rsa_t *rsapriv;
    // the reason that it takes both pub and priv here is that
    // some platform does not well support exporting private
    // key to public key numbers
} tt_sshsvr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_sshsvr_t *tt_sshsvr_create(IN tt_sktaddr_t *address,
                                     IN OPT tt_sshsvr_attr_t *attr,
                                     IN OPT tt_sshsvr_cb_t *cb,
                                     IN OPT void *opaque,
                                     IN tt_sshch_cb_t *sshch_cb);

// brute can only be set to true when sshsvr is created just now
tt_export void tt_sshsvr_destroy(IN tt_sshsvr_t *sshsvr, IN tt_bool_t brute);

tt_export void tt_sshsvr_attr_default(IN tt_sshsvr_attr_t *attr);

tt_export void tt_sshsvr_cb_default(IN tt_sshsvr_cb_t *cb);

// rsa key
tt_export tt_result_t tt_sshsvr_create_rsa(IN tt_sshsvr_t *sshsvr);

tt_export void tt_sshsvr_destroy_rsa(IN tt_sshsvr_t *sshsvr);

tt_inline tt_rsa_t *tt_sshsvr_get_rsapub(IN tt_sshsvr_t *sshsvr)
{
    if (sshsvr->rsapub == NULL) {
        tt_sshsvr_create_rsa(sshsvr);
    }
    return sshsvr->rsapub;
}

tt_inline tt_rsa_t *tt_sshsvr_get_rsapriv(IN tt_sshsvr_t *sshsvr)
{
    if (sshsvr->rsapriv == NULL) {
        tt_sshsvr_create_rsa(sshsvr);
    }
    return sshsvr->rsapriv;
}

#endif /* __TT_SSH_SERVER__ */
