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

#include <network/dns/tt_dns_rr.h>

#include <os/tt_fiber.h>
#include <time/tt_time_reference.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_RR TT_ASSERT

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_fiber_t *fb;
    tt_dnode_t node;
} __rr_wait_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_dns_a_t *__a_copy(IN tt_dns_a_t *src);

static tt_dns_aaaa_t *__aaaa_copy(IN tt_dns_aaaa_t *src);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_dns_rr_init(IN tt_dns_rr_t *drr, IN tt_dns_type_t type)
{
    drr->ttl = 0;
    tt_dlist_init(&drr->waiting);
    tt_dns_rrlist_init(&drr->rrlist, type);
}

void tt_dns_rr_clear(IN tt_dns_rr_t *drr)
{
    drr->ttl = 0;

    // there should be no fiber waiting on it
    TT_ASSERT_RR(tt_dlist_empty(&drr->waiting));

    tt_dns_rrlist_clear(&drr->rrlist);
}

tt_dns_rrlist_t *tt_dns_rr_wait(IN tt_dns_rr_t *drr)
{
    __rr_wait_t wait;

    // wait because there is no rr
    TT_ASSERT_RR(tt_dlist_empty(&drr->rrlist.rr));

    wait.fb = tt_current_fiber();
    tt_dnode_init(&wait.node);

    tt_dlist_push_tail(&drr->waiting, &wait.node);
    tt_fiber_suspend();
    return &drr->rrlist;
}

void tt_dns_rr_noitfy(IN tt_dns_rr_t *drr)
{
    tt_dnode_t *node;
    while ((node = tt_dlist_pop_head(&drr->waiting)) != NULL) {
        __rr_wait_t *wait = TT_CONTAINER(node, __rr_wait_t, node);
        tt_fiber_resume(wait->fb, TT_FALSE);
    }
}

// ========================================
// rr list
// ========================================

void tt_dns_rrlist_clear(IN tt_dns_rrlist_t *rrl)
{
    if (rrl->type == TT_DNS_A_IN) {
        tt_dns_a_clear(rrl);
    } else {
        TT_ASSERT(rrl->type == TT_DNS_AAAA_IN);
        tt_dns_aaaa_clear(rrl);
    }
}

// RR: A
tt_dns_a_t *tt_dns_a_head(IN tt_dns_rrlist_t *rrl)
{
    tt_dnode_t *node;

    TT_ASSERT(rrl->type == TT_DNS_A_IN);

    node = tt_dlist_head(&rrl->rr);
    if (node != NULL) {
        return TT_CONTAINER(node, tt_dns_a_t, node);
    } else {
        return NULL;
    }
}

tt_dns_a_t *tt_dns_a_next(IN tt_dns_a_t *a)
{
    tt_dnode_t *node = a->node.next;
    if (node != NULL) {
        return TT_CONTAINER(node, tt_dns_a_t, node);
    } else {
        return NULL;
    }
}

tt_result_t tt_dns_a_copy(IN tt_dns_rrlist_t *dst, IN tt_dns_rrlist_t *src)
{
    tt_dns_a_t *a = tt_dns_a_head(src);
    TT_ASSERT_RR((dst->type == TT_DNS_A_IN) && (src->type == TT_DNS_A_IN));
    while (a != NULL) {
        tt_dns_a_t *new_a = __a_copy(a);
        if (new_a != NULL) {
            tt_dlist_push_tail(&dst->rr, &new_a->node);
        } else {
            TT_ERROR("no mem for copying dns a");
        }

        a = tt_dns_a_next(a);
    }
    return TT_SUCCESS;
}

void tt_dns_a_clear(IN tt_dns_rrlist_t *rrl)
{
    tt_dns_a_t *a = tt_dns_a_head(rrl);
    while (a != NULL) {
        tt_dns_a_t *next = tt_dns_a_next(a);
        tt_free(a);
        a = next;
    }
    tt_dlist_init(&rrl->rr);
}

// RR: AAAA
tt_dns_aaaa_t *tt_dns_aaaa_head(IN tt_dns_rrlist_t *rrl)
{
    tt_dnode_t *node;

    TT_ASSERT(rrl->type == TT_DNS_AAAA_IN);

    node = tt_dlist_head(&rrl->rr);
    if (node != NULL) {
        return TT_CONTAINER(node, tt_dns_aaaa_t, node);
    } else {
        return NULL;
    }
}

tt_dns_aaaa_t *tt_dns_aaaa_next(IN tt_dns_aaaa_t *aaaa)
{
    tt_dnode_t *node = aaaa->node.next;
    if (node != NULL) {
        return TT_CONTAINER(node, tt_dns_aaaa_t, node);
    } else {
        return NULL;
    }
}

tt_result_t tt_dns_aaaa_copy(IN tt_dns_rrlist_t *dst, IN tt_dns_rrlist_t *src)
{
    tt_dns_aaaa_t *aaaa = tt_dns_aaaa_head(src);
    while (aaaa != NULL) {
        tt_dns_aaaa_t *new_aaaa = __aaaa_copy(aaaa);
        if (new_aaaa != NULL) {
            tt_dlist_push_tail(&dst->rr, &new_aaaa->node);
        } else {
            TT_ERROR("no mem for copying dns aaaa");
        }

        aaaa = tt_dns_aaaa_next(aaaa);
    }
    return TT_SUCCESS;
}

void tt_dns_aaaa_clear(IN tt_dns_rrlist_t *rrl)
{
    tt_dns_aaaa_t *aaaa = tt_dns_aaaa_head(rrl);
    while (aaaa != NULL) {
        tt_dns_aaaa_t *next = tt_dns_aaaa_next(aaaa);
        tt_free(aaaa);
        aaaa = next;
    }
    tt_dlist_init(&rrl->rr);
}

tt_dns_a_t *__a_copy(IN tt_dns_a_t *src)
{
    tt_dns_a_t *dst = tt_malloc(sizeof(tt_dns_a_t));
    if (dst != NULL) {
        tt_dnode_init(&dst->node);
        tt_memcpy(&dst->addr, &src->addr, sizeof(tt_sktaddr_t));
    }
    return dst;
}

tt_dns_aaaa_t *__aaaa_copy(IN tt_dns_aaaa_t *src)
{
    tt_dns_aaaa_t *dst = tt_malloc(sizeof(tt_dns_aaaa_t));
    if (dst != NULL) {
        tt_dnode_init(&dst->node);
        tt_memcpy(&dst->addr, &src->addr, sizeof(tt_sktaddr_t));
    }
    return dst;
}
