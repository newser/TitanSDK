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

static tt_result_t __http_svr_dispatch2fiber(IN void *param);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_server_create(IN tt_http_server_t *svr,
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
    tt_memcpy(&svr->accepted_skt_attr,
              &attr->accepted_skt_attr,
              sizeof(tt_skt_attr_t));

    return TT_SUCCESS;
}

void tt_http_server_destroy(IN tt_http_server_t *svr)
{
    TT_ASSERT(svr != NULL);

    tt_skt_destroy(svr->skt);
}

void tt_http_server_attr_default(IN tt_http_server_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_skt_attr_default(&attr->accepted_skt_attr);
}

tt_result_t __http_svr_dispatch2fiber(IN void *param)
{
    tt_http_server_t *svr = (tt_http_server_t *)param;
    tt_skt_t *server_skt = svr->skt, *client_skt;
    tt_sktaddr_t addr;
    tt_fiber_ev_t *fev;
    tt_tmr_t *tmr;

    /*
    while ((client_skt = tt_skt_accept(server_skt,
                                       &svr->accepted_skt_attr,
                                       &addr,
                                       &client_skt,
                                       &fev,
                                       &tmr)) != NULL) {
        if (fev != NULL) {
            tt_fiber_finish(fev);
        }

        if (tmr != NULL) {
            // todo
        }
    }
*/

    return TT_SUCCESS;
}
