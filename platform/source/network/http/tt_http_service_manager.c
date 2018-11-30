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

#include <network/http/tt_http_service_manager.h>

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
    TT_ASSERT(sm != NULL);

    sm->owner = NULL;
    tt_dlist_init(&sm->inserv);
    tt_dlist_init(&sm->outserv);
}

void tt_http_svcmgr_destroy(IN tt_http_svcmgr_t *sm)
{
    tt_dnode_t *dn;

    TT_ASSERT(sm != NULL);

    while ((dn = tt_dlist_pop_head(&sm->inserv)) != NULL) {
        tt_http_inserv_destroy(TT_CONTAINER(dn, tt_http_inserv_t, dnode));
    }

    while ((dn = tt_dlist_pop_head(&sm->outserv)) != NULL) {
        tt_http_outserv_destroy(TT_CONTAINER(dn, tt_http_outserv_t, dnode));
    }
}

void tt_http_svcmgr_clear(IN tt_http_svcmgr_t *sm)
{
    tt_dnode_t *dn;

    TT_ASSERT(sm != NULL);

    sm->owner = NULL;

    dn = tt_dlist_head(&sm->inserv);
    while (dn != NULL) {
        tt_http_inserv_clear(TT_CONTAINER(dn, tt_http_inserv_t, dnode));
        dn = dn->next;
    }

    dn = tt_dlist_head(&sm->outserv);
    while (dn != NULL) {
        tt_http_outserv_clear(TT_CONTAINER(dn, tt_http_outserv_t, dnode));
        dn = dn->next;
    }
}

tt_http_inserv_action_t tt_http_svcmgr_on_uri(IN tt_http_svcmgr_t *sm,
                                              IN struct tt_http_parser_s *req,
                                              OUT tt_http_resp_render_t *resp)
{
    tt_dnode_t *dn;
    tt_http_inserv_action_t a;

    dn = tt_dlist_head(&sm->inserv);
    while (dn != NULL) {
        tt_http_inserv_t *s = TT_CONTAINER(dn, tt_http_inserv_t, dnode);

        dn = dn->next;

        a = tt_http_inserv_on_uri(s, req, resp);
        if (a <= TT_HTTP_INSERV_ACT_DISCARD) {
            // if anyone says close/shutdown/discard, no need to do more things
            return a;
        } else if (a == TT_HTTP_INSERV_ACT_OWNER) {
            TT_ASSERT(sm->owner == NULL);
            sm->owner = s;
        }
    }

    return TT_HTTP_INSERV_ACT_IGNORE;
}

tt_http_inserv_action_t tt_http_svcmgr_on_header(
    IN tt_http_svcmgr_t *sm,
    IN struct tt_http_parser_s *req,
    OUT tt_http_resp_render_t *resp)
{
    tt_dnode_t *dn;
    tt_http_inserv_action_t a;

    dn = tt_dlist_head(&sm->inserv);
    while (dn != NULL) {
        tt_http_inserv_t *s = TT_CONTAINER(dn, tt_http_inserv_t, dnode);

        dn = dn->next;

        a = tt_http_inserv_on_uri(s, req, resp);
        if (a <= TT_HTTP_INSERV_ACT_DISCARD) {
            return a;
        } else if (a == TT_HTTP_INSERV_ACT_OWNER) {
            if (sm->owner == NULL) {
                sm->owner = s;
            } else if (sm->owner != s) {
                TT_WARN("http request owner has been set");
            }
        }
    }

    return TT_HTTP_INSERV_ACT_IGNORE;
}

tt_http_inserv_action_t tt_http_svcmgr_on_body(IN tt_http_svcmgr_t *sm,
                                               IN struct tt_http_parser_s *req,
                                               OUT tt_http_resp_render_t *resp)
{
    if (sm->owner != NULL) {
        return tt_http_inserv_on_body(sm->owner, req, resp);
    } else {
        // send 500 resp and discard following data
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }
}

tt_http_inserv_action_t tt_http_svcmgr_on_trailing(
    IN tt_http_svcmgr_t *sm,
    IN struct tt_http_parser_s *req,
    OUT tt_http_resp_render_t *resp)
{
    if (sm->owner != NULL) {
        return tt_http_inserv_on_trailing(sm->owner, req, resp);
    } else {
        // send 500 resp and discard following data
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }
}

tt_http_inserv_action_t tt_http_svcmgr_on_complete(
    IN tt_http_svcmgr_t *sm,
    IN struct tt_http_parser_s *req,
    OUT tt_http_resp_render_t *resp)
{
    if (sm->owner != NULL) {
        return tt_http_inserv_on_complete(sm->owner, req, resp);
    } else {
        // send 500 resp and discard following data
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }
}

void tt_http_svcmgr_on_resp(IN tt_http_svcmgr_t *sm,
                            IN struct tt_http_parser_s *req,
                            IN OUT tt_http_resp_render_t *resp)
{
    tt_dnode_t *dn = tt_dlist_head(&sm->inserv);
    while (dn != NULL) {
        tt_http_outserv_on_resp(TT_CONTAINER(dn, tt_http_outserv_t, dnode),
                                req,
                                resp);
        dn = dn->next;
    }
}
