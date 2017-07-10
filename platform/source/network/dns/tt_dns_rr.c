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

#include <network/dns/tt_dns_entry.h>
#include <os/tt_fiber.h>
#include <time/tt_time_reference.h>
#include <memory/tt_memory_alloc.h>

#if TT_ENV_OS_IS_MACOS
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#endif

// clang-format off
#define HAVE_CONFIG_H
#include <ares_setup.h>
#include <nameser.h>
#include <ares.h>
#include <ares_dns.h>
#include <ares_private.h>
// clang-format on

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_ASSERT_RR TT_ASSERT

#define __DE_OF(drr)                                                           \
    TT_CONTAINER(((drr) - ((drr)->rrl.type)), tt_dns_entry_t, rr)

#define __MAX_TTL (24 * 3600)

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

extern tt_dns_rrlist_t __empty_rrlist_a;

extern tt_dns_rrlist_t __empty_rrlist_aaaa;

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __ares_query(IN tt_dns_rr_t *drr, IN tt_dns_t d);

static void __a_clear_list(IN tt_dns_rrlist_t *rrl);

static tt_result_t __a_copy_list(IN tt_dns_rrlist_t *dst,
                                 IN tt_dns_rrlist_t *src);

static tt_dns_a_t *__a_copy(IN tt_dns_a_t *src);

static void __a_callback(IN void *arg,
                         IN int status,
                         IN int timeouts,
                         IN unsigned char *abuf,
                         IN int alen);

int __a_parse(IN const unsigned char *abuf,
              IN int alen,
              OUT tt_s64_t *ttl,
              OUT tt_dns_rrlist_t *rrl);

static void __aaaa_clear_list(IN tt_dns_rrlist_t *rrl);

static tt_result_t __aaaa_copy_list(IN tt_dns_rrlist_t *dst,
                                    IN tt_dns_rrlist_t *src);

static tt_dns_aaaa_t *__aaaa_copy(IN tt_dns_aaaa_t *src);

static void __aaaa_callback(IN void *arg,
                            IN int status,
                            IN int timeouts,
                            IN unsigned char *abuf,
                            IN int alen);

int __aaaa_parse(IN const unsigned char *abuf,
                 IN int alen,
                 OUT tt_s64_t *ttl,
                 OUT tt_dns_rrlist_t *rrl);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_dns_rr_init(IN tt_dns_rr_t *drr,
                    IN const tt_char_t *name,
                    IN tt_dns_type_t type)
{
    drr->ttl = TT_TIME_INFINITE;
    drr->name = name;
    drr->querying_fb = NULL;
    tt_dlist_init(&drr->waiting);
    tt_dns_rrlist_init(&drr->rrl, type);
}

void tt_dns_rr_destroy(IN tt_dns_rr_t *drr)
{
    // dns_rr is destroyed when there is long time that no one
    // query it, so there must be no fiber waiting
    TT_ASSERT(drr->querying_fb == NULL);
    TT_ASSERT(tt_dlist_empty(&drr->waiting));
    tt_dns_rr_clear(drr);
}

void tt_dns_rr_clear(IN tt_dns_rr_t *drr)
{
    drr->ttl = TT_TIME_INFINITE;
    tt_dns_rrlist_clear(&drr->rrl);
}

tt_dns_rrlist_t *tt_dns_rr_get(IN tt_dns_rr_t *drr, IN tt_dns_t d)
{
    tt_dns_rrlist_t *rrl;
    tt_fiber_t *fb;
    __rr_wait_t wait;

    if (drr->ttl < tt_time_ref()) {
        tt_dns_rr_clear(drr);
    }

    rrl = &drr->rrl;
    if (!tt_dns_rrlist_empty(rrl)) {
        return rrl;
    }

    fb = tt_current_fiber();
    if (drr->querying_fb == NULL) {
        drr->querying_fb = fb;
        __ares_query(drr, d);
        if (drr->querying_fb == NULL) {
            // callback has done querying
            goto done;
        }
    }

    // waiting for ongoing dns query
    wait.fb = fb;
    tt_dnode_init(&wait.node);
    tt_dlist_push_tail(&drr->waiting, &wait.node);
    tt_fiber_suspend();

done:
    tt_dns_entry_update_ttl(__DE_OF(drr), drr->ttl);
    return rrl;
}

void tt_dns_rr_set(IN tt_dns_rr_t *drr,
                   IN tt_s64_t ttl,
                   IN tt_dns_rrlist_t *rrl,
                   IN tt_bool_t notify)
{
    tt_dnode_t *node;

    TT_ASSERT_RR(tt_dns_rrlist_empty(&drr->rrl));
    drr->ttl = ttl;
    drr->querying_fb = NULL;
    tt_dns_rrlist_move(&drr->rrl, rrl);

    if (notify) {
        while ((node = tt_dlist_pop_head(&drr->waiting)) != NULL) {
            __rr_wait_t *wait = TT_CONTAINER(node, __rr_wait_t, node);
            tt_fiber_resume(wait->fb, TT_FALSE);
        }
    }
}

// ========================================
// rr list
// ========================================

void tt_dns_rrlist_clear(IN tt_dns_rrlist_t *rrl)
{
    if (rrl->type == TT_DNS_A_IN) {
        __a_clear_list(rrl);
    } else {
        TT_ASSERT(rrl->type == TT_DNS_AAAA_IN);
        __aaaa_clear_list(rrl);
    }
}

tt_result_t tt_dns_rrlist_copy(IN tt_dns_rrlist_t *dst, IN tt_dns_rrlist_t *src)
{
    tt_dns_type_t t = dst->type;
    TT_ASSERT(t == src->type);
    if (t == TT_DNS_A_IN) {
        return __a_copy_list(dst, src);
    } else {
        TT_ASSERT(t == TT_DNS_AAAA_IN);
        return __aaaa_copy_list(dst, src);
    }
}

tt_result_t tt_dns_rrlist_add_a(IN tt_dns_rrlist_t *rrl,
                                IN tt_sktaddr_ip32_t *ip)
{
    tt_dns_a_t *a;

    TT_ASSERT(rrl->type == TT_DNS_A_IN);

    a = tt_malloc(sizeof(tt_dns_a_t));
    if (a == NULL) {
        TT_ERROR("no mem for new a rr");
        return TT_FAIL;
    }

    tt_dnode_init(&a->node);
    tt_memcpy(&a->ip, ip, sizeof(tt_sktaddr_ip32_t));
    tt_dlist_push_tail(&rrl->rr, &a->node);
    return TT_SUCCESS;
}

tt_result_t tt_dns_rrlist_add_aaaa(IN tt_dns_rrlist_t *rrl,
                                   IN tt_sktaddr_ip128_t *ip)
{
    tt_dns_aaaa_t *aaaa;

    TT_ASSERT(rrl->type == TT_DNS_AAAA_IN);

    aaaa = tt_malloc(sizeof(tt_dns_aaaa_t));
    if (aaaa == NULL) {
        TT_ERROR("no mem for new aaaa rr");
        return TT_FAIL;
    }

    tt_dnode_init(&aaaa->node);
    tt_memcpy(&aaaa->ip, ip, sizeof(tt_sktaddr_ip128_t));
    tt_dlist_push_tail(&rrl->rr, &aaaa->node);
    return TT_SUCCESS;
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

void __ares_query(IN tt_dns_rr_t *drr, IN tt_dns_t d)
{
    switch (drr->rrl.type) {
        case TT_DNS_A_IN: {
            ares_query(d, drr->name, C_IN, T_A, __a_callback, drr);
        } break;
        case TT_DNS_AAAA_IN:
        default: {
            ares_query(d, drr->name, C_IN, T_AAAA, __aaaa_callback, drr);
        } break;
    }
}

void __a_clear_list(IN tt_dns_rrlist_t *rrl)
{
    tt_dns_a_t *a = tt_dns_a_head(rrl);
    while (a != NULL) {
        tt_dns_a_t *next = tt_dns_a_next(a);
        tt_free(a);
        a = next;
    }
    tt_dlist_init(&rrl->rr);
}

tt_result_t __a_copy_list(IN tt_dns_rrlist_t *dst, IN tt_dns_rrlist_t *src)
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

tt_dns_a_t *__a_copy(IN tt_dns_a_t *src)
{
    tt_dns_a_t *dst = tt_malloc(sizeof(tt_dns_a_t));
    if (dst != NULL) {
        tt_dnode_init(&dst->node);
        tt_memcpy(&dst->ip, &src->ip, sizeof(tt_sktaddr_ip_t));
    }
    return dst;
}

void __a_callback(IN void *arg,
                  IN int status,
                  IN int timeouts,
                  IN unsigned char *abuf,
                  IN int alen)
{
    tt_dns_rr_t *drr = (tt_dns_rr_t *)arg;
    tt_bool_t notify;
    tt_s64_t ttl;
    tt_dns_rrlist_t rrl;

    TT_ASSERT(drr->querying_fb != NULL);
    notify = TT_BOOL(drr->querying_fb != tt_current_fiber());
    drr->querying_fb = NULL;

    if (status != ARES_SUCCESS) {
        tt_dns_rr_set(drr, TT_TIME_INFINITE, &__empty_rrlist_a, notify);
        return;
    }

    ttl = TT_TIME_INFINITE;
    tt_dns_rrlist_init(&rrl, TT_DNS_A_IN);
    status = __a_parse(abuf, alen, &ttl, &rrl);
    if (status != ARES_SUCCESS) {
        TT_ERROR("dns a parsing failed: %s", ares_strerror(status));
        tt_dns_rr_set(drr, TT_TIME_INFINITE, &__empty_rrlist_a, notify);
        tt_dns_rrlist_clear(&rrl);
        return;
    }

    tt_dns_rr_set(drr, ttl, &rrl, notify);
    TT_ASSERT_RR(tt_dns_rrlist_empty(&rrl));
}

int __a_parse(IN const unsigned char *abuf,
              IN int alen,
              OUT tt_s64_t *ttl,
              OUT tt_dns_rrlist_t *rrl)
{
    unsigned int qdcount, ancount;
    int status, i, rr_type, rr_class, rr_len, rr_ttl;
    int min_ttl = INT_MAX;
    long len;
    const unsigned char *aptr;
    char *hostname, *rr_name, *rr_data;

    /* Give up if abuf doesn't have room for a header. */
    if (alen < HFIXEDSZ) {
        return ARES_EBADRESP;
    }

    /* Fetch the question and answer count from the header. */
    qdcount = DNS_HEADER_QDCOUNT(abuf);
    ancount = DNS_HEADER_ANCOUNT(abuf);
    if (qdcount != 1) {
        return ARES_EBADRESP;
    }

    /* Expand the name from the question, and skip past the question. */
    aptr = abuf + HFIXEDSZ;
    status = ares__expand_name_for_response(aptr, abuf, alen, &hostname, &len);
    if (status != ARES_SUCCESS) {
        return status;
    }
    if ((aptr + len + QFIXEDSZ) > (abuf + alen)) {
        ares_free(hostname);
        return ARES_EBADRESP;
    }
    aptr += (len + QFIXEDSZ);

    /* Examine each answer resource record (RR) in turn. */
    for (i = 0; i < (int)ancount; i++) {
        /* Decode the RR up to the data field. */
        status =
            ares__expand_name_for_response(aptr, abuf, alen, &rr_name, &len);
        if (status != ARES_SUCCESS) {
            break;
        }
        aptr += len;
        if ((aptr + RRFIXEDSZ) > (abuf + alen)) {
            ares_free(rr_name);
            status = ARES_EBADRESP;
            break;
        }
        rr_type = DNS_RR_TYPE(aptr);
        rr_class = DNS_RR_CLASS(aptr);
        rr_len = DNS_RR_LEN(aptr);
        rr_ttl = DNS_RR_TTL(aptr);
        if (rr_ttl < 0) {
            rr_ttl = 0;
        }
        if (rr_ttl > __MAX_TTL) {
            rr_ttl = __MAX_TTL;
        }
        aptr += RRFIXEDSZ;
        if ((aptr + rr_len) > (abuf + alen)) {
            ares_free(rr_name);
            status = ARES_EBADRESP;
            break;
        }

        if ((rr_class == C_IN) && (rr_type == T_A) &&
            (rr_len == sizeof(tt_sktaddr_ip32_t)) &&
            (strcasecmp(rr_name, hostname) == 0)) {
            if ((aptr + sizeof(tt_sktaddr_ip32_t)) >
                (abuf + alen)) { /* LCOV_EXCL_START: already checked above */
                ares_free(rr_name);
                status = ARES_EBADRESP;
                break;
            } /* LCOV_EXCL_STOP */

            if (!TT_OK(tt_dns_rrlist_add_a(rrl, (tt_sktaddr_ip32_t *)aptr))) {
                ares_free(rr_name);
                status = ARES_ENOMEM;
                break;
            }
            if (rr_ttl < min_ttl) {
                min_ttl = rr_ttl;
            }
            status = ARES_SUCCESS;
        }

        if ((rr_class == C_IN) && (rr_type == T_CNAME)) {
            /* Decode the RR data and replace the hostname with it. */
            status = ares__expand_name_for_response(aptr,
                                                    abuf,
                                                    alen,
                                                    &rr_data,
                                                    &len);
            if (status != ARES_SUCCESS) {
                break;
            }
            ares_free(hostname);
            hostname = rr_data;
            if (rr_ttl < min_ttl) {
                min_ttl = rr_ttl;
            }
        }

        ares_free(rr_name);
        aptr += rr_len;
        if (aptr > (abuf + alen)) { /* LCOV_EXCL_START: already checked above */
            status = ARES_EBADRESP;
            break;
        } /* LCOV_EXCL_STOP */
    }
    ares_free(hostname);

    if (status == ARES_SUCCESS) {
        if (!tt_dns_rrlist_empty(rrl)) {
            *ttl = min_ttl;
            return ARES_SUCCESS;
        } else {
            return ARES_ENODATA;
        }
    } else {
        tt_dns_rrlist_clear(rrl);
        return status;
    }
}

void __aaaa_clear_list(IN tt_dns_rrlist_t *rrl)
{
    tt_dns_aaaa_t *aaaa = tt_dns_aaaa_head(rrl);
    while (aaaa != NULL) {
        tt_dns_aaaa_t *next = tt_dns_aaaa_next(aaaa);
        tt_free(aaaa);
        aaaa = next;
    }
    tt_dlist_init(&rrl->rr);
}

tt_result_t __aaaa_copy_list(IN tt_dns_rrlist_t *dst, IN tt_dns_rrlist_t *src)
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

tt_dns_aaaa_t *__aaaa_copy(IN tt_dns_aaaa_t *src)
{
    tt_dns_aaaa_t *dst = tt_malloc(sizeof(tt_dns_aaaa_t));
    if (dst != NULL) {
        tt_dnode_init(&dst->node);
        tt_memcpy(&dst->ip, &src->ip, sizeof(tt_sktaddr_ip_t));
    }
    return dst;
}

void __aaaa_callback(IN void *arg,
                     IN int status,
                     IN int timeouts,
                     IN unsigned char *abuf,
                     IN int alen)
{
    tt_dns_rr_t *drr = (tt_dns_rr_t *)arg;
    tt_bool_t notify;
    tt_s64_t ttl;
    tt_dns_rrlist_t rrl;

    TT_ASSERT(drr->querying_fb != NULL);
    notify = TT_BOOL(drr->querying_fb != tt_current_fiber());
    drr->querying_fb = NULL;

    if (status != ARES_SUCCESS) {
        tt_dns_rr_set(drr, TT_TIME_INFINITE, &__empty_rrlist_aaaa, notify);
        return;
    }

    ttl = TT_TIME_INFINITE;
    tt_dns_rrlist_init(&rrl, TT_DNS_AAAA_IN);
    status = __aaaa_parse(abuf, alen, &ttl, &rrl);
    if (status != ARES_SUCCESS) {
        TT_ERROR("dns aaaa parsing failed: %s", ares_strerror(status));
        tt_dns_rr_set(drr, TT_TIME_INFINITE, &__empty_rrlist_aaaa, notify);
        tt_dns_rrlist_clear(&rrl);
        return;
    }

    tt_dns_rr_set(drr, ttl, &rrl, notify);
    TT_ASSERT_RR(tt_dns_rrlist_empty(&rrl));
}

int __aaaa_parse(IN const unsigned char *abuf,
                 IN int alen,
                 OUT tt_s64_t *ttl,
                 OUT tt_dns_rrlist_t *rrl)
{
    unsigned int qdcount, ancount;
    int status, i, rr_type, rr_class, rr_len, rr_ttl;
    int min_ttl = INT_MAX;
    long len;
    const unsigned char *aptr;
    char *hostname, *rr_name, *rr_data;

    /* Give up if abuf doesn't have room for a header. */
    if (alen < HFIXEDSZ) {
        return ARES_EBADRESP;
    }

    /* Fetch the question and answer count from the header. */
    qdcount = DNS_HEADER_QDCOUNT(abuf);
    ancount = DNS_HEADER_ANCOUNT(abuf);
    if (qdcount != 1) {
        return ARES_EBADRESP;
    }

    /* Expand the name from the question, and skip past the question. */
    aptr = abuf + HFIXEDSZ;
    status = ares__expand_name_for_response(aptr, abuf, alen, &hostname, &len);
    if (status != ARES_SUCCESS) {
        return status;
    }
    if ((aptr + len + QFIXEDSZ) > (abuf + alen)) {
        ares_free(hostname);
        return ARES_EBADRESP;
    }
    aptr += (len + QFIXEDSZ);

    /* Examine each answer resource record (RR) in turn. */
    for (i = 0; i < (int)ancount; i++) {
        /* Decode the RR up to the data field. */
        status =
            ares__expand_name_for_response(aptr, abuf, alen, &rr_name, &len);
        if (status != ARES_SUCCESS) {
            break;
        }
        aptr += len;
        if ((aptr + RRFIXEDSZ) > (abuf + alen)) {
            ares_free(rr_name);
            status = ARES_EBADRESP;
            break;
        }
        rr_type = DNS_RR_TYPE(aptr);
        rr_class = DNS_RR_CLASS(aptr);
        rr_len = DNS_RR_LEN(aptr);
        rr_ttl = DNS_RR_TTL(aptr);
        if (rr_ttl < 0) {
            rr_ttl = 0;
        }
        if (rr_ttl > __MAX_TTL) {
            rr_ttl = __MAX_TTL;
        }
        aptr += RRFIXEDSZ;
        if ((aptr + rr_len) > (abuf + alen)) {
            ares_free(rr_name);
            status = ARES_EBADRESP;
            break;
        }

        if ((rr_class == C_IN) && (rr_type == T_AAAA) &&
            (rr_len == sizeof(tt_sktaddr_ip128_t)) &&
            (strcasecmp(rr_name, hostname) == 0)) {
            if ((aptr + sizeof(tt_sktaddr_ip128_t)) >
                (abuf + alen)) { /* LCOV_EXCL_START: already checked above */
                ares_free(rr_name);
                status = ARES_EBADRESP;
                break;
            } /* LCOV_EXCL_STOP */

            if (!TT_OK(
                    tt_dns_rrlist_add_aaaa(rrl, (tt_sktaddr_ip128_t *)aptr))) {
                ares_free(rr_name);
                status = ARES_ENOMEM;
                break;
            }
            if (rr_ttl < min_ttl) {
                min_ttl = rr_ttl;
            }
            status = ARES_SUCCESS;
        }

        if ((rr_class == C_IN) && (rr_type == T_CNAME)) {
            /* Decode the RR data and replace the hostname with it. */
            status = ares__expand_name_for_response(aptr,
                                                    abuf,
                                                    alen,
                                                    &rr_data,
                                                    &len);
            if (status != ARES_SUCCESS) {
                break;
            }
            ares_free(hostname);
            hostname = rr_data;
            if (rr_ttl < min_ttl) {
                min_ttl = rr_ttl;
            }
        }

        ares_free(rr_name);
        aptr += rr_len;
        if (aptr > (abuf + alen)) { /* LCOV_EXCL_START: already checked above */
            status = ARES_EBADRESP;
            break;
        } /* LCOV_EXCL_STOP */
    }
    ares_free(hostname);

    if ((status == ARES_SUCCESS) && !tt_dns_rrlist_empty(rrl)) {
        *ttl = min_ttl;
        return ARES_SUCCESS;
    } else {
        tt_dns_rrlist_clear(rrl);
        return status;
    }
}
