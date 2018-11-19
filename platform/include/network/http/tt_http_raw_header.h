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
@file tt_http_raw_header.h
@brief http raw header

this file defines raw http header APIs
*/

#ifndef __TT_HTTP_RAW_HEADER__
#define __TT_HTTP_RAW_HEADER__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blobex.h>
#include <algorithm/tt_double_linked_list.h>
#include <memory/tt_slab.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_http_rawval_s
{
    tt_dnode_t dnode;
    tt_blobex_t val;
} tt_http_rawval_t;

typedef struct tt_http_rawhdr_s
{
    tt_dnode_t dnode;
    tt_dlist_t val;
    tt_blobex_t name;
} tt_http_rawhdr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// raw value
// ========================================

tt_inline void tt_http_rawval_init(IN tt_http_rawval_t *rv)
{
    tt_dnode_init(&rv->dnode);
    tt_blobex_init(&rv->val, NULL, 0);
}

tt_export tt_http_rawval_t *tt_http_rawval_create(IN tt_slab_t *slab,
                                                  IN tt_char_t *val,
                                                  IN tt_u32_t len,
                                                  IN tt_bool_t owner);

tt_inline void tt_http_rawval_destroy(IN tt_http_rawval_t *rv)
{
    TT_ASSERT(!tt_dnode_in_dlist(&rv->dnode));
    tt_blobex_destroy(&rv->val);
    tt_slab_free(rv);
}

tt_inline tt_result_t tt_http_rawval_append(IN tt_http_rawval_t *rv,
                                            IN const tt_char_t *val,
                                            IN tt_u32_t len)
{
    return tt_blobex_memcat(&rv->val, (tt_u8_t *)val, len);
}

tt_export tt_http_rawval_t *tt_http_rawval_find_n(IN tt_dlist_t *dl,
                                                  IN const tt_char_t *val,
                                                  IN tt_u32_t len);

tt_inline tt_http_rawval_t *tt_http_rawval_find(IN tt_dlist_t *dl,
                                                IN const tt_char_t *val)
{
    return tt_http_rawval_find_n(dl, val, tt_strlen(val));
}

tt_export tt_result_t tt_http_rawval_add_n(IN tt_slab_t *slab,
                                           IN tt_dlist_t *dl,
                                           IN const tt_char_t *val,
                                           IN tt_u32_t len);

tt_inline tt_result_t tt_http_rawval_add(IN tt_slab_t *slab,
                                         IN tt_dlist_t *dl,
                                         IN const tt_char_t *val)
{
    return tt_http_rawval_add_n(slab, dl, val, tt_strlen(val));
}

tt_export tt_bool_t tt_http_rawval_remove_n(IN tt_dlist_t *dl,
                                            IN const tt_char_t *val,
                                            IN tt_u32_t len);

tt_inline tt_bool_t tt_http_rawval_remove(IN tt_dlist_t *dl,
                                          IN const tt_char_t *val)
{
    return tt_http_rawval_remove_n(dl, val, tt_strlen(val));
}

// ========================================
// raw header
// ========================================

tt_inline void tt_http_rawhdr_init(IN tt_http_rawhdr_t *rh)
{
    tt_dnode_init(&rh->dnode);
    tt_dlist_init(&rh->val);
    tt_blobex_init(&rh->name, NULL, 0);
}

tt_export tt_http_rawhdr_t *tt_http_rawhdr_create(IN tt_slab_t *slab,
                                                  IN tt_char_t *name,
                                                  IN tt_u32_t len,
                                                  IN tt_bool_t owner);

tt_export void tt_http_rawhdr_destroy(IN tt_http_rawhdr_t *rh);

tt_inline tt_result_t tt_http_rawhdr_append_name(IN tt_http_rawhdr_t *rh,
                                                 IN const tt_char_t *name,
                                                 IN tt_u32_t len)
{
    return tt_blobex_memcat(&rh->name, (tt_u8_t *)name, len);
}

tt_export tt_http_rawhdr_t *tt_http_rawhdr_find_n(IN tt_dlist_t *dl,
                                                  IN const tt_char_t *name,
                                                  IN tt_u32_t len);

tt_inline tt_http_rawhdr_t *tt_http_rawhdr_find(IN tt_dlist_t *dl,
                                                IN const tt_char_t *name)
{
    return tt_http_rawhdr_find_n(dl, name, tt_strlen(name));
}

tt_inline void tt_http_rawhdr_add(IN tt_dlist_t *dl, IN tt_http_rawhdr_t *rh)
{
    tt_dlist_push_tail(dl, &rh->dnode);
}

tt_inline void tt_http_rawhdr_remove(IN tt_dlist_t *dl, IN tt_http_rawhdr_t *rh)
{
    tt_dlist_remove(dl, &rh->dnode);
}

tt_inline void tt_http_rawhdr_add_val(IN tt_http_rawhdr_t *rh,
                                      IN tt_http_rawval_t *rv)
{
    tt_dlist_push_tail(&rh->val, &rv->dnode);
}

tt_inline void tt_http_rawhdr_remove_val(IN tt_http_rawhdr_t *rh,
                                         IN tt_http_rawval_t *rv)
{
    tt_dlist_remove(&rh->val, &rv->dnode);
}

#endif /* __TT_HTTP_RAW_HEADER__ */
