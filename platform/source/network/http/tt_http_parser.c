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
#include <network/http/header/tt_http_hdr_auth.h>
#include <network/http/header/tt_http_hdr_content_encoding.h>
#include <network/http/header/tt_http_hdr_cookie.h>
#include <network/http/header/tt_http_hdr_etag.h>
#include <network/http/header/tt_http_hdr_transfer_encoding.h>
#include <network/http/tt_http_raw_header.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __FOREACH_RV(rh, rv, name)                                             \
    for (rh = tt_http_rawhdr_find(&hp->rawhdr, name); rh != NULL;              \
         rh = tt_http_rawhdr_next(rh, name)) {                                 \
        for (rv = tt_http_rawval_head(&rh->val); rv != NULL;                   \
             rv = tt_http_rawval_next(rv))
#define __FOREACH_RV_END() }

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

static void __clear_hdr(IN tt_dlist_t *dl);

tt_inline void __add_hdr(IN tt_http_parser_t *hp, IN tt_http_hdr_t *h)
{
    tt_dlist_push_tail(&hp->hdr, &h->dnode);
}

static tt_http_rawval_t *__hp_first_rv(IN tt_http_parser_t *hp,
                                       IN tt_char_t *name,
                                       IN tt_u32_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_http_parser_create(IN tt_http_parser_t *hp,
                                  IN tt_slab_t *rawhdr_slab,
                                  IN tt_slab_t *rawval_slab,
                                  IN OPT tt_http_parser_attr_t *attr)
{
    tt_http_parser_attr_t __attr;
    tt_u32_t i;

    TT_ASSERT(hp != NULL);
    TT_ASSERT(rawhdr_slab != NULL);
    TT_ASSERT(rawhdr_slab->obj_size >= sizeof(tt_http_rawhdr_t));
    TT_ASSERT(rawval_slab != NULL);
    TT_ASSERT(rawval_slab->obj_size >= sizeof(tt_http_rawval_t));

    if (attr == NULL) {
        tt_http_parser_attr_default(&__attr);
        attr = &__attr;
    }

    hp->c = NULL;
    hp->rh = NULL;
    hp->rv = NULL;

    hp->rawhdr_slab = rawhdr_slab;
    hp->rawval_slab = rawval_slab;
    tt_dlist_init(&hp->rawhdr);
    tt_dlist_init(&hp->trailing_rawhdr);
    hp->host = NULL;
    hp->contype_map = attr->contype_map;
    tt_dlist_init(&hp->hdr);

    // extend to initial size
    tt_buf_init(&hp->buf, &attr->buf_attr);
    if (!TT_OK(tt_buf_extend(&hp->buf))) {
        return TT_FAIL;
    }

    tt_blobex_init(&hp->rawuri, NULL, 0);
    tt_http_uri_init(&hp->uri);
    tt_blobex_init(&hp->body, NULL, 0);

    http_parser_init(&hp->parser, HTTP_BOTH);

    hp->body_counter = 0;
    hp->contype = TT_HTTP_CONTYPE_NUM;
    hp->content_len = -1;
    tt_http_accenc_init(&hp->accenc);

    hp->txenc_num = 0;
    for (i = 0; i < TT_HTTP_TXENC_NUM; ++i) {
        hp->txenc[i] = TT_HTTP_TXENC_NUM;
    }

    hp->contenc_num = 0;
    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        hp->contenc[i] = TT_HTTP_ENC_NUM;
    }

    hp->complete_line1 = TT_FALSE;
    hp->complete_header = TT_FALSE;
    hp->complete_message = TT_FALSE;
    hp->complete_trailing_header = TT_FALSE;
    hp->updated_host = TT_FALSE;
    hp->updated_uri = TT_FALSE;
    hp->updated_content_len = TT_FALSE;
    hp->updated_contype = TT_FALSE;
    hp->updated_txenc = TT_FALSE;
    hp->updated_contenc = TT_FALSE;
    hp->updated_accenc = TT_FALSE;
#define __ENTRY(hdr, name) hp->updated_##hdr = TT_FALSE;
    TT_HTTP_PARSE_HDR_MAP(__ENTRY)
#undef __ENTRY
    hp->miss_txenc = TT_FALSE;
    hp->miss_contype = TT_FALSE;
    hp->miss_content_len = TT_FALSE;
    hp->miss_contenc = TT_FALSE;

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

    // hp->host is a pointer and need not be destroyed

    __clear_rawhdr(&hp->rawhdr);
    __clear_rawhdr(&hp->trailing_rawhdr);
    __clear_hdr(&hp->hdr);

    tt_buf_destroy(&hp->buf);

    tt_blobex_destroy(&hp->rawuri);
    tt_http_uri_destroy(&hp->uri);
    tt_blobex_destroy(&hp->body);
}

void tt_http_parser_attr_default(IN tt_http_parser_attr_t *attr)
{
    tt_buf_attr_t *buf_attr;

    TT_ASSERT(attr != NULL);

    attr->contype_map = &tt_g_http_contype_map;

    buf_attr = &attr->buf_attr;
    tt_buf_attr_default(buf_attr);
    buf_attr->min_extend = 16 * 1024; // 16K
    buf_attr->max_extend = 128 * 1024; // 128K
    buf_attr->max_limit = buf_attr->max_extend + 1;
}

void tt_http_parser_clear(IN tt_http_parser_t *hp, IN tt_bool_t clear_recv_buf)
{
    tt_u32_t i;

    TT_ASSERT(hp != NULL);

    // do not change hp->c

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
    __clear_hdr(&hp->hdr);

    hp->host = NULL;

    if (clear_recv_buf) {
        tt_buf_clear(&hp->buf);
    } else if (TT_BUF_RLEN(&hp->buf) == 0) {
        tt_buf_clear(&hp->buf);
    }

    tt_blobex_clear(&hp->rawuri);
    tt_http_uri_clear(&hp->uri);
    tt_blobex_clear(&hp->body);

    http_parser_init(&hp->parser, HTTP_BOTH);

    hp->body_counter = 0;
    hp->content_len = -1;
    tt_http_accenc_init(&hp->accenc);

    hp->txenc_num = 0;
    for (i = 0; i < TT_HTTP_TXENC_NUM; ++i) {
        hp->txenc[i] = TT_HTTP_TXENC_NUM;
    }

    hp->contenc_num = 0;
    for (i = 0; i < TT_HTTP_ENC_NUM; ++i) {
        hp->contenc[i] = TT_HTTP_ENC_NUM;
    }

    hp->contype = TT_HTTP_CONTYPE_NUM;
    hp->complete_line1 = TT_FALSE;
    hp->complete_header = TT_FALSE;
    hp->complete_message = TT_FALSE;
    hp->complete_trailing_header = TT_FALSE;
    hp->updated_host = TT_FALSE;
    hp->updated_uri = TT_FALSE;
    hp->updated_contype = TT_FALSE;
    hp->updated_content_len = TT_FALSE;
    hp->updated_txenc = TT_FALSE;
    hp->updated_contenc = TT_FALSE;
    hp->updated_accenc = TT_FALSE;
#define __ENTRY(hdr, name) hp->updated_##hdr = TT_FALSE;
    TT_HTTP_PARSE_HDR_MAP(__ENTRY)
#undef __ENTRY
    hp->miss_txenc = TT_FALSE;
    hp->miss_contype = TT_FALSE;
    hp->miss_content_len = TT_FALSE;
    hp->miss_contenc = TT_FALSE;
}

tt_result_t tt_http_parser_run(IN tt_http_parser_t *hp)
{
    http_parser *p = &hp->parser;
    tt_buf_t *buf = &hp->buf;
    tt_u32_t n;
    enum http_errno e;

    n = (tt_u32_t)http_parser_execute(p,
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

tt_http_uri_t *tt_http_parser_get_uri(IN tt_http_parser_t *hp)
{
    if (hp->parser.type == HTTP_RESPONSE) {
        return NULL;
    }

    if (hp->updated_uri) {
        return &hp->uri;
    }

    if (tt_blobex_empty(&hp->rawuri) ||
        !TT_OK(tt_http_uri_parse_n(&hp->uri,
                                   tt_blobex_addr(&hp->rawuri),
                                   tt_blobex_len(&hp->rawuri)))) {
        return NULL;
    }

    hp->updated_uri = TT_TRUE;
    return &hp->uri;
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

tt_blobex_t *tt_http_parser_get_host(IN tt_http_parser_t *hp)
{
    if (!hp->updated_host) {
        tt_http_rawval_t *rv = __hp_first_rv(hp, "Host", sizeof("Host") - 1);
        if (rv != NULL) {
            hp->host = &rv->val;
            if ((tt_blobex_len(hp->host) == 0) &&
                (tt_blobex_addr(hp->host) != NULL)) {
                // this is possible when recevied "Host: \r\n"
                tt_blobex_clear(hp->host);
            }
        }

        hp->updated_host = TT_TRUE;
    }
    return hp->host;
}

tt_http_hdr_t *tt_http_parser_find_hdr(IN tt_http_parser_t *hp,
                                       IN tt_http_hname_t hname)
{
    tt_dnode_t *dn;
    for (dn = tt_dlist_head(&hp->hdr); dn != NULL; dn = dn->next) {
        tt_http_hdr_t *h = TT_CONTAINER(dn, tt_http_hdr_t, dnode);
        if (h->name == hname) {
            return h;
        }
    }
    return NULL;
}

tt_http_contype_t tt_http_parser_get_contype(IN tt_http_parser_t *hp)
{
    if (!hp->updated_contype) {
        tt_http_rawval_t *rv =
            __hp_first_rv(hp, "Content-Type", sizeof("Content-Type") - 1);
        if (rv != NULL) {
            tt_http_contype_entry_t *e =
                tt_http_contype_map_find_name_n(hp->contype_map,
                                                tt_blobex_addr(&rv->val),
                                                tt_blobex_len(&rv->val));
            if (e != NULL) {
                hp->contype = e->type;
            } else {
                // unknown type
                hp->miss_contype = TT_TRUE;
            }
        }

        if (!hp->miss_contype &&
            (tt_http_rawhdr_count_name_n(&hp->rawhdr,
                                         "Content-Type",
                                         sizeof("Content-Type") - 1) > 1)) {
            // more than 1 content type
            hp->miss_contype = TT_TRUE;
        }

        hp->updated_contype = TT_TRUE;
    }
    return hp->contype;
}

tt_s32_t tt_http_parser_get_content_len(IN tt_http_parser_t *hp)
{
    if (!hp->updated_content_len) {
        tt_http_rawval_t *rv =
            __hp_first_rv(hp, "Content-Length", sizeof("Content-Length") - 1);
        if (rv != NULL) {
            tt_char_t tmp[12] = {0};
            tt_s32_t len;

            if (tt_blobex_len(&rv->val) >= sizeof(tmp)) {
                hp->miss_content_len = TT_TRUE;
                goto done;
            }

            tt_memcpy(tmp, tt_blobex_addr(&rv->val), tt_blobex_len(&rv->val));
            if (!TT_OK(tt_strtos32(tmp, NULL, 10, &len))) {
                hp->content_len = -1;
                hp->miss_content_len = TT_TRUE;
                goto done;
            }

            if (tt_http_rawhdr_count_name_n(&hp->rawhdr,
                                            "Content-Length",
                                            sizeof("Content-Length") - 1) > 1) {
                hp->miss_content_len = TT_TRUE;
            }

            hp->content_len = len;
        }

    done:
        hp->updated_content_len = TT_TRUE;
    }
    return hp->content_len;
}

tt_u32_t tt_http_parser_get_txenc(IN tt_http_parser_t *hp,
                                  OUT tt_http_txenc_t *txenc)
{
    tt_u32_t i;

    if (!hp->updated_txenc) {
        tt_http_hdr_t *h;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

        h = tt_http_hdr_txenc_create();
        if (h == NULL) {
            return 0;
        }

        __FOREACH_RV(rh, rv, "Transfer-Encoding")
        {
            tt_http_hdr_parse_n(h,
                                tt_blobex_addr(&rv->val),
                                tt_blobex_len(&rv->val));
        }
        __FOREACH_RV_END()

        hp->txenc_num = tt_http_hdr_txenc_get(h, hp->txenc);
        hp->miss_txenc = h->missed_field;
        tt_http_hdr_destroy(h);

        hp->updated_txenc = TT_TRUE;
    }

    for (i = 0; i < hp->txenc_num; ++i) {
        txenc[i] = hp->txenc[i];
    }
    return hp->txenc_num;
}

tt_u32_t tt_http_parser_get_contenc(IN tt_http_parser_t *hp,
                                    OUT tt_http_enc_t *contenc)
{
    tt_u32_t i;

    if (!hp->updated_contenc) {
        tt_http_hdr_t *h;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

        h = tt_http_hdr_contenc_create();
        if (h == NULL) {
            return 0;
        }

        __FOREACH_RV(rh, rv, "Content-Encoding")
        {
            tt_http_hdr_parse_n(h,
                                tt_blobex_addr(&rv->val),
                                tt_blobex_len(&rv->val));
        }
        __FOREACH_RV_END()

        hp->contenc_num = tt_http_hdr_contenc_get(h, hp->contenc);
        hp->miss_txenc = h->missed_field;
        tt_http_hdr_destroy(h);

        hp->updated_contenc = TT_TRUE;
    }

    for (i = 0; i < hp->contenc_num; ++i) {
        contenc[i] = hp->contenc[i];
    }
    return hp->contenc_num;
}

tt_http_accenc_t *tt_http_parser_get_accenc(IN tt_http_parser_t *hp)
{
    tt_u32_t i;

    if (!hp->updated_accenc) {
        tt_http_hdr_t *h;
        tt_http_rawhdr_t *rh;
        tt_http_rawval_t *rv;

        h = tt_http_hdr_accenc_create();
        if (h == NULL) {
            goto out;
        }

        __FOREACH_RV(rh, rv, "Accept-Encoding")
        {
            tt_http_hdr_parse_n(h,
                                tt_blobex_addr(&rv->val),
                                tt_blobex_len(&rv->val));
        }
        __FOREACH_RV_END()

        tt_memcpy(&hp->accenc,
                  tt_http_hdr_accenc_get(h),
                  sizeof(tt_http_accenc_t));

        tt_http_hdr_destroy(h);

        hp->updated_accenc = TT_TRUE;
    }

out:
    return &hp->accenc;
}

#define __ENTRY(hdr, name)                                                     \
    tt_http_hdr_t *tt_http_parser_get_##hdr(IN tt_http_parser_t *hp)           \
    {                                                                          \
        tt_http_hdr_t *h;                                                      \
                                                                               \
        if (!hp->updated_##hdr) {                                              \
            tt_http_rawhdr_t *rh;                                              \
            tt_http_rawval_t *rv;                                              \
            const tt_char_t *name_str = tt_g_http_hname[name];                 \
            tt_u32_t num = 0;                                                  \
                                                                               \
            h = tt_http_hdr_##hdr##_create();                                  \
            if (h == NULL) {                                                   \
                return NULL;                                                   \
            }                                                                  \
                                                                               \
            __FOREACH_RV(rh, rv, name_str)                                     \
            {                                                                  \
                TT_DO_G(fail, tt_http_hdr_pre_parse(h));                       \
                TT_DO_G(fail,                                                  \
                        tt_http_hdr_parse_n(h,                                 \
                                            tt_blobex_addr(&rv->val),          \
                                            tt_blobex_len(&rv->val)));         \
                TT_DO_G(fail, tt_http_hdr_post_parse(h));                      \
                ++num;                                                         \
            }                                                                  \
            __FOREACH_RV_END()                                                 \
                                                                               \
            if (num > 0) {                                                     \
                __add_hdr(hp, h);                                              \
            } else {                                                           \
                tt_http_hdr_destroy(h);                                        \
                h = NULL;                                                      \
            }                                                                  \
                                                                               \
            hp->updated_##hdr = TT_TRUE;                                       \
        } else {                                                               \
            h = tt_http_parser_find_hdr(hp, name);                             \
        }                                                                      \
                                                                               \
        return h;                                                              \
                                                                               \
    fail:                                                                      \
        tt_http_hdr_destroy(h);                                                \
        return NULL;                                                           \
    }
TT_HTTP_PARSE_HDR_MAP(__ENTRY)
#undef __ENTRY

void __clear_rawhdr(IN tt_dlist_t *dl)
{
    tt_dnode_t *node;
    while ((node = tt_dlist_pop_head(dl)) != NULL) {
        tt_http_rawhdr_destroy(TT_CONTAINER(node, tt_http_rawhdr_t, dnode));
    }
}

void __clear_hdr(IN tt_dlist_t *dl)
{
    tt_dnode_t *node;
    while ((node = tt_dlist_pop_head(dl)) != NULL) {
        tt_http_hdr_destroy(TT_CONTAINER(node, tt_http_hdr_t, dnode));
    }
}

tt_http_rawval_t *__hp_first_rv(IN tt_http_parser_t *hp,
                                IN tt_char_t *name,
                                IN tt_u32_t len)
{
    tt_http_rawhdr_t *rh = tt_http_rawhdr_find(&hp->rawhdr, name);
    return TT_COND(rh != NULL, tt_http_rawval_head(&rh->val), NULL);
}

// ========================================
// parse callbacks
// ========================================

int __on_uri(http_parser *p, const char *at, size_t length)
{
    tt_http_parser_t *hp = TT_CONTAINER(p, tt_http_parser_t, parser);
    tt_blobex_t *uri = &hp->rawuri;

    if (tt_blobex_len(uri) == 0) {
        tt_blobex_set(uri, (tt_u8_t *)at, (tt_u32_t)length, TT_FALSE);
        return 0;
    } else if (TT_OK(tt_blobex_memcat(uri, (tt_u8_t *)at, (tt_u32_t)length))) {
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
        if (TT_OK(tt_http_rawhdr_append_name(hp->rh, at, (tt_u32_t)length))) {
            return 0;
        } else {
            return -1;
        }
    } else {
        // ok a new header
        tt_http_rawhdr_t *rh;

        rh = tt_http_rawhdr_create(hp->rawhdr_slab,
                                   (tt_char_t *)at,
                                   (tt_u32_t)length,
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
        if (TT_OK(tt_http_rawval_append(hp->rv, at, (tt_u32_t)length))) {
            return 0;
        } else {
            return -1;
        }
    } else {
        // see header value
        tt_http_rawval_t *rv = tt_http_rawval_create(hp->rawval_slab,
                                                     (tt_char_t *)at,
                                                     (tt_u32_t)length,
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

    tt_blobex_set(&hp->body, (tt_u8_t *)at, (tt_u32_t)length, TT_FALSE);
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

// ========================================
// helper
// ========================================

void tt_http_parse_weight(IN OUT tt_char_t **s,
                          IN OUT tt_u32_t *len,
                          OUT tt_float_t *q)
{
    tt_char_t *pos, *end, *sep;
    tt_u32_t n;
    tt_bool_t ok;
    tt_float_t weight;

    tt_trim_lr((tt_u8_t **)s, len, ' ');
    pos = *s;
    n = *len;
    end = pos + n;

    // no "q", return trimmed
    sep = tt_memchr(pos, ';', n);
    if (sep == NULL) {
        // If no "q" parameter is present, the default weight is 1.
        *q = 1.0f;
        return;
    }

    // trim value before ";"
    n = (tt_u32_t)(sep - pos);
    tt_trim_lr((tt_u8_t **)&pos, &n, ' ');
    //*s = pos;
    *len = n;

    // parse "q="
    /*
     weight = OWS ";" OWS "q=" qvalue
     qvalue = ( "0" [ "." 0*3DIGIT ] ) / ( "1" [ "." 0*3("0") ] )
     */
    ok = TT_FALSE;
    pos = sep + 1;
    while ((pos + 2) <= end) {
        if (((pos[0] == 'q') || (pos[0] == 'Q')) && (pos[1] == '=')) {
            ok = TT_TRUE;
            break;
        }
        ++pos;
    }
    if (!ok) {
        *q = 1.0f;
        return;
    }
    pos += 2;

    ok = TT_TRUE;
    weight = 0.0f;
    if (ok && (pos < end) && (ok = tt_isdigit(pos[0]))) {
        weight += pos[0] - '0';
    }
    if (ok && ((pos + 1) < end) && (ok = (pos[1] == '.'))) {
    }
    if (ok && ((pos + 2) < end) && (ok = tt_isdigit(pos[2]))) {
        weight += (pos[2] - '0') * 0.1;
    }
    if (ok && ((pos + 3) < end) && (ok = tt_isdigit(pos[3]))) {
        weight += (pos[3] - '0') * 0.01;
    }
    if (ok && ((pos + 4) < end) && (ok = tt_isdigit(pos[4]))) {
        weight += (pos[4] - '0') * 0.001;
    }

    weight = TT_MAX(weight, 0.0f);
    weight = TT_MIN(weight, 1.0f);
    *q = weight;
}
