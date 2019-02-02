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

/**
@file tt_http_service_manager.h
@brief http service manager

this file defines http service manager APIs
*/

#ifndef __TT_HTTP_SERVICE_MANAGER__
#define __TT_HTTP_SERVICE_MANAGER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/def/tt_http_def.h>
#include <network/http/def/tt_http_service_def.h>
#include <network/http/service/tt_http_inserv_auth.h>
#include <network/http/tt_http_encoding_service.h>
#include <network/http/tt_http_in_service.h>
#include <network/http/tt_http_out_service.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_INLINE_OUTSERV_NUM 16

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_parser_s;
struct tt_http_resp_render_s;
struct tt_buf_s;

typedef struct
{
    tt_http_inserv_t *s;
    void *ctx;
} tt_http_inserv_ctx_t;

typedef struct tt_http_svcmgr_s
{
    struct tt_http_host_s *host;
    tt_http_inserv_ctx_t *owner;
    tt_http_inserv_ctx_t inserv[TT_HTTP_INSERV_TYPE_NUM];
    tt_http_inserv_ctx_t dynamic_inserv[TT_HTTP_INSERV_TYPE_NUM];
    tt_http_outserv_t **outserv;
    tt_http_outserv_t *inline_outserv[TT_HTTP_INLINE_OUTSERV_NUM];
    tt_http_encserv_t *encserv[TT_HTTP_TXENC_NUM];
    tt_http_inserv_host_ctx_t host_ctx;
    tt_http_inserv_param_ctx_t param_ctx;
    tt_http_inserv_cond_ctx_t cond_ctx;
    tt_http_inserv_file_ctx_t file_ctx;
    tt_http_inserv_auth_ctx_t auth_ctx;
    tt_u16_t inserv_num;
    tt_u16_t dynamic_inserv_num;
    tt_u16_t outserv_num;
    tt_u16_t outserv_max;
    tt_bool_t created_cond_ctx;
    tt_bool_t created_file_ctx;
    tt_bool_t created_auth_ctx;
    tt_bool_t discarding : 1;
    tt_bool_t loaded_dynamic_inserv : 1;
} tt_http_svcmgr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_http_svcmgr_init(IN tt_http_svcmgr_t *sm);

tt_export void tt_http_svcmgr_destroy(IN tt_http_svcmgr_t *sm);

tt_export void tt_http_svcmgr_clear(IN tt_http_svcmgr_t *sm);

tt_export tt_result_t tt_http_svcmgr_add_inserv(IN tt_http_svcmgr_t *sm,
                                                IN TO tt_http_inserv_t *s,
                                                IN OPT void *ctx);

tt_export tt_result_t
tt_http_svcmgr_add_dynamic_inserv(IN tt_http_svcmgr_t *sm,
                                  IN TO tt_http_inserv_t *s,
                                  IN OPT void *ctx,
                                  IN OUT tt_bool_t *ctx_created);

tt_export tt_result_t tt_http_svcmgr_add_inserv_host(IN tt_http_svcmgr_t *sm);

tt_export tt_result_t tt_http_svcmgr_add_inserv_param(IN tt_http_svcmgr_t *sm);

tt_export tt_result_t tt_http_svcmgr_add_outserv(IN tt_http_svcmgr_t *sm,
                                                 IN TO tt_http_outserv_t *s);

tt_export void tt_http_svcmgr_set_encserv(IN tt_http_svcmgr_t *sm,
                                          IN tt_http_txenc_t txenc,
                                          IN TO tt_http_encserv_t *s);

// ========================================
// incoming request
// ========================================

tt_export tt_http_inserv_action_t
tt_http_svcmgr_on_uri(IN tt_http_svcmgr_t *sm,
                      IN struct tt_http_parser_s *req,
                      OUT struct tt_http_resp_render_s *resp);

tt_export tt_http_inserv_action_t
tt_http_svcmgr_on_header(IN tt_http_svcmgr_t *sm,
                         IN struct tt_http_parser_s *req,
                         OUT struct tt_http_resp_render_s *resp);

tt_export tt_http_inserv_action_t
tt_http_svcmgr_on_body(IN tt_http_svcmgr_t *sm,
                       IN struct tt_http_parser_s *req,
                       OUT struct tt_http_resp_render_s *resp);

tt_export tt_http_inserv_action_t
tt_http_svcmgr_on_trailing(IN tt_http_svcmgr_t *sm,
                           IN struct tt_http_parser_s *req,
                           OUT struct tt_http_resp_render_s *resp);

tt_export tt_http_inserv_action_t
tt_http_svcmgr_on_complete(IN tt_http_svcmgr_t *sm,
                           IN struct tt_http_parser_s *req,
                           OUT struct tt_http_resp_render_s *resp);

tt_export tt_http_inserv_action_t
tt_http_svcmgr_get_body(IN tt_http_svcmgr_t *sm,
                        IN struct tt_http_parser_s *req,
                        IN struct tt_http_resp_render_s *resp,
                        OUT struct tt_buf_s *buf);

// ========================================
// outgoing response
// ========================================

tt_export tt_result_t
tt_http_svcmgr_on_resp_header(IN tt_http_svcmgr_t *sm,
                              IN struct tt_http_parser_s *req,
                              IN OUT struct tt_http_resp_render_s *resp);

// ========================================
// encoding response body
// ========================================

tt_export tt_result_t
tt_http_svcmgr_pre_body(IN tt_http_svcmgr_t *sm,
                        IN struct tt_http_parser_s *req,
                        IN struct tt_http_resp_render_s *resp,
                        OUT struct tt_buf_s **output);

tt_export tt_result_t
tt_http_svcmgr_on_resp_body(IN tt_http_svcmgr_t *sm,
                            IN struct tt_http_parser_s *req,
                            IN struct tt_http_resp_render_s *resp,
                            IN struct tt_buf_s *input,
                            OUT struct tt_buf_s **output);

tt_export tt_result_t
tt_http_svcmgr_post_body(IN tt_http_svcmgr_t *sm,
                         IN struct tt_http_parser_s *req,
                         IN struct tt_http_resp_render_s *resp,
                         IN struct tt_buf_s *input,
                         OUT struct tt_buf_s **output);

#endif /* __TT_HTTP_SERVICE_MANAGER__ */