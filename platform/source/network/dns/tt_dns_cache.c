/* Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <network/dns/tt_dns_cache.h>

#include <algorithm/ptr/tt_ptr_queue.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <network/dns/tt_dns_rr.h>
#include <os/tt_fiber.h>

#if TT_ENV_OS_IS_MACOS || TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_LINUX
#include <netdb.h> // struct hostent
#endif

#include <ares.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef enum {
    __RR_A,
    __RR_AAAA,

    __RR_TYPE_NUM
} __rr_type_t;

typedef struct
{
    tt_s64_t ttl;
    union
    {
        tt_dns_a_t *a;
        tt_dns_aaaa_t *aaaa;
        void *p;
    };
    tt_ptrq_t waiting;
} __rr_t;

typedef struct
{
    const tt_char_t *name;
    __rr_t rr[__RR_TYPE_NUM];
    tt_hnode_t hnode;
    tt_u32_t name_len;
} __domain_t;

typedef struct
{
    tt_fiber_t *src;
    union
    {
        tt_dns_a_t **p_a;
        tt_dns_aaaa_t **p_aaaa;
    };
} __dns_get_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __domain_t *__domain_create(IN const tt_char_t *name,
                                   IN tt_u32_t name_len);

static void __domain_destroy(IN __domain_t *dm);

static void __rr_init(IN __rr_t *rr);

static void __rr_a_set(IN __rr_t *rr, IN tt_s64_t ttl, IN tt_dns_a_t *a);

static void __rr_a_clear(IN __rr_t *rr);

static tt_result_t __rr_a_wait(IN __rr_t *rr, IN void *p);

static void __rr_a_notify(IN __rr_t *rr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_dns_cache_create(IN tt_dns_cache_t *dc,
                                IN OPT tt_dns_cache_attr_t *attr)
{
    tt_dns_cache_attr_t __attr;

    TT_ASSERT(dc != NULL);

    if (attr == NULL) {
        tt_dns_cache_attr_default(&__attr);
        attr = &__attr;
    }

    dc->d = tt_dns_create(&attr->dns_attr);
    if (dc->d == NULL) {
        TT_ERROR("fail to create dns resolver");
        return TT_FAIL;
    }

    if (!TT_OK(tt_hmap_create(&dc->map, attr->slot_num, &attr->map_attr))) {
        TT_ERROR("fail to create dns cache map");
        tt_dns_destroy(dc->d);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_dns_cache_destroy(IN tt_dns_cache_t *dc)
{
    TT_ASSERT(dc != NULL);
}

void tt_dns_cache_attr_default(IN tt_dns_cache_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_dns_attr_default(&attr->dns_attr);

    attr->slot_num = 11;
    tt_hmap_attr_default(&attr->map_attr);
}

tt_dns_a_t *tt_dns_get_a(IN tt_dns_cache_t *dc, IN const tt_char_t *name)
{
    tt_u32_t name_len;
    tt_hnode_t *hn;
    __domain_t *dm;
    __dns_get_t get_a;
    tt_dns_a_t *a;

    TT_ASSERT(dc != NULL);
    TT_ASSERT(name != NULL);

    name_len = tt_strlen(name);
    hn = tt_hmap_find(&dc->map, (tt_u8_t *)name, name_len);
    if (hn != NULL) {
        tt_dns_a_t *a;

        dm = TT_CONTAINER(hn, __domain_t, hnode);
        a = dm->rr[__RR_A].a;
        if (a != NULL) {
            return a;
        }
    } else {
        dm = NULL;
    }

    if (dm == NULL) {
        dm = __domain_create(name, name_len);
        if (dm == NULL) {
            return NULL;
        }

        if (!TT_OK(tt_hmap_add(&dc->map,
                               (tt_u8_t *)dm->name,
                               name_len,
                               &dm->hnode))) {
            __domain_destroy(dm);
            return NULL;
        }
    }

    get_a.src = tt_current_fiber();
    a = NULL;
    get_a.p_a = &a;
    // if (!TT_OK(tt_ptrq_push(&dm->waiting_fiber, &get_a))) {
    //    return NULL;
    //}
    tt_fiber_suspend();
    return a;
}

__domain_t *__domain_create(IN const tt_char_t *name, IN tt_u32_t name_len)
{
    __domain_t *dm;
    tt_ptrq_attr_t q_attr;

    dm = tt_malloc(sizeof(__domain_t) + name_len + 1);
    if (dm == NULL) {
        TT_ERROR("no mem for dns domain");
        return NULL;
    }

    dm->name = TT_PTR_INC(const tt_char_t, dm, sizeof(__domain_t));
    tt_memset(dm->rr, 0, sizeof(dm->rr));

    tt_ptrq_attr_default(&q_attr);
    q_attr.ptr_per_frame = 16;
    // tt_ptrq_init(&dm->waiting_fiber, &q_attr);

    tt_hnode_init(&dm->hnode);
    dm->name_len = name_len;

    tt_memcpy((tt_u8_t *)dm->name, name, name_len + 1);

    return dm;
}

void __domain_destroy(IN __domain_t *dm)
{
    __dns_get_t *fb;
    tt_dns_a_t *a;
    tt_dns_aaaa_t *aaaa;

    /*while ((fb = tt_ptrq_pop(&dm->waiting_fiber)) != NULL) {
        // todo: set result
        tt_fiber_resume(fb, TT_FALSE);
    }*/

    a = dm->rr[__RR_A].a;
    if (a != NULL) {
        // tt_dns_a_destroy(a);
    }

    aaaa = dm->rr[__RR_AAAA].aaaa;
    if (aaaa != NULL) {
        // tt_dns_aaaa_destroy(aaaa);
    }

    tt_free(dm);
}
