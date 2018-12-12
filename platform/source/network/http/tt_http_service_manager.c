/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain act copy of the License at
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

#include <network/http/tt_http_service_manager.h>

#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>

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

void tt_http_svcmgr_init(IN tt_http_svcmgr_t *sm)
{
    tt_u32_t i;

    TT_ASSERT(sm != NULL);

    sm->owner = NULL;

    for (i = 0; i < TT_HTTP_INLINE_INSERV_NUM; ++i) {
        sm->inline_inserv[i] = NULL;
    }
    sm->inserv = sm->inline_inserv;
    sm->inserv_num = 0;
    sm->inserv_max = TT_HTTP_INLINE_INSERV_NUM;

    for (i = 0; i < TT_HTTP_INLINE_OUTSERV_NUM; ++i) {
        sm->inline_outserv[i] = NULL;
    }
    sm->outserv = sm->inline_outserv;
    sm->outserv_num = 0;
    sm->outserv_max = TT_HTTP_INLINE_OUTSERV_NUM;

    sm->discarding = TT_FALSE;
}

void tt_http_svcmgr_destroy(IN tt_http_svcmgr_t *sm)
{
    tt_u32_t i;

    TT_ASSERT(sm != NULL);

    for (i = 0; i < sm->inserv_num; ++i) {
        tt_http_inserv_release(sm->inserv[i]);
    }
    if (sm->inserv != sm->inline_inserv) {
        tt_free(sm->inserv);
    }

    for (i = 0; i < sm->outserv_num; ++i) {
        tt_http_outserv_release(sm->outserv[i]);
    }
    if (sm->outserv != sm->inline_outserv) {
        tt_free(sm->outserv);
    }
}

void tt_http_svcmgr_clear(IN tt_http_svcmgr_t *sm)
{
    tt_u32_t i;

    TT_ASSERT(sm != NULL);

    sm->owner = NULL;

    for (i = 0; i < sm->inserv_num; ++i) {
        tt_http_inserv_clear(sm->inserv[i]);
    }

    for (i = 0; i < sm->outserv_num; ++i) {
        tt_http_outserv_clear(sm->outserv[i]);
    }

    sm->discarding = TT_FALSE;
}

tt_result_t tt_http_svcmgr_add_inserv(IN tt_http_svcmgr_t *sm,
                                      IN TO tt_http_inserv_t *s)
{
    if (sm->inserv_num == sm->inserv_max) {
        tt_u32_t new_num = sm->inserv_num + TT_HTTP_INLINE_INSERV_NUM;
        tt_http_inserv_t **s = tt_malloc(sizeof(tt_http_inserv_t *) * new_num);
        if (s == NULL) {
            TT_ERROR("fail to expand inserv array");
            return TT_E_NOMEM;
        }
        tt_memcpy(s, sm->inserv, sizeof(tt_http_inserv_t *) * sm->inserv_num);

        if (sm->inserv != sm->inline_inserv) {
            tt_free(sm->inserv);
        }
        sm->inserv = s;
        sm->inserv_max = new_num;
    }

    sm->inserv[sm->inserv_num++] = s;
    tt_http_inserv_ref(s);
    return TT_SUCCESS;
}

tt_result_t tt_http_svcmgr_add_outserv(IN tt_http_svcmgr_t *sm,
                                       IN TO tt_http_outserv_t *s)
{
    if (sm->outserv_num == sm->outserv_max) {
        tt_u32_t new_num = sm->outserv_num + TT_HTTP_INLINE_OUTSERV_NUM;
        tt_http_outserv_t **s =
            tt_malloc(sizeof(tt_http_outserv_t *) * new_num);
        if (s == NULL) {
            TT_ERROR("fail to expand outserv array");
            return TT_E_NOMEM;
        }
        tt_memcpy(s,
                  sm->outserv,
                  sizeof(tt_http_outserv_t *) * sm->outserv_num);

        if (sm->outserv != sm->inline_outserv) {
            tt_free(sm->outserv);
        }
        sm->outserv = s;
        sm->outserv_max = new_num;
    }

    sm->outserv[sm->outserv_num++] = s;
    tt_http_outserv_ref(s);
    return TT_SUCCESS;
}

tt_http_inserv_action_t tt_http_svcmgr_on_uri(IN tt_http_svcmgr_t *sm,
                                              IN tt_http_parser_t *req,
                                              OUT tt_http_resp_render_t *resp)
{
    tt_u32_t i;
    tt_http_inserv_action_t act = TT_HTTP_INSERV_ACT_PASS;

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    for (i = 0; i < sm->inserv_num; ++i) {
        tt_http_inserv_t *s = sm->inserv[i];
        tt_http_inserv_action_t this_act;

        this_act = tt_http_inserv_on_uri(s, req, resp);
        if (this_act <= TT_HTTP_INSERV_ACT_SHUTDOWN) {
            // if anyone says close/shutdown, no need to do more things,
            // call should close/shutdown connection
            return this_act;
        } else if (this_act == TT_HTTP_INSERV_ACT_DISCARD) {
            sm->discarding = TT_TRUE;
            return this_act;
        } else if (this_act == TT_HTTP_INSERV_ACT_OWNER) {
            if (sm->owner == NULL) {
                // can not overwrite previous service which is of higher
                // priority
                sm->owner = s;
            }
            act = TT_HTTP_INSERV_ACT_OWNER;
        }
    }

    return act;
}

tt_http_inserv_action_t tt_http_svcmgr_on_header(
    IN tt_http_svcmgr_t *sm,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp)
{
    tt_u32_t i;
    tt_http_inserv_action_t act = TT_HTTP_INSERV_ACT_PASS;

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    for (i = 0; i < sm->inserv_num; ++i) {
        tt_http_inserv_t *s = sm->inserv[i];
        tt_http_inserv_action_t this_act;

        this_act = tt_http_inserv_on_header(s, req, resp);
        if (this_act <= TT_HTTP_INSERV_ACT_SHUTDOWN) {
            return this_act;
        } else if (this_act == TT_HTTP_INSERV_ACT_DISCARD) {
            sm->discarding = TT_TRUE;
            return this_act;
        } else if (this_act == TT_HTTP_INSERV_ACT_OWNER) {
            if (sm->owner == NULL) {
                sm->owner = s;
            }
            act = TT_HTTP_INSERV_ACT_OWNER;
        }
    }

    return act;
}

tt_http_inserv_action_t tt_http_svcmgr_on_body(IN tt_http_svcmgr_t *sm,
                                               IN tt_http_parser_t *req,
                                               OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_action_t act;

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (sm->owner == NULL) {
        // this is unexpected as there should be at least a default service
        // handling request that all other services are not interested
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);

        sm->discarding = TT_TRUE;
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    act = tt_http_inserv_on_body(sm->owner, req, resp);
    if (act == TT_HTTP_INSERV_ACT_DISCARD) {
        sm->discarding = TT_TRUE;
    }
    return act;
}

tt_http_inserv_action_t tt_http_svcmgr_on_trailing(
    IN tt_http_svcmgr_t *sm,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_action_t act;

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (sm->owner == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);

        sm->discarding = TT_TRUE;
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    act = tt_http_inserv_on_trailing(sm->owner, req, resp);
    if (act == TT_HTTP_INSERV_ACT_DISCARD) {
        sm->discarding = TT_TRUE;
    }
    return act;
}

tt_http_inserv_action_t tt_http_svcmgr_on_complete(
    IN tt_http_svcmgr_t *sm,
    IN tt_http_parser_t *req,
    OUT tt_http_resp_render_t *resp)
{
    if (sm->discarding) {
        // if request is completed and discarded, check if resp status is set
        if (!TT_HTTP_STATUS_VALID(tt_http_resp_render_get_status(resp))) {
            tt_http_resp_render_set_status(
                resp, TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        }
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (sm->owner == NULL) {
        // 404, 500?
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);

        sm->discarding = TT_TRUE;
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    return tt_http_inserv_on_complete(sm->owner, req, resp);
}

tt_http_inserv_action_t tt_http_svcmgr_get_body(IN tt_http_svcmgr_t *sm,
                                                IN tt_http_parser_t *req,
                                                IN tt_http_resp_render_t *resp,
                                                OUT struct tt_buf_s *buf)
{
    // no need to set response status

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (sm->owner == NULL) {
        sm->discarding = TT_TRUE;
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    return tt_http_inserv_get_body(sm->owner, req, resp, buf);
}

tt_result_t tt_http_svcmgr_on_resp_header(IN tt_http_svcmgr_t *sm,
                                          IN tt_http_parser_t *req,
                                          IN OUT tt_http_resp_render_t *resp)
{
    tt_u32_t i;

    for (i = 0; i < sm->outserv_num; ++i) {
        tt_result_t result =
            tt_http_outserv_on_header(sm->outserv[i], req, resp);
        if (!TT_OK(result)) {
            return result;
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_http_svcmgr_on_resp_body(IN tt_http_svcmgr_t *sm,
                                        IN tt_http_parser_t *req,
                                        IN OUT tt_http_resp_render_t *resp,
                                        IN OUT struct tt_buf_s *input,
                                        OUT struct tt_buf_s **output)
{
    tt_u32_t i;

    // copy input if there is no outserv
    *output = input;

    for (i = 0; i < sm->outserv_num; ++i) {
        tt_result_t result;

        input = *output;
        result =
            tt_http_outserv_on_body(sm->outserv[i], req, resp, input, output);
        if (!TT_OK(result)) {
            return result;
        }
    }

    return TT_SUCCESS;
}
