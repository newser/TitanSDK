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
@file tt_distinguished_name.h
@brief distinguished name

this file defines distinguished name APIs
*/

#ifndef __TT_DISTINGUSHED_NAME__
#define __TT_DISTINGUSHED_NAME__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_RDN_DOMAIN,
    TT_RDN_COMMON_NAME,
    TT_RDN_ORG_UNIT,
    TT_RDN_ORG,
    TT_RDN_STREET,
    TT_RDN_LOCALITY,
    TT_RDN_STATE,
    TT_RDN_COUNTRY,
    TT_RDN_UID,

    TT_RDN_TYPE_NUM
} tt_rdn_type_t;
#define TT_RDN_TYPE_VALID(t) ((t) < TT_RDN_TYPE_NUM)

typedef struct tt_rdn_s
{
    tt_lnode_t node;

    tt_rdn_type_t type;
    const tt_char_t *value;
} tt_rdn_t;

typedef struct tt_distname_s
{
    tt_list_t rdn_list;
} tt_distname_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_distname_init(IN tt_distname_t *dn);

tt_export void tt_distname_destroy(IN tt_distname_t *dn);

tt_export tt_result_t tt_distname_add_rdn(IN tt_distname_t *dn,
                                          IN tt_rdn_type_t type,
                                          IN const tt_char_t *value);

#endif // __TT_DISTINGUSHED_NAME__
