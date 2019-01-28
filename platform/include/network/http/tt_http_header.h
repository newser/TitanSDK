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
@file tt_http_header.h
@brief http header

this file defines http header APIs
*/

#ifndef __TT_HTTP_HEADER__
#define __TT_HTTP_HEADER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <network/http/def/tt_http_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_HTTP_HVAL_CAST(hv, name) TT_PTR_INC(name, hv, sizeof(tt_http_hval_t))

#define TT_HTTP_HDR_CAST(hv, name) TT_PTR_INC(name, hv, sizeof(tt_http_hdr_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_hdr_s;
struct tt_http_hval_s;
struct tt_blobex_s;

// ========================================
// header value
// ========================================

typedef struct tt_http_hval_s *(*tt_http_hval_create_t)(
    IN struct tt_http_hdr_s *h);

typedef void (*tt_http_hval_destroy_t)(IN struct tt_http_hval_s *hv);

typedef tt_result_t (*tt_http_hval_parse_t)(IN struct tt_http_hval_s *hv,
                                            IN const tt_char_t *val,
                                            IN tt_u32_t len);

typedef tt_u32_t (*tt_http_hval_render_len_t)(IN struct tt_http_hval_s *hv);

typedef tt_u32_t (*tt_http_hval_render_t)(IN struct tt_http_hval_s *hv,
                                          IN tt_char_t *dst);

typedef struct
{
    tt_http_hval_create_t create;
    tt_http_hval_destroy_t destroy;
    tt_http_hval_parse_t parse;
    tt_http_hval_render_len_t render_len;
    tt_http_hval_render_t render;
} tt_http_hval_itf_t;

typedef struct tt_http_hval_s
{
    tt_dnode_t dnode;
} tt_http_hval_t;

// ========================================
// header
// ========================================

typedef void (*tt_http_hdr_destroy_t)(IN struct tt_http_hdr_s *h);

typedef tt_result_t (*tt_http_hdr_parse_t)(IN struct tt_http_hdr_s *h,
                                           IN const tt_char_t *val,
                                           IN tt_u32_t len);

typedef tt_u32_t (*tt_http_hdr_render_len_t)(IN struct tt_http_hdr_s *h);

typedef tt_u32_t (*tt_http_hdr_render_t)(IN struct tt_http_hdr_s *h,
                                         IN tt_char_t *dst);

typedef struct
{
    tt_http_hdr_destroy_t destroy;
    tt_http_hdr_parse_t parse;
    tt_http_hdr_render_len_t render_len;
    tt_http_hdr_render_t render;
} tt_http_hdr_itf_t;

typedef struct tt_http_hdr_s
{
    tt_http_hdr_itf_t *itf;
    tt_http_hval_itf_t *val_itf;
    // @final_val_itf is a private field
    tt_http_hdr_itf_t *final_val_itf;
    tt_dlist_t val;
    tt_dnode_t dnode;
    tt_http_hname_t name : 8;
    // can be set if saw unknown field during parsing
    tt_bool_t missed_field : 1;
} tt_http_hdr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_http_hval_itf_t tt_g_http_hval_blob_itf;

tt_export tt_http_hdr_itf_t tt_g_http_hdr_line_itf;

tt_export tt_http_hdr_itf_t tt_g_http_hdr_cs_itf;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// header value
// ========================================

tt_export tt_http_hval_t *tt_http_hval_create(IN tt_u32_t extra_size);

tt_export void tt_http_hval_destroy(IN tt_http_hval_t *hv);

tt_inline tt_http_hval_t *tt_http_hval_head(IN tt_http_hdr_t *h)
{
    tt_dnode_t *dn = tt_dlist_head(&h->val);
    return TT_COND(dn != NULL, TT_CONTAINER(dn, tt_http_hval_t, dnode), NULL);
}

tt_inline tt_http_hval_t *tt_http_hval_next(IN tt_http_hval_t *hv)
{
    tt_dnode_t *dn = hv->dnode.next;
    return TT_COND(dn != NULL, TT_CONTAINER(dn, tt_http_hval_t, dnode), NULL);
}

// ========================================
// header
// ========================================

tt_export tt_http_hdr_t *tt_http_hdr_create(IN tt_u32_t extra_size,
                                            IN tt_http_hname_t name,
                                            IN tt_http_hdr_itf_t *itf,
                                            IN tt_http_hval_itf_t *val_itf);

tt_export void tt_http_hdr_destroy(IN tt_http_hdr_t *h);

tt_export void tt_http_hdr_clear(IN tt_http_hdr_t *h);

tt_inline void tt_http_hdr_add(IN tt_http_hdr_t *h, IN tt_http_hval_t *hv)
{
    tt_dlist_push_tail(&h->val, &hv->dnode);
}

tt_inline void tt_http_hdr_remove(IN tt_http_hdr_t *h, IN tt_http_hval_t *hv)
{
    tt_dlist_remove(&h->val, &hv->dnode);
}

tt_inline tt_result_t tt_http_hdr_parse_n(IN tt_http_hdr_t *h,
                                          IN const tt_char_t *val,
                                          IN tt_u32_t len)
{
    if ((val[0] != 0) && (len != 0)) {
        return h->itf->parse(h, val, len);
    } else {
        return TT_SUCCESS;
    }
}

tt_inline tt_result_t tt_http_hdr_parse(IN tt_http_hdr_t *h,
                                        IN const tt_char_t *val)
{
    return TT_COND(val[0] != 0,
                   tt_http_hdr_parse_n(h, val, tt_strlen(val)),
                   TT_SUCCESS);
}

tt_inline tt_u32_t tt_http_hdr_render_len(IN tt_http_hdr_t *h)
{
    if ((h->final_val_itf != NULL) && (h->final_val_itf->render_len != NULL)) {
        return h->final_val_itf->render_len(h);
    } else {
        return h->itf->render_len(h);
    }
}

tt_inline tt_u32_t tt_http_hdr_render(IN tt_http_hdr_t *h, IN tt_char_t *dst)
{
    if ((h->final_val_itf != NULL) && (h->final_val_itf->render != NULL)) {
        return h->final_val_itf->render(h, dst);
    } else {
        return h->itf->render(h, dst);
    }
}

// ========================================
// helper
// ========================================

tt_export tt_http_hdr_t *tt_http_hdr_create_line(
    IN tt_u32_t extra_size,
    IN tt_http_hname_t name,
    IN OPT tt_http_hdr_itf_t *val_itf);

tt_export tt_http_hdr_t *tt_http_hdr_create_cs(IN tt_u32_t extra_size,
                                               IN tt_http_hname_t name,
                                               IN OPT
                                                   tt_http_hdr_itf_t *val_itf);

#endif /* __TT_HTTP_HEADER__ */
