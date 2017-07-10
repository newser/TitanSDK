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

#include <network/dns/tt_dns.h>

#include <algorithm/tt_buffer_format.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_socket_addr.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <os/tt_task.h>

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

typedef struct __dns_query4_s
{
    tt_fiber_t *src;
    tt_sktaddr_ip_t *ip;
    tt_result_t result;
} __dns_query_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __dns_component_init(IN tt_component_t *comp,
                                        IN tt_profile_t *profile);

static void __dns_attr2option(IN tt_dns_attr_t *attr,
                              OUT struct ares_options *options,
                              OUT int *optmask);

static tt_result_t __dns_config(IN ares_channel ch, IN tt_dns_attr_t *attr);

static void *__dns_malloc(IN size_t s);

static void __dns_free(IN void *p);

static void *__dns_realloc(IN void *p, IN size_t s);

static void __query4_cb(IN void *arg,
                        IN int status,
                        IN int timeouts,
                        IN struct hostent *hostent);

static void __query6_cb(IN void *arg,
                        IN int status,
                        IN int timeouts,
                        IN struct hostent *hostent);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_dns_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __dns_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_DNS, "DNS", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

tt_dns_t tt_dns_create(IN OPT tt_dns_attr_t *attr)
{
    tt_dns_attr_t __attr;
    ares_channel ch;
    struct ares_options options;
    int optmask;
    int e;

    if (attr == NULL) {
        tt_dns_attr_default(&__attr);
        attr = &__attr;
    }

    __dns_attr2option(attr, &options, &optmask);
    e = ares_init_options(&ch, &options, optmask);
    if (e != ARES_SUCCESS) {
        TT_ERROR("fail to create dns: %s", ares_strerror(e));
        return NULL;
    }

    if (!TT_OK(__dns_config(ch, attr))) {
        ares_destroy(ch);
        return NULL;
    }

    if (!TT_OK(tt_dns_create_ntv(ch))) {
        ares_destroy(ch);
        return NULL;
    }

    return ch;
}

void tt_dns_destroy(IN tt_dns_t d)
{
    TT_ASSERT(d != NULL);

    // the ares channel is destroyed in tt_dns_destroy_ntv()
    tt_dns_destroy_ntv(d);
}

void tt_dns_attr_default(IN tt_dns_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->enable_edns = TT_FALSE;
    attr->prefer_tcp = TT_FALSE;
    attr->timeout_ms = 0;
    attr->try_num = 0;
    attr->send_buf_size = 0;
    attr->recv_buf_size = 0;
    attr->server = NULL;
    attr->server_num = 0;
    attr->local_ip4 = NULL;
    attr->local_ip6 = NULL;
    attr->local_device = NULL;
}

tt_dns_t __ut_current_dns_d()
{
    return tt_current_task()->dns_cache->d;
}

tt_result_t __ut_dns_query4(IN tt_dns_t d,
                            IN const tt_char_t *name,
                            OUT tt_sktaddr_ip_t *ip)
{
    __dns_query_t dq;

    dq.src = tt_current_fiber();
    dq.ip = ip;
    dq.result = TT_PROCEEDING;

    ares_gethostbyname(d, name, AF_INET, __query4_cb, &dq);
    if (dq.result == TT_PROCEEDING) {
        tt_fiber_suspend();
    }
    TT_ASSERT(dq.result != TT_PROCEEDING);
    return dq.result;
}

tt_result_t __ut_dns_query6(IN tt_dns_t d,
                            IN const tt_char_t *name,
                            OUT tt_sktaddr_ip_t *ip)
{
    __dns_query_t dq;

    dq.src = tt_current_fiber();
    dq.ip = ip;
    dq.result = TT_PROCEEDING;

    ares_gethostbyname(d, name, AF_INET6, __query6_cb, &dq);
    if (dq.result == TT_PROCEEDING) {
        tt_fiber_suspend();
    }
    TT_ASSERT(dq.result != TT_PROCEEDING);
    return dq.result;
}

tt_result_t __dns_component_init(IN tt_component_t *comp,
                                 IN tt_profile_t *profile)
{
    int flags = 0;
    int e;

#if TT_ENV_OS_IS_WINDOWS
    flags |= ARES_LIB_INIT_WIN32;
#endif
    e = ares_library_init_mem(flags, __dns_malloc, __dns_free, __dns_realloc);
    if (e != ARES_SUCCESS) {
        TT_ERROR("fail to ini ares: %s", ares_strerror(e));
        return TT_FAIL;
    }

    if (!TT_OK(tt_dns_component_init_ntv(profile))) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void __dns_attr2option(IN tt_dns_attr_t *attr,
                       OUT struct ares_options *options,
                       OUT int *optmask)
{
    tt_memset(options, 0, sizeof(struct ares_options));
    *optmask = 0;

    options->flags |= ARES_FLAG_STAYOPEN;
    if (attr->enable_edns) {
        options->flags |= ARES_FLAG_EDNS;
    }
    if (attr->prefer_tcp) {
        options->flags |= ARES_FLAG_USEVC;
    }
    *optmask |= ARES_OPT_FLAGS;

    if (attr->timeout_ms != 0) {
        options->timeout = attr->timeout_ms;
        *optmask |= ARES_OPT_TIMEOUTMS;
    }

    if (attr->try_num != 0) {
        options->tries = attr->try_num;
        *optmask |= ARES_OPT_TRIES;
    }

    if (attr->send_buf_size != 0) {
        options->socket_send_buffer_size = attr->send_buf_size;
        *optmask |= ARES_OPT_SOCK_SNDBUF;
    }

    if (attr->recv_buf_size != 0) {
        options->socket_receive_buffer_size = attr->recv_buf_size;
        *optmask |= ARES_OPT_SOCK_RCVBUF;
    }

    if (attr->server != NULL) {
        options->servers = NULL;
        options->nservers = 0;
        *optmask |= ARES_OPT_SERVERS;
        // set nservers to 0 and configure servers in __dns_config()
    }
}

tt_result_t __dns_config(IN ares_channel ch, IN tt_dns_attr_t *attr)
{
    if (attr->server != NULL) {
        tt_buf_t buf;
        tt_u32_t i;
        int e;

        TT_ASSERT(attr->server_num != 0);

        tt_buf_init(&buf, NULL);
        for (i = 0; i < attr->server_num; ++i) {
            TT_DO(tt_buf_put_cstr(&buf, attr->server[i]));
            TT_DO(tt_buf_put_u8(&buf, (tt_u8_t)','));
        }
        TT_DO(tt_buf_put_u8(&buf, 0));

        e = ares_set_servers_ports_csv(ch, (const char *)TT_BUF_RPOS(&buf));
        tt_buf_destroy(&buf);
        if (e != ARES_SUCCESS) {
            TT_ERROR("fail to set dns servers: %s", ares_strerror(e));
            return TT_FAIL;
        }
    }

    if (attr->local_ip4 != NULL) {
        tt_sktaddr_t addr;
        tt_sktaddr_ip_t ip;
        tt_sktaddr_init(&addr, TT_NET_AF_INET);
        if (!TT_OK(tt_sktaddr_set_ip_p(&addr, attr->local_ip4))) {
            TT_ERROR("invalid ipv4 addr: %s", attr->local_ip4);
            return TT_FAIL;
        }
        tt_sktaddr_get_ip_n(&addr, &ip);
        ares_set_local_ip4(ch, ip.a32.__u32);
    }

    if (attr->local_ip6 != NULL) {
        tt_sktaddr_t addr;
        tt_sktaddr_ip_t ip;
        tt_sktaddr_init(&addr, TT_NET_AF_INET6);
        if (!TT_OK(tt_sktaddr_set_ip_p(&addr, attr->local_ip6))) {
            TT_ERROR("invalid ipv6 addr: %s", attr->local_ip6);
            return TT_FAIL;
        }
        tt_sktaddr_get_ip_n(&addr, &ip);
        ares_set_local_ip6(ch, ip.a128.__u8);
    }

    if (attr->local_device != NULL) {
        ares_set_local_dev(ch, attr->local_device);
    }

    return TT_SUCCESS;
}

void *__dns_malloc(IN size_t s)
{
    return tt_malloc(s);
}

void __dns_free(IN void *p)
{
    tt_free(p);
}

void *__dns_realloc(IN void *p, IN size_t s)
{
    return tt_realloc(p, s);
}

void __query4_cb(IN void *arg,
                 IN int status,
                 IN int timeouts,
                 IN struct hostent *hostent)
{
    __dns_query_t *dq = (__dns_query_t *)arg;

    TT_ASSERT(dq->result == TT_PROCEEDING);

    if ((status == ARES_SUCCESS) && (hostent != NULL) &&
        (hostent->h_addrtype == AF_INET) && (hostent->h_addr_list != NULL) &&
        (hostent->h_addr_list[0] != NULL)) {
        struct in_addr *ip = (struct in_addr *)hostent->h_addr_list[0];
        dq->ip->a32.__u32 = ip->s_addr;
        dq->result = TT_SUCCESS;
    } else if (status == ARES_ETIMEOUT) {
        dq->result = TT_TIME_OUT;
    } else {
        dq->result = TT_FAIL;
    }

    if (tt_current_fiber() != dq->src) {
        tt_fiber_resume(dq->src, TT_FALSE);
    }
}

void __query6_cb(IN void *arg,
                 IN int status,
                 IN int timeouts,
                 IN struct hostent *hostent)
{
    __dns_query_t *dq = (__dns_query_t *)arg;

    TT_ASSERT(dq->result == TT_PROCEEDING);

    if ((status == ARES_SUCCESS) && (hostent != NULL) &&
        (hostent->h_addrtype == AF_INET6) && (hostent->h_addr_list != NULL) &&
        (hostent->h_addr_list[0] != NULL)) {
        struct in6_addr *ip = (struct in6_addr *)hostent->h_addr_list[0];
        tt_memcpy(dq->ip->a128.__u8, ip->__u6_addr.__u6_addr8, 16);
        dq->result = TT_SUCCESS;
    } else if (status == ARES_ETIMEOUT) {
        dq->result = TT_TIME_OUT;
    } else {
        dq->result = TT_FAIL;
    }

    if (tt_current_fiber() != dq->src) {
        tt_fiber_resume(dq->src, TT_FALSE);
    }
}
