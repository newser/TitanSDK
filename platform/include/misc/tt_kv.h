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
@file tt_kv.h
@brief key value pair

this file defines key value blobex pair api
*/

#ifndef __TT_KV__
#define __TT_KV__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blobex.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_kv_s
{
    tt_blobex_t key;
    tt_blobex_t val;
} tt_kv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_kv_init(IN tt_kv_t *kv)
{
    tt_blobex_init(&kv->key, NULL, 0);
    tt_blobex_init(&kv->val, NULL, 0);
}

tt_inline void tt_kv_destroy(IN tt_kv_t *kv)
{
    tt_blobex_destroy(&kv->key);
    tt_blobex_destroy(&kv->val);
}

tt_inline void tt_kv_clear(IN tt_kv_t *kv)
{
    tt_blobex_clear(&kv->key);
    tt_blobex_clear(&kv->val);
}

tt_inline tt_result_t tt_kv_own(IN tt_kv_t *kv)
{
    if (TT_OK(tt_blobex_own(&kv->key)) && TT_OK(tt_blobex_own(&kv->val))) {
        return TT_SUCCESS;
    } else {
        tt_kv_clear(kv);
        return TT_E_NOMEM;
    }
}

tt_inline tt_result_t tt_kv_set_key(IN tt_kv_t *kv,
                                    IN OPT tt_u8_t *addr,
                                    IN tt_u32_t len,
                                    IN tt_bool_t owner)
{
    return tt_blobex_set(&kv->key, addr, len, owner);
}

tt_inline tt_result_t tt_kv_set_val(IN tt_kv_t *kv,
                                    IN OPT tt_u8_t *addr,
                                    IN tt_u32_t len,
                                    IN tt_bool_t owner)
{
    return tt_blobex_set(&kv->val, addr, len, owner);
}

#endif /* __TT_KV__ */
