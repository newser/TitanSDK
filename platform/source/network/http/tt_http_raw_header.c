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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_rawval_t *tt_http_rawval_create(IN tt_slab_t *slab,
                                        IN tt_char_t *val,
                                        IN tt_u32_t len,
                                        IN tt_bool_t owner)
{
    tt_http_rawval_t *rv = tt_slab_alloc(slab);
    if (rv == NULL) {
        return NULL;
    }
    tt_http_rawval_init(rv);

    if (TT_OK(tt_blobex_set(&rv->val, (tt_u8_t *)val, len, owner))) {
        return rv;
    } else {
        tt_slab_free(rv);
        return NULL;
    }
}

tt_http_rawval_t *tt_http_rawval_find_n(IN tt_dlist_t *dl,
                                        IN const tt_char_t *val,
                                        IN tt_u32_t len)
{
    tt_dnode_t *node = tt_dlist_head(dl);
    while (node != NULL) {
        tt_http_rawval_t *rv = TT_CONTAINER(node, tt_http_rawval_t, dnode);
        node = node->next;

        if (tt_blobex_memcmp(&rv->val, (tt_u8_t *)val, len) == 0) {
            return rv;
        }
    }
    return NULL;
}

tt_result_t tt_http_rawval_add_n(IN tt_slab_t *slab,
                                 IN tt_dlist_t *dl,
                                 IN const tt_char_t *val,
                                 IN tt_u32_t len)
{
    tt_http_rawval_t *rv =
        tt_http_rawval_create(slab, (tt_char_t *)val, len, TT_TRUE);
    if (rv == NULL) {
        return TT_FAIL;
    }

    tt_dlist_push_tail(dl, &rv->dnode);
    return TT_SUCCESS;
}

tt_http_rawhdr_t *tt_http_rawhdr_create(IN tt_slab_t *slab,
                                        IN tt_char_t *name,
                                        IN tt_u32_t len,
                                        IN tt_bool_t owner)
{
    tt_http_rawhdr_t *rh = tt_slab_alloc(slab);
    if (rh == NULL) {
        return NULL;
    }
    tt_http_rawhdr_init(rh);

    if (TT_OK(tt_blobex_set(&rh->name, (tt_u8_t *)name, len, owner))) {
        return rh;
    } else {
        tt_slab_free(rh);
        return NULL;
    }
}

void tt_http_rawhdr_destroy(IN tt_http_rawhdr_t *rh)
{
    tt_dnode_t *node;

    TT_ASSERT(!tt_dnode_in_dlist(&rh->dnode));

    while ((node = tt_dlist_pop_head(&rh->val)) != NULL) {
        tt_http_rawval_destroy(TT_CONTAINER(node, tt_http_rawval_t, dnode));
    }

    tt_blobex_destroy(&rh->name);

    tt_slab_free(rh);
}

tt_http_rawhdr_t *tt_http_rawhdr_find_n(IN tt_dlist_t *dl,
                                        IN const tt_char_t *name,
                                        IN tt_u32_t len)
{
    tt_dnode_t *node = tt_dlist_head(dl);
    while (node != NULL) {
        tt_http_rawhdr_t *rh = TT_CONTAINER(node, tt_http_rawhdr_t, dnode);
        node = node->next;

        if ((tt_blobex_len(&rh->name) == len) &&
            (tt_strnicmp(tt_blobex_addr(&rh->name), name, len) == 0)) {
            return rh;
        }
    }
    return NULL;
}

tt_u32_t tt_http_rawhdr_count_name_n(IN tt_dlist_t *dl,
                                     IN tt_char_t *name,
                                     IN tt_u32_t len)
{
    tt_dnode_t *node = tt_dlist_head(dl);
    tt_u32_t count = 0;
    while (node != NULL) {
        tt_http_rawhdr_t *rh = TT_CONTAINER(node, tt_http_rawhdr_t, dnode);
        node = node->next;

        if ((tt_blobex_len(&rh->name) == len) &&
            (tt_strnicmp(tt_blobex_addr(&rh->name), name, len) == 0)) {
            ++count;
        }
    }
    return count;
}
