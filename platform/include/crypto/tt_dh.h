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
@file tt_dh.h
@brief crypto: dh

this file defines crypto dh APIs
*/

#ifndef __TT_DH__
#define __TT_DH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <dhm.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    mbedtls_dhm_context ctx;
} tt_dh_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_dh_init(IN tt_dh_t *dh);

tt_export void tt_dh_destroy(IN tt_dh_t *dh);

tt_export tt_result_t tt_dh_load_param(IN tt_dh_t *dh, IN tt_u8_t *param,
                                       IN tt_u32_t len);

tt_export tt_result_t tt_dh_load_param_file(IN tt_dh_t *dh,
                                            IN const tt_char_t *path);

tt_inline tt_u32_t tt_dh_size(IN tt_dh_t *dh)
{
    return (tt_u32_t)dh->ctx.len;
}

// len should be at least group size, 128 bytes if dh group is of 1024 bits
tt_export tt_result_t tt_dh_generate_pub(IN tt_dh_t *dh, IN tt_u32_t priv_size,
                                         OUT tt_u8_t *pub, IN OUT tt_u32_t len);

tt_export tt_result_t tt_dh_get_pub(IN tt_dh_t *dh, IN tt_bool_t local,
                                    OUT tt_u8_t *pub, IN tt_u32_t len);

tt_export tt_result_t tt_dh_set_pub(IN tt_dh_t *dh, IN tt_u8_t *pub,
                                    IN tt_u32_t len);

tt_export tt_result_t tt_dh_derive(IN tt_dh_t *dh, OUT tt_u8_t *secret,
                                   IN OUT tt_u32_t *len);

tt_export tt_result_t tt_dh_get_secret(IN tt_dh_t *dh, OUT tt_u8_t *secret,
                                       IN tt_u32_t len);

#endif /* __TT_DH__ */
