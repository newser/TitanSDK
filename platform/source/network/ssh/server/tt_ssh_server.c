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

#include <network/ssh/server/tt_ssh_server.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <network/ssh/server/tt_ssh_server_conn.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SSH_SVR_BUF_SIZE (64 * 1024) // 64K

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

static tt_result_t __check_sshsvr_attr(IN tt_sshsvr_attr_t *attr);
static tt_result_t __check_sshsvr_cb(IN tt_sshsvr_cb_t *cb);
static tt_result_t __check_sshch_cb(IN tt_sshch_cb_t *cb);
static tt_result_t __sshsvr_start(IN tt_sshsvr_t *sshsvr);

static void __sshsvr_on_destroy(IN tt_skt_t *skt, IN void *cb_param);
void __sshsvr_destroy(IN tt_sshsvr_t *sshsvr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_sshsvr_t *tt_sshsvr_create(IN tt_sktaddr_t *address,
                              IN OPT tt_sshsvr_attr_t *attr,
                              IN OPT tt_sshsvr_cb_t *cb,
                              IN OPT void *opaque,
                              IN tt_sshch_cb_t *sshch_cb)
{
    tt_sshsvr_t *sshsvr = NULL;

    tt_result_t result;
    tt_skt_attr_t skt_attr;
    tt_skt_exit_t skt_exit;

    tt_u32_t __done = 0;
#define __SSVR_SKT (1 << 0)
#define __SSVR_MEM (1 << 1)

    TT_ASSERT(address != NULL);
    TT_ASSERT(sshch_cb != NULL);

    sshsvr = (tt_sshsvr_t *)tt_mem_alloc(sizeof(tt_sshsvr_t));
    if (sshsvr == NULL) {
        TT_ERROR("no mem for sshsvr");
        return NULL;
    }
    __done |= __SSVR_MEM;
    tt_memset(sshsvr, 0, sizeof(tt_sshsvr_t));

    // ssh ip address
    tt_memcpy(&sshsvr->address, address, sizeof(tt_sktaddr_t));

    // attributes
    if (attr != NULL) {
        if (!TT_OK(__check_sshsvr_attr(attr))) {
            goto sfail;
        }

        tt_memcpy(&sshsvr->attr, attr, sizeof(tt_sshsvr_attr_t));
    } else {
        tt_sshsvr_attr_default(&sshsvr->attr);
    }

    // server callbacks
    if (cb != NULL) {
        if (!TT_OK(__check_sshsvr_cb(cb))) {
            goto sfail;
        }

        tt_memcpy(&sshsvr->cb, cb, sizeof(tt_sshsvr_cb_t));
    } else {
        tt_sshsvr_cb_default(&sshsvr->cb);
    }

    // opaque data
    sshsvr->opaque = opaque;

    // channel callbacks
    if (!TT_OK(__check_sshch_cb(sshch_cb))) {
        goto sfail;
    }
    tt_memcpy(&sshsvr->sshch_cb, sshch_cb, sizeof(tt_sshch_cb_t));

    // connection list
    tt_list_init(&sshsvr->conn_list);

    sshsvr->skt_destroyed = TT_FALSE;

    // rsa key
    sshsvr->rsapub = NULL;
    sshsvr->rsapriv = NULL;

    // create async socket
    tt_skt_attr_default(&skt_attr);
    tt_skt_attr_set_nodelay(&skt_attr, TT_TRUE);
    tt_skt_attr_set_reuseaddr(&skt_attr, TT_TRUE);

    skt_exit.on_destroy = __sshsvr_on_destroy;
    skt_exit.cb_param = NULL;

    result = tt_tcp_server_async(&sshsvr->skt,
                                 tt_sktaddr_get_family(&sshsvr->address),
                                 &skt_attr,
                                 &sshsvr->address,
                                 TT_SKT_BACKLOG_DEFAULT,
                                 &skt_exit);
    if (!TT_OK(result)) {
        TT_ERROR("fail to create ssh server socket");
        goto sfail;
    }
    __done |= __SSVR_SKT;

    // start running
    result = __sshsvr_start(sshsvr);
    if (!TT_OK(result)) {
        goto sfail;
    }

    return sshsvr;

sfail:

    if (__done & __SSVR_SKT) {
        // it's safe to destroy skt brutely
        tt_async_skt_destroy(&sshsvr->skt, TT_TRUE);
    }

    if (__done & __SSVR_MEM) {
        tt_mem_free(sshsvr);
    }

    return NULL;
}

void tt_sshsvr_destroy(IN tt_sshsvr_t *sshsvr, IN tt_bool_t brute)
{
    TT_ASSERT(sshsvr != NULL);

    if (brute) {
        tt_lnode_t *node;

        while ((node = tt_list_pophead(&sshsvr->conn_list)) != NULL) {
            tt_sshsvrconn_destroy(TT_CONTAINER(node, tt_sshsvrconn_t, node),
                                  TT_TRUE);
        }

        tt_async_skt_destroy(&sshsvr->skt, TT_TRUE);

        // note callback is not invoked, as this is a rule that
        // when object is destroyed brutely, callback won't be
        // called, refer async socket

        // rsa key
        tt_sshsvr_destroy_rsa(sshsvr);

        tt_mem_free(sshsvr);
    } else {
        tt_lnode_t *node;

        node = tt_list_head(&sshsvr->conn_list);
        while (node != NULL) {
            tt_sshsvrconn_t *svrconn;

            svrconn = TT_CONTAINER(node, tt_sshsvrconn_t, node);
            node = node->next;

            tt_sshsvrconn_destroy(svrconn, TT_FALSE);
        }

        tt_async_skt_destroy(&sshsvr->skt, TT_FALSE);
    }
}

void tt_sshsvr_attr_default(IN tt_sshsvr_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->conn_buf_init_size = __SSH_SVR_BUF_SIZE;
    tt_buf_attr_default(&attr->conn_buf_attr);
    attr->max_conn_num = 0;
    attr->concurrent_conn_num = 1;
    attr->channel_per_conn = 8;

    // rsa key
    attr->rsapub_format = TT_RSA_FORMAT_NUM;
    tt_blob_init(&attr->rsapub_key_data);
    tt_rsa_attr_default(&attr->rsapub_attr);
    attr->rsapub_attr.padding.mode = TT_RSA_PADDING_PKCS1;

    attr->rsapriv_format = TT_RSA_FORMAT_NUM;
    tt_blob_init(&attr->rsapriv_key_data);
    tt_rsa_attr_default(&attr->rsapriv_attr);
    attr->rsapriv_attr.padding.mode = TT_RSA_PADDING_PKCS1;
}

void tt_sshsvr_cb_default(IN tt_sshsvr_cb_t *cb)
{
    cb->on_hello = NULL;
    cb->on_destroy = NULL;
}

tt_result_t tt_sshsvr_create_rsa(IN tt_sshsvr_t *sshsvr)
{
    tt_rsa_t *rsa;
    tt_sshsvr_attr_t *ssh_attr;

    TT_ASSERT(sshsvr != NULL);

    if (sshsvr->rsapub != NULL) {
        TT_ERROR("rsa keys has already been created");
        return TT_FAIL;
    }
    TT_ASSERT(sshsvr->rsapriv == NULL);

    ssh_attr = &sshsvr->attr;

    // rsa public
    rsa = (tt_rsa_t *)tt_mem_alloc(sizeof(tt_rsa_t));
    if (rsa == NULL) {
        TT_ERROR("no mem for ssh rsa");
        return TT_FAIL;
    }

    if (!TT_OK(tt_rsa_create(rsa,
                             ssh_attr->rsapub_format,
                             TT_RSA_TYPE_PUBLIC,
                             &ssh_attr->rsapub_key_data,
                             &ssh_attr->rsapub_attr))) {
        tt_mem_free(rsa);
        return TT_FAIL;
    }
    sshsvr->rsapub = rsa;

    // rsa private
    rsa = (tt_rsa_t *)tt_mem_alloc(sizeof(tt_rsa_t));
    if (rsa == NULL) {
        TT_ERROR("no mem for ssh rsa priv");

        tt_rsa_destroy(sshsvr->rsapub);
        tt_mem_free(sshsvr->rsapub);
        sshsvr->rsapub = NULL;
        return TT_FAIL;
    }

    if (!TT_OK(tt_rsa_create(rsa,
                             ssh_attr->rsapriv_format,
                             TT_RSA_TYPE_PRIVATE,
                             &ssh_attr->rsapriv_key_data,
                             &ssh_attr->rsapriv_attr))) {
        tt_mem_free(rsa);

        tt_rsa_destroy(sshsvr->rsapub);
        tt_mem_free(sshsvr->rsapub);
        sshsvr->rsapub = NULL;
        return TT_FAIL;
    }
    sshsvr->rsapriv = rsa;

    // todo: check rsa pub and priv

    return TT_SUCCESS;
}

void tt_sshsvr_destroy_rsa(IN tt_sshsvr_t *sshsvr)
{
    TT_ASSERT(sshsvr != NULL);

    if (sshsvr->rsapub != NULL) {
        tt_rsa_destroy(sshsvr->rsapub);
        tt_mem_free(sshsvr->rsapub);
        sshsvr->rsapub = NULL;
    }

    if (sshsvr->rsapriv != NULL) {
        tt_rsa_destroy(sshsvr->rsapriv);
        tt_mem_free(sshsvr->rsapriv);
        sshsvr->rsapriv = NULL;
    }
}

tt_result_t __check_sshsvr_attr(IN tt_sshsvr_attr_t *attr)
{
    if (attr->concurrent_conn_num == 0) {
        TT_ERROR("concurrent_conn_num can not be zero");
        return TT_FAIL;
    }

    if ((attr->max_conn_num != 0) &&
        (attr->concurrent_conn_num > attr->max_conn_num)) {
        TT_ERROR("conncurrent conn num[%d] should be less than limit[%d]",
                 attr->concurrent_conn_num,
                 attr->max_conn_num);
        return TT_FAIL;
    }

    if (attr->channel_per_conn == 0) {
        TT_ERROR("channel per conn can not be 0");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __check_sshsvr_cb(IN tt_sshsvr_cb_t *cb)
{
    return TT_SUCCESS;
}

tt_result_t __check_sshch_cb(IN tt_sshch_cb_t *cb)
{
    if (cb->on_recv == NULL) {
        TT_ERROR("ssh ch must have on_recv cb");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __sshsvr_start(IN tt_sshsvr_t *sshsvr)
{
    tt_u32_t i;
    tt_bool_t new_conn = TT_FALSE;

    for (i = 0; i < sshsvr->attr.concurrent_conn_num; ++i) {
        if (tt_sshsvrconn_create(sshsvr) != NULL) {
            new_conn = TT_TRUE;
        } else {
            TT_WARN("fail to create a new ssh server connection");
        }
    }

    if (new_conn) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void __sshsvr_on_destroy(IN tt_skt_t *skt, IN void *cb_param)
{
    tt_sshsvr_t *sshsvr = TT_CONTAINER(skt, tt_sshsvr_t, skt);

    // mark as destroyed
    sshsvr->skt_destroyed = TT_TRUE;

    if (tt_list_empty(&sshsvr->conn_list)) {
        __sshsvr_destroy(sshsvr);
        // do not access sshsvr any more
    }
}

void __sshsvr_destroy(IN tt_sshsvr_t *sshsvr)
{
    TT_ASSERT(tt_list_empty(&sshsvr->conn_list));
    TT_ASSERT(sshsvr->skt_destroyed);

    if (sshsvr->cb.on_destroy != NULL) {
        sshsvr->cb.on_destroy(sshsvr);
    }

    // rsa key
    tt_sshsvr_destroy_rsa(sshsvr);

    tt_mem_free(sshsvr);
}
