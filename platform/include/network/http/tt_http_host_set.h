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
 * distributed under the License h distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_http_hostset_set.h
@brief http host set

this file defines http host set
*/

#ifndef __TT_HTTP_HOST_SET__
#define __TT_HTTP_HOST_SET__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>
#include <network/http/tt_http_host.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_http_hostset_s
{
    tt_http_host_t *default_host;
    tt_dlist_t hosts;
} tt_http_hostset_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_http_hostset_init(IN tt_http_hostset_t *hs);

tt_export void tt_http_hostset_destroy(IN tt_http_hostset_t *hs);

tt_inline void tt_http_hostset_add(IN tt_http_hostset_t *hs,
                                   IN tt_http_host_t *host)
{
    tt_dlist_push_tail(&hs->hosts, &host->dnode);
}

tt_inline void tt_http_hostset_set_default(IN tt_http_hostset_t *hs,
                                           IN tt_http_host_t *host)
{
    if (hs->default_host != NULL) { tt_http_host_destroy(hs->default_host); }
    hs->default_host = host;
}

tt_export tt_http_host_t *tt_http_hostset_match_n(IN tt_http_hostset_t *hs,
                                                  IN tt_char_t *name,
                                                  IN tt_u32_t name_len);

tt_inline tt_http_host_t *tt_http_hostset_match(IN tt_http_hostset_t *hs,
                                                IN const tt_char_t *name)
{
    return tt_http_hostset_match_n(hs, (tt_char_t *)name,
                                   (tt_u32_t)tt_strlen(name));
}

tt_export tt_http_hostset_t *tt_current_http_hostset(IN tt_bool_t create);

#endif /* __TT_HTTP_HOST_SET__ */
