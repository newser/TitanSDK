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

#include <network/http/tt_http_parser.h>

#include <memory/tt_slab.h>
#include <network/http/tt_http_raw_header.h>

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

static int __on_uri(http_parser *p, const char *at, size_t length);

static int __on_status(http_parser *p, const char *at, size_t length);

static int __on_hdr_field(http_parser *p, const char *at, size_t length);

static int __on_hdr_value(http_parser *p, const char *at, size_t length);

static int __on_hdr_end(http_parser *p);

static int __on_body(http_parser *p, const char *at, size_t length);

static int __on_msg_end(http_parser *p, const char *at, size_t length);

static http_parser_settings tt_s_hp_setting = {
    NULL, // on_message_begin
    __on_uri,
    __on_status,
    __on_hdr_field,
    __on_hdr_value,
    __on_hdr_end,
    __on_msg_end,
    NULL,
    NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __clear_rawhdr(IN tt_dlist_t *dl);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_http_parser_init(IN tt_http_parser_t *hp,
                         IN tt_bool_t request,
                         IN tt_slab_t *rawhdr_slab,
                         IN tt_slab_t *rawval_slab,
                         IN OPT tt_http_parser_attr_t *attr)
{
    tt_http_parser_attr_t __attr;

    TT_ASSERT(hp != NULL);
    TT_ASSERT(rawhdr_slab != NULL);
    TT_ASSERT(rawhdr_slab->obj_size >= sizeof(tt_http_rawhdr_t));
    TT_ASSERT(rawval_slab != NULL);
    TT_ASSERT(rawval_slab->obj_size >= sizeof(tt_http_rawval_t));

    if (attr == NULL) {
        tt_http_parser_attr_default(&__attr);
        attr = &__attr;
    }

    tt_dlist_init(&hp->rawhdr);
    hp->rawhdr_slab = rawhdr_slab;
    hp->rawval_slab = rawval_slab;
    tt_buf_init(&hp->rbuf, &attr->rbuf_attr);
    hp->refind_threshold = attr->rbuf_refind_threshold;

    http_parser_init(&hp->parser,
                     TT_COND(request, HTTP_REQUEST, HTTP_RESPONSE));
}

void tt_http_parser_destroy(IN tt_http_parser_t *hp)
{
    TT_ASSERT(hp != NULL);

    __clear_rawhdr(&hp->rawhdr);

    tt_buf_destroy(&hp->rbuf);
}

void tt_http_parser_attr_default(IN tt_http_parser_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_buf_attr_default(&attr->rbuf_attr);
    attr->rbuf_refind_threshold = 1024;
}

void tt_http_parser_clear(IN tt_http_parser_t *hp)
{
    TT_ASSERT(hp != NULL);

    __clear_rawhdr(&hp->rawhdr);

    tt_buf_try_refine(&hp->rbuf, hp->refind_threshold);

    http_parser_init(&hp->parser, hp->parser.type);
}

tt_result_t tt_http_parser_run(IN tt_http_parser_t *hp, IN tt_u32_t len)
{
    tt_u32_t n = http_parser_execute(&hp->parser,
                                     &tt_s_hp_setting,
                                     (char *)TT_BUF_RPOS(&hp->rbuf),
                                     len);
    tt_buf_inc_rp(&hp->rbuf, n);
    TT_ASSERT(n <= len);
    if (n == len) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("http parsing failed: %s",
                 http_errno_name(HTTP_PARSER_ERRNO(&hp->parser)));
        return TT_FAIL;
    }
}

void __clear_rawhdr(IN tt_dlist_t *dl)
{
    tt_dnode_t *node;
    while ((node = tt_dlist_pop_head(dl)) != NULL) {
        tt_http_rawhdr_destroy(TT_CONTAINER(node, tt_http_rawhdr_t, dnode));
    }
}

// ========================================
// parse callbacks
// ========================================

int __on_uri(http_parser *p, const char *at, size_t length)
{
    return 0;
}

int __on_status(http_parser *p, const char *at, size_t length)
{
    return 0;
}

int __on_hdr_field(http_parser *p, const char *at, size_t length)
{
    return 0;
}

int __on_hdr_value(http_parser *p, const char *at, size_t length)
{
    return 0;
}

int __on_hdr_end(http_parser *p)
{
    return 0;
}

int __on_body(http_parser *p, const char *at, size_t length)
{
    return 0;
}

int __on_msg_end(http_parser *p, const char *at, size_t length)
{
    return 0;
}
