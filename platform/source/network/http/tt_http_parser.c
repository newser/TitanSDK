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

#include <limits.h>

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

static int __on_msg_end(http_parser *p);

static http_parser_settings tt_s_hp_setting = {
    NULL, // on_message_begin
    __on_uri,
    __on_status,
    __on_hdr_field,
    __on_hdr_value,
    __on_hdr_end,
    __on_body,
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

tt_result_t tt_http_parser_create(IN tt_http_parser_t *hp,
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

    hp->rh = NULL;
    hp->rv = NULL;

    hp->rawhdr_slab = rawhdr_slab;
    hp->rawval_slab = rawval_slab;
    tt_dlist_init(&hp->rawhdr);
    tt_dlist_init(&hp->trailing_rawhdr);

    // extend to initial size
    tt_buf_init(&hp->buf, &attr->buf_attr);
    if (!TT_OK(tt_buf_extend(&hp->buf))) {
        return TT_FAIL;
    }

    tt_blobex_init(&hp->uri, NULL, 0);
    tt_blobex_init(&hp->body, NULL, 0);

    http_parser_init(&hp->parser, HTTP_BOTH);

    hp->body_counter = 0;
    hp->complete_line1 = TT_FALSE;
    hp->complete_header = TT_FALSE;
    hp->complete_message = TT_FALSE;
    hp->complete_trailing_header = TT_FALSE;

    return TT_SUCCESS;
}

void tt_http_parser_destroy(IN tt_http_parser_t *hp)
{
    TT_ASSERT(hp != NULL);

    if (hp->rh != NULL) {
        tt_http_rawhdr_destroy(hp->rh);
    }

    if (hp->rv != NULL) {
        tt_http_rawval_destroy(hp->rv);
    }

    __clear_rawhdr(&hp->rawhdr);
    __clear_rawhdr(&hp->trailing_rawhdr);

    tt_buf_destroy(&hp->buf);

    tt_blobex_destroy(&hp->uri);
    tt_blobex_destroy(&hp->body);
}

void tt_http_parser_attr_default(IN tt_http_parser_attr_t *attr)
{
    tt_buf_attr_t *buf_attr;

    TT_ASSERT(attr != NULL);

    buf_attr = &attr->buf_attr;
    tt_buf_attr_default(buf_attr);
    buf_attr->min_extend = 16 * 1024; // 16K
    buf_attr->max_extend = 128 * 1024; // 128K
    buf_attr->max_limit = buf_attr->max_extend + 1;
}

void tt_http_parser_clear(IN tt_http_parser_t *hp, IN tt_bool_t clear_recv_buf)
{
    TT_ASSERT(hp != NULL);

    // not null means stopped during parsing and has not been added to list yet
    if (hp->rh != NULL) {
        tt_http_rawhdr_destroy(hp->rh);
        hp->rh = NULL;
    }

    if (hp->rv != NULL) {
        tt_http_rawval_destroy(hp->rv);
        hp->rv = NULL;
    }

    __clear_rawhdr(&hp->rawhdr);
    __clear_rawhdr(&hp->trailing_rawhdr);

    if (clear_recv_buf) {
        tt_buf_clear(&hp->buf);
    } else if (TT_BUF_RLEN(&hp->buf) == 0) {
        tt_buf_clear(&hp->buf);
    }

    tt_blobex_clear(&hp->uri);
    tt_blobex_clear(&hp->body);

    http_parser_init(&hp->parser, HTTP_BOTH);

    hp->body_counter = 0;
    hp->complete_line1 = TT_FALSE;
    hp->complete_header = TT_FALSE;
    hp->complete_message = TT_FALSE;
    hp->complete_trailing_header = TT_FALSE;
}

tt_result_t tt_http_parser_run(IN tt_http_parser_t *hp)
{
    http_parser *p = &hp->parser;
    tt_buf_t *buf = &hp->buf;
    tt_u32_t n;
    enum http_errno e;

    n = http_parser_execute(p,
                            &tt_s_hp_setting,
                            (char *)TT_BUF_RPOS(buf),
                            TT_BUF_RLEN(buf));
    TT_ASSERT(n <= TT_BUF_RLEN(buf));
    tt_buf_inc_rp(buf, n);

    if (p->upgrade) {
        // todo...
    }

    e = HTTP_PARSER_ERRNO(p);
    if (e == HPE_OK) {
        return TT_SUCCESS;
    } else if (e == HPE_PAUSED) {
        http_parser_pause(&hp->parser, 0);
        return TT_SUCCESS;
    } else {
        TT_ERROR("fail to parse http: [%s:%s]",
                 http_errno_name(e),
                 http_errno_description(e));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_http_method_t tt_http_parser_get_method(IN tt_http_parser_t *hp)
{
    if (hp->parser.method != 0) {
        return (tt_http_method_t)hp->parser.method;
    } else {
        return TT_HTTP_METHOD_NUM;
    }
}

tt_http_ver_t tt_http_parser_get_version(IN tt_http_parser_t *hp)
{
    http_parser *p = &hp->parser;
    if (p->http_major == 0) {
        if (p->http_minor == 9) {
            return TT_HTTP_V0_9;
        } else {
            return TT_HTTP_VER_NUM;
        }
    } else if (p->http_major == 1) {
        if (p->http_minor == 0) {
            return TT_HTTP_V1_0;
        } else if (p->http_minor == 1) {
            return TT_HTTP_V1_1;
        } else {
            return TT_HTTP_VER_NUM;
        }
    } else if (p->http_major == 2) {
        if (p->http_minor == 0) {
            return TT_HTTP_V2_0;
        } else {
            return TT_HTTP_VER_NUM;
        }
    } else {
        return TT_HTTP_VER_NUM;
    }
}

tt_http_status_t tt_http_parser_get_status(IN tt_http_parser_t *hp)
{
    http_parser *p = &hp->parser;
    if (p->status_code != 0) {
        return (tt_http_status_t)p->status_code;
    } else {
        return TT_HTTP_STATUS_INVALID;
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
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);
    tt_blobex_t *uri = &hp->uri;

    if (tt_blobex_len(uri) == 0) {
        tt_blobex_set(uri, (tt_u8_t *)at, length, TT_FALSE);
        return 0;
    } else if (TT_OK(tt_blobex_memcat(uri, (tt_u8_t *)at, length))) {
        return 0;
    } else {
        // fail to merge uri
        return -1;
    }
}

int __on_status(http_parser *p, const char *at, size_t length)
{
    return 0;
}

int __on_hdr_field(http_parser *p, const char *at, size_t length)
{
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);

    if (!hp->complete_line1) {
        hp->complete_line1 = TT_TRUE;
    }

    if (hp->rv != NULL) {
        // the last raw header is done parsing
        TT_ASSERT(hp->rh != NULL);
        tt_http_rawhdr_add_val(hp->rh, hp->rv);
        tt_http_rawhdr_add(TT_COND(hp->complete_header,
                                   &hp->trailing_rawhdr,
                                   &hp->rawhdr),
                           hp->rh);
        hp->rh = NULL;
        hp->rv = NULL;
    }

    if (hp->rh != NULL) {
        // it's parsing a header name
        if (TT_OK(tt_http_rawhdr_append_name(hp->rh, at, length))) {
            return 0;
        } else {
            return -1;
        }
    } else {
        // found a new header
        tt_http_rawhdr_t *rh = tt_http_rawhdr_create(hp->rawhdr_slab,
                                                     (tt_char_t *)at,
                                                     length,
                                                     TT_FALSE);
        if (rh != NULL) {
            hp->rh = rh;
            return 0;
        } else {
            return -1;
        }
    }
}

int __on_hdr_value(http_parser *p, const char *at, size_t length)
{
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);

    if (!hp->complete_line1) {
        hp->complete_line1 = TT_TRUE;
    }

    if (hp->rv != NULL) {
        // it's parsing a header value
        if (TT_OK(tt_http_rawval_append(hp->rv, at, length))) {
            return 0;
        } else {
            return -1;
        }
    } else {
        // see header value
        tt_http_rawval_t *rv = tt_http_rawval_create(hp->rawval_slab,
                                                     (tt_char_t *)at,
                                                     length,
                                                     TT_FALSE);
        if (rv != NULL) {
            hp->rv = rv;
            return 0;
        } else {
            return -1;
        }
    }

    return 0;
}

int __on_hdr_end(http_parser *p)
{
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);

    if (!hp->complete_line1) {
        hp->complete_line1 = TT_TRUE;
    }

    hp->complete_header = TT_TRUE;

    if (hp->rv != NULL) {
        // the last raw header is done parsing
        TT_ASSERT(hp->rh != NULL);
        tt_http_rawhdr_add_val(hp->rh, hp->rv);
        tt_http_rawhdr_add(&hp->rawhdr, hp->rh);
        hp->rh = NULL;
        hp->rv = NULL;
    }

    if (hp->rh != NULL) {
        // end with an empty header
        tt_http_rawhdr_add(&hp->rawhdr, hp->rh);
        hp->rh = NULL;
    }

    return 0;
}

int __on_body(http_parser *p, const char *at, size_t length)
{
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);

    tt_blobex_set(&hp->body, (tt_u8_t *)at, length, TT_FALSE);
    ++hp->body_counter;

    if (!http_body_is_final(p)) {
        // 1. for msg with content-length, it must not pause parser, otherwise
        // __on_msg_end() won't be called because there is no new data
        // 2. for chunked body, the last "0\r\n" would be pass to parser
        // and __on_body() was not called and thus parser won't be paused
        http_parser_pause(&hp->parser, 1);
    }

    return 0;
}

int __on_msg_end(http_parser *p)
{
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);

    hp->complete_message = TT_TRUE;

    if (hp->rv != NULL) {
        // the last raw header is done parsing
        TT_ASSERT(hp->rh != NULL);
        tt_http_rawhdr_add_val(hp->rh, hp->rv);
        tt_http_rawhdr_add(&hp->trailing_rawhdr, hp->rh);
        hp->rh = NULL;
        hp->rv = NULL;

        hp->complete_trailing_header = TT_TRUE;
    }

    // must pause as there may be more than 1 msg in buf
    http_parser_pause(&hp->parser, 1);

    return 0;
}
