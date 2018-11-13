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

#include <network/http/tt_http_uri.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct __hu_param_s
{
    tt_kv_t name_val;
    tt_bool_t removed : 1;
} __hu_param_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

// from tt_uri.c
extern tt_u32_t __percent_encode_len(IN tt_char_t *str,
                                     IN tt_u32_t len,
                                     IN tt_char_t *enc_tbl);

extern tt_u32_t __percent_encode(IN tt_char_t *str,
                                 IN tt_u32_t len,
                                 IN tt_char_t *enc_tbl,
                                 OUT tt_char_t *dst);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_char_t tt_s_http_generic_enc_tbl[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    '-', '.', 0,   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   0,
    0,   0,   0,   0,   0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', 0,   0,   0,   0,   '_', 0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', 0,   0,   0,   '~', 0,
};

static tt_char_t tt_s_http_path_enc_tbl[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   '&', 0,   0,   0,   0,   0,   0,
    '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   ';',
    0,   '=', 0,   0,   0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', 0,   0,   0,   0,   '_', 0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', 0,   0,   0,   '~', 0,
};

static tt_char_t tt_s_http_query_enc_tbl[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   '&', 0,   0,   0,   0,   0,   0,
    '-', '.', 0,   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   0,
    0,   '=', 0,   0,   0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', 0,   0,   0,   0,   '_', 0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', 0,   0,   0,   '~', 0,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_http_scheme_t __hu_parse_scheme(IN tt_uri_t *u);

static tt_result_t __hu_parse_path(IN tt_http_uri_t *hu,
                                   IN tt_char_t *path,
                                   IN tt_u32_t path_len);

static tt_result_t __hu_parse_query(IN tt_http_uri_t *hu,
                                    IN tt_char_t *query,
                                    IN tt_u32_t query_len);

static tt_result_t __hu_parse_param(IN tt_queue_t *q,
                                    IN tt_char_t *param,
                                    IN tt_u32_t param_len);

static void __hupq_clear(IN tt_queue_t *q);

static tt_result_t __hupq_parse_add(IN tt_queue_t *q,
                                    IN tt_char_t *p,
                                    IN tt_u32_t len);

static tt_result_t __hupq_add_n(IN tt_queue_t *q,
                                IN tt_char_t *name,
                                IN tt_u32_t len,
                                IN tt_bool_t owner);

static tt_result_t __hupq_add_nv(IN tt_queue_t *q,
                                 IN tt_char_t *name,
                                 IN tt_u32_t name_len,
                                 IN tt_bool_t name_owner,
                                 IN tt_char_t *val,
                                 IN tt_u32_t val_len,
                                 IN tt_bool_t val_owner);

static tt_bool_t __hupq_remove_n(IN tt_queue_t *q,
                                 IN tt_char_t *name,
                                 IN tt_u32_t len,
                                 IN tt_bool_t owner);

static tt_bool_t __hupq_remove_nv(IN tt_queue_t *q,
                                  IN tt_char_t *name,
                                  IN tt_u32_t name_len,
                                  IN tt_bool_t name_owner,
                                  IN tt_char_t *val,
                                  IN tt_u32_t val_len,
                                  IN tt_bool_t val_owner);

static __hu_param_t *__hupq_find_n(IN tt_queue_t *q,
                                   IN tt_char_t *name,
                                   IN tt_u32_t len);

static __hu_param_t *__hupq_find_nv(IN tt_queue_t *q,
                                    IN tt_char_t *name,
                                    IN tt_u32_t name_len,
                                    IN tt_char_t *val,
                                    IN tt_u32_t val_len);

static void __hupg_own(IN tt_queue_t *q);

tt_inline void __hup_init(IN __hu_param_t *hp)
{
    tt_kv_init(&hp->name_val);
    hp->removed = TT_FALSE;
}

tt_inline void __hup_destroy(IN __hu_param_t *hp)
{
    tt_kv_destroy(&hp->name_val);
}

tt_inline tt_result_t __hup_set_name(IN __hu_param_t *hp,
                                     IN tt_u8_t *addr,
                                     IN tt_u32_t len,
                                     IN tt_bool_t owner)
{
    return tt_kv_set_key(&hp->name_val, addr, len, owner);
}

tt_inline tt_result_t __hup_set_val(IN __hu_param_t *hp,
                                    IN tt_u8_t *addr,
                                    IN tt_u32_t len,
                                    IN tt_bool_t owner)
{
    return tt_kv_set_val(&hp->name_val, addr, len, owner);
}

static tt_result_t __hu_update_path(IN tt_http_uri_t *hu);

static tt_result_t __hu_update_query(IN tt_http_uri_t *hu);

static tt_u32_t __encode_q_len(IN tt_queue_t *q, IN tt_char_t *t);

static tt_u32_t __encode_q(IN tt_queue_t *q,
                           IN tt_char_t *t,
                           OUT tt_char_t *dst);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_http_uri_init(IN tt_http_uri_t *hu)
{
    tt_queue_attr_t q_attr;

    tt_queue_attr_default(&q_attr);
    q_attr.obj_per_frame = 16;

    tt_queue_init(&hu->path_param, sizeof(__hu_param_t), &q_attr);
    tt_queue_init(&hu->query_param, sizeof(__hu_param_t), &q_attr);
    tt_uri_init(&hu->u);
    tt_fpath_init(&hu->path, '/');
    hu->scheme = TT_HTTP_SCHEME_UNDEFINED; // default
    hu->path_modified = TT_FALSE;
    hu->query_modified = TT_FALSE;
}

tt_result_t tt_http_uri_create(IN tt_http_uri_t *hu,
                               IN tt_char_t *str,
                               IN tt_u32_t len)
{
    tt_uri_t *u = &hu->u;
    tt_queue_attr_t q_attr;

    TT_DO(tt_uri_create(u, str, len));

    hu->scheme = __hu_parse_scheme(u);
    if (!TT_HTTP_SCHEME_VALID(hu->scheme)) {
        // must destroy as it's a create function
        tt_uri_destroy(u);
        return TT_E_BADARG;
    }

    tt_queue_attr_default(&q_attr);
    q_attr.obj_per_frame = 16;

    tt_queue_init(&hu->path_param, sizeof(__hu_param_t), &q_attr);
    tt_queue_init(&hu->query_param, sizeof(__hu_param_t), &q_attr);
    tt_fpath_init(&hu->path, '/');
    hu->path_modified = TT_FALSE;
    hu->query_modified = TT_FALSE;

    if (tt_uri_get_path(u)[0] != 0) {
        TT_DO_G(fail,
                __hu_parse_path(hu,
                                tt_blobex_addr(&u->path),
                                tt_blobex_len(&u->path) - 1));
    }

    if (tt_uri_get_query(u)[0] != 0) {
        TT_DO_G(fail,
                __hu_parse_query(hu,
                                 tt_blobex_addr(&u->query),
                                 tt_blobex_len(&u->query) - 1));
    }

    return TT_SUCCESS;

fail:
    tt_http_uri_destroy(hu);
    return TT_FAIL;
}

void tt_http_uri_destroy(IN tt_http_uri_t *hu)
{
    __hupq_clear(&hu->path_param);
    tt_queue_destroy(&hu->path_param);

    __hupq_clear(&hu->query_param);
    tt_queue_destroy(&hu->query_param);

    tt_uri_destroy(&hu->u);
    tt_fpath_destroy(&hu->path);
}

void tt_http_uri_clear(IN tt_http_uri_t *hu)
{
    __hupq_clear(&hu->path_param);
    __hupq_clear(&hu->path_param);
    tt_uri_clear(&hu->u);
    tt_fpath_clear(&hu->path);
    hu->scheme = TT_HTTP_SCHEME_UNDEFINED;
    hu->path_modified = TT_FALSE;
    hu->query_modified = TT_FALSE;
}

tt_result_t tt_http_uri_set(IN tt_http_uri_t *hu,
                            IN tt_char_t *str,
                            IN tt_u32_t len)
{
    tt_result_t result;

    tt_http_uri_destroy(hu);

    result = tt_http_uri_create(hu, str, len);
    if (!TT_OK(result)) {
        // must keep it valid
        tt_http_uri_init(hu);
    }
    return result;
}

const tt_char_t *tt_http_uri_encode(IN tt_http_uri_t *hu)
{
    tt_uri_encode_table_t uet;

    __hu_update_path(hu);

    tt_uri_encode_table_default(&uet);
    uet.generic = tt_s_http_generic_enc_tbl;
    uet.path = tt_s_http_path_enc_tbl;
    uet.query = tt_s_http_query_enc_tbl;

    return tt_uri_encode(&hu->u, &uet);
}

tt_result_t tt_http_uri_encode2buf(IN tt_http_uri_t *hu, IN tt_buf_t *buf)
{
    tt_uri_encode_table_t uet;

    __hu_update_path(hu);

    tt_uri_encode_table_default(&uet);
    return tt_uri_encode2buf(&hu->u, buf, &uet);
}

tt_result_t tt_http_uri_set_scheme(IN tt_http_uri_t *hu,
                                   IN tt_http_scheme_t scheme)
{
    TT_ASSERT(TT_HTTP_SCHEME_VALID(scheme));

    if (scheme == TT_HTTP_SCHEME_HTTP) {
        tt_blobex_set(&hu->u.scheme,
                      (tt_u8_t *)"http",
                      sizeof("http"),
                      TT_FALSE);
    } else if (scheme == TT_HTTP_SCHEME_HTTPS) {
        tt_blobex_set(&hu->u.scheme,
                      (tt_u8_t *)"https",
                      sizeof("https"),
                      TT_FALSE);
    } else {
        tt_blobex_set(&hu->u.scheme, NULL, 0, TT_FALSE);
    }

    hu->scheme = scheme;
    return TT_SUCCESS;
}

tt_kv_t *tt_http_uri_find_path_param(IN tt_http_uri_t *hu,
                                     IN const tt_char_t *name)
{
    __hu_param_t *hp;

    if (name[0] == 0) {
        return NULL;
    }

    hp = __hupq_find_n(&hu->path_param,
                       (tt_char_t *)name,
                       (tt_u32_t)tt_strlen(name));
    if (hp) {
        return &hp->name_val;
    } else {
        return NULL;
    }
}

tt_result_t tt_http_uri_add_path_param(IN tt_http_uri_t *hu,
                                       IN const tt_char_t *name)
{
    hu->path_modified = TT_TRUE;
    return __hupq_add_n(&hu->path_param,
                        (tt_char_t *)name,
                        (tt_u32_t)tt_strlen(name),
                        TT_TRUE);
}

tt_result_t tt_http_uri_add_path_param_nv(IN tt_http_uri_t *hu,
                                          IN const tt_char_t *name,
                                          IN const tt_char_t *value)
{
    hu->path_modified = TT_TRUE;
    return __hupq_add_nv(&hu->path_param,
                         (tt_char_t *)name,
                         (tt_u32_t)tt_strlen(name),
                         TT_TRUE,
                         (tt_char_t *)value,
                         (tt_u32_t)tt_strlen(value),
                         TT_TRUE);
}

tt_bool_t tt_http_uri_remove_path_param(IN tt_http_uri_t *hu,
                                        IN const tt_char_t *name)
{
    __hu_param_t *hp = __hupq_find_n(&hu->path_param,
                                     (tt_char_t *)name,
                                     (tt_u32_t)tt_strlen(name));
    if (hp != NULL) {
        hp->removed = TT_TRUE;
        hu->path_modified = TT_TRUE;
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t tt_http_uri_remove_path_param_nv(IN tt_http_uri_t *hu,
                                           IN const tt_char_t *name,
                                           IN const tt_char_t *value)
{
    __hu_param_t *hp = __hupq_find_nv(&hu->path_param,
                                      (tt_char_t *)name,
                                      (tt_u32_t)tt_strlen(name),
                                      (tt_char_t *)value,
                                      (tt_u32_t)tt_strlen(value));
    if (hp != NULL) {
        hp->removed = TT_TRUE;
        hu->path_modified = TT_TRUE;
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_kv_t *tt_http_uri_find_query_param(IN tt_http_uri_t *hu,
                                      IN const tt_char_t *name)
{
    __hu_param_t *hp;

    if (name[0] == 0) {
        return NULL;
    }

    hp = __hupq_find_n(&hu->query_param,
                       (tt_char_t *)name,
                       (tt_u32_t)tt_strlen(name));
    if (hp) {
        return &hp->name_val;
    } else {
        return NULL;
    }
}

tt_result_t tt_http_uri_add_query_param(IN tt_http_uri_t *hu,
                                        IN const tt_char_t *name)
{
    hu->query_modified = TT_TRUE;
    return __hupq_add_n(&hu->query_param,
                        (tt_char_t *)name,
                        (tt_u32_t)tt_strlen(name),
                        TT_TRUE);
}

tt_result_t tt_http_uri_add_query_param_nv(IN tt_http_uri_t *hu,
                                           IN const tt_char_t *name,
                                           IN const tt_char_t *value)
{
    hu->query_modified = TT_TRUE;
    return __hupq_add_nv(&hu->query_param,
                         (tt_char_t *)name,
                         (tt_u32_t)tt_strlen(name),
                         TT_TRUE,
                         (tt_char_t *)value,
                         (tt_u32_t)tt_strlen(value),
                         TT_TRUE);
}

tt_bool_t tt_http_uri_remove_query_param(IN tt_http_uri_t *hu,
                                         IN const tt_char_t *name)
{
    __hu_param_t *hp = __hupq_find_n(&hu->query_param,
                                     (tt_char_t *)name,
                                     (tt_u32_t)tt_strlen(name));
    if (hp != NULL) {
        hp->removed = TT_TRUE;
        hu->query_modified = TT_TRUE;
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_bool_t tt_http_uri_remove_query_param_nv(IN tt_http_uri_t *hu,
                                            IN const tt_char_t *name,
                                            IN const tt_char_t *value)
{
    __hu_param_t *hp = __hupq_find_nv(&hu->query_param,
                                      (tt_char_t *)name,
                                      (tt_u32_t)tt_strlen(name),
                                      (tt_char_t *)value,
                                      (tt_u32_t)tt_strlen(value));
    if (hp != NULL) {
        hp->removed = TT_TRUE;
        hu->query_modified = TT_TRUE;
        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}

tt_http_scheme_t __hu_parse_scheme(IN tt_uri_t *u)
{
    const tt_char_t *p;

    p = tt_uri_get_opaque(u);
    if (p[0] != 0) {
        // can not be opaque
        return TT_HTTP_SCHEME_NUM;
    }

    p = tt_uri_get_scheme(u);
    if (p[0] != 0) {
        if (tt_strcmp(p, "http") == 0) {
            return TT_HTTP_SCHEME_HTTP;
        } else if (tt_strcmp(p, "https") == 0) {
            return TT_HTTP_SCHEME_HTTPS;
        } else {
            // not http uri
            return TT_HTTP_SCHEME_NUM;
        }
    } else {
        // no scheme
        return TT_HTTP_SCHEME_UNDEFINED;
    }
}

tt_result_t __hu_parse_path(IN tt_http_uri_t *hu,
                            IN tt_char_t *path,
                            IN tt_u32_t path_len)
{
    tt_char_t *p, *end;

    p = tt_memchr(path, ';', path_len);
    if (p == NULL) {
        TT_DO(tt_fpath_set(&hu->path, path, path_len));
    } else if (p > path) {
        TT_DO(tt_fpath_set(&hu->path, path, (tt_u32_t)(p - path)));
        ++p;
    } else {
        // empty path
        TT_ASSERT(p == path);
        tt_fpath_clear(&hu->path);
        ++p;
    }

    end = path + path_len;
    TT_ASSERT((p == NULL) || (p <= end));
    if ((p != NULL) && (p < end)) {
        TT_DO(__hu_parse_param(&hu->path_param, p, (tt_u32_t)(end - p)));
        // do not clear path, as it's still referenced by path_param
    }

    return TT_SUCCESS;
}

tt_result_t __hu_parse_query(IN tt_http_uri_t *hu,
                             IN tt_char_t *query,
                             IN tt_u32_t query_len)
{
    return __hu_parse_param(&hu->query_param, query, query_len);
    // do not clear path, as it's still referenced by query_param
}

tt_result_t __hu_parse_param(IN tt_queue_t *q,
                             IN tt_char_t *param,
                             IN tt_u32_t param_len)
{
    tt_char_t *p, *end, *prev;
    tt_u32_t n;

    p = param;
    end = param + param_len;
    prev = param;
    n = param_len;
    while (p < end) {
        p = tt_memchr(p, '&', n);
        if (p == NULL) {
            break;
        }
        TT_DO(__hupq_parse_add(q, prev, (tt_u32_t)(p - prev)));
        ++p;
        prev = p;
        TT_ASSERT(p <= end);
        n = (tt_u32_t)(end - p);
    }
    TT_ASSERT(prev <= end);
    if (prev < end) {
        TT_DO(__hupq_parse_add(q, prev, (tt_u32_t)(end - prev)));
    }

    return TT_SUCCESS;
}

void __hupq_clear(IN tt_queue_t *q)
{
    __hu_param_t hp;
    while (TT_OK(tt_queue_pop_head(q, &hp))) {
        __hup_destroy(&hp);
    }
}

tt_result_t __hupq_parse_add(IN tt_queue_t *q, IN tt_char_t *p, IN tt_u32_t len)
{
    __hu_param_t hp;
    tt_kv_t *kv = &hp.name_val;
    tt_char_t *val;

    __hup_init(&hp);

    val = tt_memchr(p, '=', len);
    if (val != NULL) {
        tt_char_t *end = p + len;
        __hup_set_name(&hp, (tt_u8_t *)p, (tt_u32_t)(val - p), TT_FALSE);
        ++val;
        if (val < end) {
            __hup_set_val(&hp, (tt_u8_t *)val, (tt_u32_t)(end - val), TT_FALSE);
        }
    } else {
        __hup_set_name(&hp, (tt_u8_t *)p, len, TT_FALSE);
    }

    return tt_queue_push_tail(q, &hp);
}

tt_result_t __hupq_add_n(IN tt_queue_t *q,
                         IN tt_char_t *name,
                         IN tt_u32_t len,
                         IN tt_bool_t owner)
{
    __hu_param_t hp;
    __hup_init(&hp);

    TT_DO_G(fail, __hup_set_name(&hp, (tt_u8_t *)name, len, owner));
    TT_DO_G(fail, tt_queue_push_tail(q, &hp));
    return TT_SUCCESS;

fail:
    __hup_destroy(&hp);
    return TT_FAIL;
}

tt_result_t __hupq_add_nv(IN tt_queue_t *q,
                          IN tt_char_t *name,
                          IN tt_u32_t name_len,
                          IN tt_bool_t name_owner,
                          IN tt_char_t *val,
                          IN tt_u32_t val_len,
                          IN tt_bool_t val_owner)
{
    __hu_param_t hp;
    __hup_init(&hp);

    TT_DO_G(fail, __hup_set_name(&hp, (tt_u8_t *)name, name_len, name_owner));
    TT_DO_G(fail, __hup_set_val(&hp, (tt_u8_t *)val, val_len, val_owner));
    TT_DO_G(fail, tt_queue_push_tail(q, &hp));
    return TT_SUCCESS;

fail:
    __hup_destroy(&hp);
    return TT_FAIL;
}

__hu_param_t *__hupq_find_n(IN tt_queue_t *q,
                            IN tt_char_t *name,
                            IN tt_u32_t len)
{
    tt_queue_iter_t i;
    __hu_param_t *hp;

    tt_queue_iter(q, &i);
    while ((hp = tt_queue_iter_next(&i)) != NULL) {
        if (!hp->removed &&
            (tt_blobex_memcmp(&hp->name_val.key, (tt_u8_t *)name, len) == 0)) {
            return hp;
        }
    }
    return NULL;
}

__hu_param_t *__hupq_find_nv(IN tt_queue_t *q,
                             IN tt_char_t *name,
                             IN tt_u32_t name_len,
                             IN tt_char_t *val,
                             IN tt_u32_t val_len)
{
    tt_queue_iter_t i;
    __hu_param_t *hp;

    tt_queue_iter(q, &i);
    while ((hp = tt_queue_iter_next(&i)) != NULL) {
        if (!hp->removed &&
            (tt_blobex_memcmp(&hp->name_val.key, (tt_u8_t *)name, name_len) ==
             0) &&
            (tt_blobex_memcmp(&hp->name_val.val, (tt_u8_t *)val, val_len) ==
             0)) {
            return hp;
        }
    }
    return NULL;
}

void __hupg_own(IN tt_queue_t *q)
{
    tt_queue_iter_t i;
    __hu_param_t *hp;

    tt_queue_iter(q, &i);
    while ((hp = tt_queue_iter_next(&i)) != NULL) {
        if (hp->removed) {
            tt_kv_clear(&hp->name_val);
        } else {
            if (!TT_OK(tt_kv_own(&hp->name_val))) {
                TT_FATAL("lost a param");
            }
        }
    }
}

tt_result_t __hu_update_path(IN tt_http_uri_t *hu)
{
    tt_char_t *p, *dst;
    tt_u32_t n = 0, len;
    tt_queue_t *q;

    if (!hu->path_modified) {
        return TT_SUCCESS;
    }

    hu->u.uri_modified = TT_TRUE;

    __hupg_own(&hu->path_param);

    // ========================================
    // compute encode length
    // ========================================

    p = (tt_char_t *)tt_fpath_cstr(&hu->path);
    if (p[0] != 0) {
        len = tt_string_len(&hu->path.path);
        n += __percent_encode_len(p, len, tt_s_http_path_enc_tbl);
    }

    q = &hu->path_param;
    if (!tt_queue_empty(q)) {
        n += 1; // ";"
        n += __encode_q_len(q, tt_s_http_path_enc_tbl);
    }

    if (n > 0) {
        TT_DO(tt_blobex_set(&hu->u.path, NULL, n + 1, TT_TRUE));
    } else {
        tt_blobex_clear(&hu->u.path);
        return TT_SUCCESS;
    }

    // ========================================
    // encode
    // ========================================

    dst = (tt_char_t *)__BLOBEX_ADDR(&hu->u.path);

    p = (tt_char_t *)tt_fpath_cstr(&hu->path);
    if (p[0] != 0) {
        len = tt_string_len(&hu->path.path);
        dst += __percent_encode(p, len, tt_s_http_path_enc_tbl, dst);
    }

    q = &hu->path_param;
    if (!tt_queue_empty(q)) {
        *dst++ = ';';
        dst += __encode_q(q, tt_s_http_path_enc_tbl, dst);
    }

    *dst++ = 0;

    TT_ASSERT(dst ==
              (tt_char_t *)__BLOBEX_ADDR(&hu->u.path) +
                  __BLOBEX_LEN(&hu->u.path));

    return TT_SUCCESS;
}

tt_result_t __hu_update_query(IN tt_http_uri_t *hu)
{
    if (!hu->query_modified) {
        return TT_SUCCESS;
    }

    return TT_SUCCESS;
}

tt_u32_t __encode_q_len(IN tt_queue_t *q, IN tt_char_t *t)
{
    tt_queue_iter_t i;
    __hu_param_t *hp;
    tt_u32_t n = 0;

    tt_queue_iter(q, &i);
    while ((hp = tt_queue_iter_next(&i)) != NULL) {
        if (!hp->removed) {
            tt_blobex_t *bex;

            bex = &hp->name_val.key;
            if (__BLOBEX_ADDR(bex) != NULL) {
                TT_ASSERT(__BLOBEX_LEN(bex) != 0);
                n += __percent_encode_len((tt_char_t *)__BLOBEX_ADDR(bex),
                                          __BLOBEX_LEN(bex),
                                          t);
            }

            bex = &hp->name_val.val;
            if (__BLOBEX_ADDR(bex) != NULL) {
                TT_ASSERT(__BLOBEX_LEN(bex) != 0);
                n += 1; // "="
                n += __percent_encode_len((tt_char_t *)__BLOBEX_ADDR(bex),
                                          __BLOBEX_LEN(bex),
                                          t);
            }

            n += 1; // "&"
        }
    }
    if (n > 0) {
        // remove terminating "&"
        --n;
    }

    return n;
}

tt_u32_t __encode_q(IN tt_queue_t *q, IN tt_char_t *t, OUT tt_char_t *dst)
{
    tt_queue_iter_t i;
    __hu_param_t *hp;
    tt_char_t *p = dst;
    tt_u32_t n = 0;

    tt_queue_iter(q, &i);
    while ((hp = tt_queue_iter_next(&i)) != NULL) {
        if (!hp->removed) {
            tt_blobex_t *bex;

            bex = &hp->name_val.key;
            if (__BLOBEX_ADDR(bex) != NULL) {
                TT_ASSERT(__BLOBEX_LEN(bex) != 0);
                p += __percent_encode((tt_char_t *)__BLOBEX_ADDR(bex),
                                      __BLOBEX_LEN(bex),
                                      t,
                                      p);
            }

            bex = &hp->name_val.val;
            if (__BLOBEX_ADDR(bex) != NULL) {
                TT_ASSERT(__BLOBEX_LEN(bex) != 0);
                *p++ = '=';
                p += __percent_encode((tt_char_t *)__BLOBEX_ADDR(bex),
                                      __BLOBEX_LEN(bex),
                                      t,
                                      p);
            }

            *p++ = '&';
        }
    }
    if (p > dst) {
        // remove terminating "&"
        --p;
    }

    return (tt_u32_t)(p - dst);
}

// ========================================
// http uri param
// ========================================

tt_kv_t *tt_http_uri_param_next(IN tt_http_uri_param_iter_t *iter)
{
    __hu_param_t *hp;
    while ((hp = tt_queue_iter_next(&iter->iter)) != NULL) {
        if (!hp->removed) {
            return &hp->name_val;
        }
    }
    return NULL;
}
