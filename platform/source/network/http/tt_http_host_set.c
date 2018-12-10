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

#include <network/http/tt_http_host_set.h>

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

static void __destroy_hosts(IN tt_dlist_t *dl);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_http_hostset_init(IN tt_http_hostset_t *hs)
{
    TT_ASSERT(hs != NULL);

    hs->default_host = NULL;

    tt_dlist_init(&hs->hosts);
}

void tt_http_hostset_destroy(IN tt_http_hostset_t *hs)
{
    TT_ASSERT(hs != NULL);

    if (hs->default_host != NULL) {
        tt_http_host_destroy(hs->default_host);
    }

    __destroy_hosts(&hs->hosts);
}

tt_http_host_t *tt_http_hostset_find_n(IN tt_http_hostset_t *hs,
                                       IN tt_char_t *name,
                                       IN tt_u32_t name_len)
{
    tt_dnode_t *dn = tt_dlist_head(&hs->hosts);
    while (dn != NULL) {
        tt_http_host_t *h = TT_CONTAINER(dn, tt_http_host_t, dnode);

        dn = dn->next;

        if (h->match(h, name, name_len)) {
            return h;
        }
    }
    return hs->default_host;
}

void __destroy_hosts(IN tt_dlist_t *dl)
{
    tt_dnode_t *dn;
    while ((dn = tt_dlist_pop_head(dl)) != NULL) {
        tt_http_host_destroy(TT_CONTAINER(dn, tt_http_host_t, dnode));
    }
}
