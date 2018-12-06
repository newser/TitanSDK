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
    TT_ASSERT(sm != NULL);

    sm->owner = NULL;
    tt_dlist_init(&sm->inserv);
    tt_dlist_init(&sm->outserv);
    sm->discarding = TT_FALSE;
}

void tt_http_svcmgr_destroy(IN tt_http_svcmgr_t *sm)
{
    tt_dnode_t *dn;

    TT_ASSERT(sm != NULL);

    while ((dn = tt_dlist_pop_head(&sm->inserv)) != NULL) {
        tt_http_inserv_release(TT_CONTAINER(dn, tt_http_inserv_t, dnode));
    }

    while ((dn = tt_dlist_pop_head(&sm->outserv)) != NULL) {
        tt_http_outserv_release(TT_CONTAINER(dn, tt_http_outserv_t, dnode));
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

    sm->discarding = TT_FALSE;
}

tt_http_inserv_action_t tt_http_svcmgr_on_uri(IN tt_http_svcmgr_t *sm,
                                              IN tt_http_parser_t *req,
                                              OUT tt_http_resp_render_t *resp)
{
    tt_dnode_t *dn;
    tt_http_inserv_action_t act = TT_HTTP_INSERV_ACT_IGNORE;

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    dn = tt_dlist_head(&sm->inserv);
    while (dn != NULL) {
        tt_http_inserv_t *s = TT_CONTAINER(dn, tt_http_inserv_t, dnode);
        tt_http_inserv_action_t this_act;

        dn = dn->next;

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
    tt_dnode_t *dn;
    tt_http_inserv_action_t act = TT_HTTP_INSERV_ACT_IGNORE;

    if (sm->discarding) {
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    dn = tt_dlist_head(&sm->inserv);
    while (dn != NULL) {
        tt_http_inserv_t *s = TT_CONTAINER(dn, tt_http_inserv_t, dnode);
        tt_http_inserv_action_t this_act;

        dn = dn->next;

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
        // if reqeust is completed and discarded, check if resp status is set
        if (!TT_HTTP_STATUS_VALID(tt_http_resp_render_get_status(resp))) {
            tt_http_resp_render_set_status(
                resp, TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        }
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (sm->owner == NULL) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);

        sm->discarding = TT_TRUE;
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    return tt_http_inserv_on_complete(sm->owner, req, resp);
}

void tt_http_svcmgr_on_resp(IN tt_http_svcmgr_t *sm,
                            IN tt_http_parser_t *req,
                            IN OUT tt_http_resp_render_t *resp)
{
    tt_dnode_t *dn = tt_dlist_head(&sm->outserv);
    while (dn != NULL) {
        tt_http_outserv_on_resp(TT_CONTAINER(dn, tt_http_outserv_t, dnode),
                                req,
                                resp);
        dn = dn->next;
    }
}
