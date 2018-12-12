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

#include <network/http/tt_http_server.h>

#include <misc/tt_assert.h>
#include <network/http/service/tt_http_inserv_file.h>
#include <network/http/service/tt_http_inserv_host.h>
#include <network/http/tt_http_server_connection.h>
#include <time/tt_timer.h>

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

static tt_result_t __sconn_skt_fiber(IN void *param);

static tt_result_t __sconn_add_default_serv(IN tt_http_sconn_t *c);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_server_create_skt(IN tt_http_server_t *svr,
                                      IN TO tt_skt_t *skt,
                                      IN OPT tt_http_server_attr_t *attr)
{
    tt_http_server_attr_t __attr;
    tt_fiber_t *f;

    TT_ASSERT(svr != NULL);

    if (attr == NULL) {
        tt_http_server_attr_default(&__attr);
        attr = &__attr;
    }

    svr->skt = skt;
    svr->ssl_server_cfg = NULL;
    tt_memcpy(&svr->new_skt_attr, &attr->new_skt_attr, sizeof(tt_skt_attr_t));
    tt_memcpy(&svr->conn_fiber_attr,
              &attr->conn_fiber_attr,
              sizeof(tt_fiber_attr_t));
    svr->https = TT_FALSE;

    return TT_SUCCESS;
}

tt_result_t tt_http_server_create_ssl(IN tt_http_server_t *svr,
                                      IN TO tt_ssl_t *ssl,
                                      IN tt_ssl_config_t *ssl_server_cfg,
                                      IN OPT tt_http_server_attr_t *attr)
{
    tt_http_server_attr_t __attr;
    tt_fiber_t *f;

    TT_ASSERT(svr != NULL);

    if (attr == NULL) {
        tt_http_server_attr_default(&__attr);
        attr = &__attr;
    }

    svr->ssl = ssl;
    svr->ssl_server_cfg = ssl_server_cfg;
    tt_memcpy(&svr->new_skt_attr, &attr->new_skt_attr, sizeof(tt_skt_attr_t));
    tt_memcpy(&svr->conn_fiber_attr,
              &attr->conn_fiber_attr,
              sizeof(tt_fiber_attr_t));
    svr->https = TT_TRUE;

    return TT_SUCCESS;
}

void tt_http_server_destroy(IN tt_http_server_t *svr)
{
    TT_ASSERT(svr != NULL);

    if (svr->https) {
        tt_ssl_destroy(svr->ssl);
    } else {
        tt_skt_destroy(svr->skt);
    }
}

void tt_http_server_attr_default(IN tt_http_server_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_skt_attr_default(&attr->new_skt_attr);

    tt_fiber_attr_default(&attr->conn_fiber_attr);
}

tt_result_t tt_http_server_run_fiber(IN tt_http_server_t *svr)
{
    tt_sktaddr_t addr;
    tt_skt_t *new_skt;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;
    tt_result_t result = TT_FAIL;

    while (TT_OK(tt_skt_accept(svr->skt,
                               &svr->new_skt_attr,
                               &addr,
                               &new_skt,
                               &fev,
                               &tmr))) {
        if (new_skt != NULL) {
            tt_fiber_t *f = tt_fiber_create(NULL,
                                            __sconn_skt_fiber,
                                            new_skt,
                                            &svr->conn_fiber_attr);
            if (f != NULL) {
                tt_fiber_resume(f, TT_FALSE);
            } else {
                tt_skt_destroy(new_skt);
            }
        }

        if (fev != NULL) {
            tt_fiber_finish(fev);
        }

        if (tmr != NULL) {
            // todo
        }
    }

    return result;
}

tt_result_t __sconn_skt_fiber(IN void *param)
{
    tt_skt_t *s = (tt_skt_t *)param;
    tt_http_sconn_t c;
    tt_http_sconn_attr_t attr;
    tt_http_inserv_t *is = NULL;

    tt_http_sconn_attr_default(&attr);

    if (!TT_OK(tt_http_sconn_create_skt(&c, s, &attr))) {
        tt_skt_destroy(s);
        return TT_FAIL;
    }

    if (!TT_OK(__sconn_add_default_serv(&c))) {
        tt_http_sconn_destroy(&c);
        return TT_FAIL;
    }

    if (tt_http_sconn_run(&c)) {
        tt_http_sconn_wait_eof(&c);
    }

    tt_http_sconn_destroy(&c);

    return TT_SUCCESS;
}

tt_result_t __sconn_add_default_serv(IN tt_http_sconn_t *c)
{
    tt_http_inserv_t *ins;

    // for loading host
    if (!TT_OK(tt_http_sconn_add_inserv(c, tt_g_http_inserv_host))) {
        return TT_FAIL;
    }

    // for basic file retrieving
    ins = tt_http_inserv_file_create(NULL);
    if (ins == NULL) {
        return TT_FAIL;
    }
    if (!TT_OK(tt_http_sconn_add_inserv(c, ins))) {
        tt_http_inserv_release(ins);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
