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

#include <network/http/service/tt_http_encserv_chunked.h>

#include <algorithm/tt_buffer_common.h>

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

tt_http_encserv_t *tt_g_http_encserv_chunked;

static tt_http_encserv_itf_t s_encserv_chunked_itf = {NULL, NULL};

static tt_result_t __es_chunked_on_body(IN tt_http_encserv_t *s,
                                        IN struct tt_http_parser_s *req,
                                        IN struct tt_http_resp_render_s *resp,
                                        IN OUT tt_buf_t *input,
                                        OUT tt_buf_t **output);

static tt_result_t __es_chunked_post_body(IN tt_http_encserv_t *s,
                                          IN struct tt_http_parser_s *req,
                                          IN struct tt_http_resp_render_s *resp,
                                          IN OUT tt_buf_t *input,
                                          OUT tt_buf_t **output);

static tt_http_encserv_cb_t s_encserv_chunked_cb = {NULL,
                                                    __es_chunked_on_body,
                                                    __es_chunked_post_body};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_encserv_chunked_component_init(
    IN struct tt_component_s *comp, IN struct tt_profile_s *profile)
{
    tt_g_http_encserv_chunked = tt_http_encserv_chunked_create();
    if (tt_g_http_encserv_chunked == NULL) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_http_encserv_chunked_component_exit(IN struct tt_component_s *comp)
{
    tt_http_encserv_release(tt_g_http_encserv_chunked);
}

tt_http_encserv_t *tt_http_encserv_chunked_create()
{
    return tt_http_encserv_create(0,
                                  &s_encserv_chunked_itf,
                                  &s_encserv_chunked_cb);
}

tt_result_t __es_chunked_on_body(IN tt_http_encserv_t *s,
                                 IN struct tt_http_parser_s *req,
                                 IN struct tt_http_resp_render_s *resp,
                                 IN OUT tt_buf_t *input,
                                 OUT tt_buf_t **output)
{
    tt_char_t tmp[20] = {0};

    tt_snprintf(tmp, sizeof(tmp) - 1, "%x", TT_BUF_RLEN(input));

    TT_DO(tt_buf_put_head(input, (tt_u8_t *)"\r\n", 2));
    TT_DO(tt_buf_put_head(input, (tt_u8_t *)tmp, tt_strlen(tmp)));

    *output = input;
    return TT_SUCCESS;
}

tt_result_t __es_chunked_post_body(IN tt_http_encserv_t *s,
                                   IN struct tt_http_parser_s *req,
                                   IN struct tt_http_resp_render_s *resp,
                                   IN OUT tt_buf_t *input,
                                   OUT tt_buf_t **output)
{
    TT_DO(tt_buf_put_head(input, (tt_u8_t *)"0\r\n\r\n", 5));

    *output = input;
    return TT_SUCCESS;
}
