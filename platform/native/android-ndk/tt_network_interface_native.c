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

#include <tt_network_interface_native.h>

#include <algorithm/tt_buffer.h>
#include <network/tt_network_interface.h>
#include <os/tt_atomic.h>
#include <os/tt_thread.h>

#include <tt_util_native.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __NLINK_FRAME_SIZE (1 << 12)

#define __NLINK_MAX_SIZE (1 << 20)

#define __cmp_addr(na, sa)                                                     \
    tt_memcmp(&((struct sockaddr_in *)&(na)->addr)->sin_addr,                  \
              &((struct sockaddr_in *)(sa))->sin_addr,                         \
              sizeof(struct in_addr))
#define __copy_addr(na, sa)                                                    \
    tt_memcpy(&(na)->addr, (sa), sizeof(struct sockaddr_in))

#define __cmp_addr6(na, sa)                                                    \
    tt_memcmp(&((struct sockaddr_in6 *)&(na)->addr)->sin6_addr,                \
              &((struct sockaddr_in6 *)(sa))->sin6_addr,                       \
              sizeof(struct in6_addr))
#define __copy_addr6(na, sa)                                                   \
    tt_memcpy(&(na)->addr, (sa), sizeof(struct sockaddr_in6))

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct __netlink_s
{
    tt_buf_t link_buf;
    tt_buf_t addr_buf;
    int s;
    __u32 pid;
    __u32 seq;
} __netlink_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_atomic_s32_t __s_netlink_pid;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __netlink_t *__netlink_get();

static tt_result_t __netlink_req(IN __netlink_t *nlink,
                                 IN __u16 type,
                                 IN tt_buf_t *buf,
                                 OUT struct nlmsghdr **hdr,
                                 OUT tt_u32_t *len);

static tt_result_t __netlink_update_link(IN tt_netif_group_t *group,
                                         IN struct ifinfomsg *ifi,
                                         IN tt_u32_t len);

static tt_result_t __netlink_update_addr(IN tt_netif_group_t *group,
                                         IN struct ifaddrmsg *ifa,
                                         IN tt_u32_t len);

static tt_result_t __netlink_send(IN __netlink_t *nlink, IN __u16 type);

static tt_u32_t __netlink_recv(IN __netlink_t *nlink, IN tt_buf_t *buf);
#define __NLRECV_OK 0
#define __NLRECV_ERROR 1
#define __NLRECV_AGAIN 2
#define __NLRECV_CONTINUE 3

static tt_result_t __netif_update_link(IN tt_netif_t *netif,
                                       IN struct ifinfomsg *ifi,
                                       IN tt_u32_t len);

static tt_result_t __netif_make_addr4(IN tt_netif_t *netif,
                                      IN struct ifaddrmsg *ifa,
                                      IN tt_u32_t len,
                                      OUT struct sockaddr_in *addr);

static tt_result_t __netif_make_addr6(IN tt_netif_t *netif,
                                      IN struct ifaddrmsg *ifa,
                                      IN tt_u32_t len,
                                      OUT struct sockaddr_in6 *addr);

static tt_netif_addr_t *tt_netif_group_find_addr(IN tt_netif_t *netif,
                                                 IN struct sockaddr *addr);

static void __netif_addr_update(IN tt_netif_addr_t *netif_addr,
                                IN struct sockaddr *addr);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_network_interface_init_ntv()
{
    tt_atomic_s32_set(&__s_netlink_pid, 0);

    return TT_SUCCESS;
}

tt_result_t tt_netif_group_refresh_ntv(IN tt_netif_group_t *group,
                                       IN tt_u32_t flag)
{
    __netlink_t *nlink;
    struct nlmsghdr *link_hdr, *addr_hdr, *h;
    tt_u32_t link_len, addr_len;

    // a good reference: https://github.com/morristech/android-ifaddrs

    nlink = __netlink_get();
    if (nlink == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(__netlink_req(nlink,
                             RTM_GETLINK,
                             &nlink->link_buf,
                             &link_hdr,
                             &link_len))) {
        return TT_FAIL;
    }
    if (!TT_OK(__netlink_req(nlink,
                             RTM_GETADDR,
                             &nlink->addr_buf,
                             &addr_hdr,
                             &addr_len))) {
        return TT_FAIL;
    }

    for (h = link_hdr; NLMSG_OK(h, link_len); h = NLMSG_NEXT(h, link_len)) {
        if (h->nlmsg_type != RTM_NEWLINK) {
            continue;
        }

        if (!TT_OK(
                __netlink_update_link(group,
                                      (struct ifinfomsg *)NLMSG_DATA(h),
                                      NLMSG_PAYLOAD(h,
                                                    sizeof(
                                                        struct ifinfomsg))))) {
            return TT_FAIL;
        }
    }

    for (h = addr_hdr; NLMSG_OK(h, addr_len); h = NLMSG_NEXT(h, addr_len)) {
        if (h->nlmsg_type != RTM_NEWADDR) {
            continue;
        }

        if (!TT_OK(
                __netlink_update_addr(group,
                                      (struct ifaddrmsg *)NLMSG_DATA(h),
                                      NLMSG_PAYLOAD(h,
                                                    sizeof(
                                                        struct ifaddrmsg))))) {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_netif_create_ntv(IN tt_netif_ntv_t *sys_netif)
{
    sys_netif->skt = -1;

    return TT_SUCCESS;
}

void tt_netif_destroy_ntv(IN tt_netif_ntv_t *sys_netif)
{
    if (sys_netif->skt != -1) {
        __RETRY_IF_EINTR(close(sys_netif->skt));
    }
}

__netlink_t *__netlink_get()
{
    tt_thread_t *t = tt_current_thread();

    if (t->sys_thread.nlink == NULL) {
        __netlink_t *nlink;
        tt_buf_attr_t buf_attr;
        struct sockaddr_nl addr;

        nlink = tt_malloc(sizeof(__netlink_t));
        if (nlink == NULL) {
            TT_ERROR("no mem for new netlink");
            return NULL;
        }

        tt_buf_attr_default(&buf_attr);
        buf_attr.max_limit = __NLINK_MAX_SIZE;
        if (!TT_OK(tt_buf_create(&nlink->link_buf,
                                 __NLINK_FRAME_SIZE,
                                 &buf_attr))) {
            tt_free(nlink);
            return NULL;
        }
        if (!TT_OK(tt_buf_create(&nlink->addr_buf,
                                 __NLINK_FRAME_SIZE,
                                 &buf_attr))) {
            tt_buf_destroy(&nlink->link_buf);
            tt_free(nlink);
            return NULL;
        }

        nlink->s = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
        if (nlink->s < 0) {
            TT_ERROR_NTV("fail to create socket");
            tt_buf_destroy(&nlink->addr_buf);
            tt_buf_destroy(&nlink->link_buf);
            tt_free(nlink);
            return NULL;
        }

        nlink->pid = tt_atomic_s32_inc(&__s_netlink_pid);
        nlink->seq = 0;

        memset(&addr, 0, sizeof(addr));
        addr.nl_family = AF_NETLINK;
        addr.nl_pid = nlink->pid;
        if (bind(nlink->s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            TT_ERROR_NTV("binding fail");
            __RETRY_IF_EINTR(close(nlink->s));
            tt_buf_destroy(&nlink->addr_buf);
            tt_buf_destroy(&nlink->link_buf);
            tt_free(nlink);
            return NULL;
        }

        t->sys_thread.nlink = nlink;
    }

    return t->sys_thread.nlink;
}

void __netlink_destroy(IN __netlink_t *nlink)
{
    tt_buf_destroy(&nlink->link_buf);
    tt_buf_destroy(&nlink->addr_buf);
    __RETRY_IF_EINTR(close(nlink->s));
    tt_free(nlink);
}

tt_result_t __netlink_req(IN __netlink_t *nlink,
                          IN __u16 type,
                          IN tt_buf_t *buf,
                          OUT struct nlmsghdr **hdr,
                          OUT tt_u32_t *len)
{
    tt_u32_t result;

send:
    if (!TT_OK(__netlink_send(nlink, type))) {
        return TT_FAIL;
    }

    tt_buf_clear(buf);
recv:
    result = __netlink_recv(nlink, buf);
    if (result == __NLRECV_ERROR) {
        return TT_FAIL;
    } else if (result == __NLRECV_AGAIN) {
        // failed due to truncated data, but buf has been enlarged
        goto send;
    } else if (result == __NLRECV_CONTINUE) {
        // no error, but should recv more
        goto recv;
    }
    TT_ASSERT(result == __NLRECV_OK);

    *hdr = (struct nlmsghdr *)TT_BUF_RPOS(buf);
    *len = TT_BUF_RLEN(buf);
    return TT_SUCCESS;
}

tt_result_t __netlink_update_link(IN tt_netif_group_t *group,
                                  IN struct ifinfomsg *ifi,
                                  IN tt_u32_t len)
{
    struct rtattr *a;
    tt_netif_t *netif = NULL;

    for (a = TT_PTR_INC(struct rtattr,
                        ifi,
                        NLMSG_ALIGN(sizeof(struct ifinfomsg)));
         RTA_OK(a, len);
         a = RTA_NEXT(a, len)) {
        if ((a->rta_type == IFLA_IFNAME) &&
            (RTA_PAYLOAD(a) <= TT_NETIF_MAX_NAME_LEN)) {
            netif = tt_netif_group_find(group, (tt_char_t *)RTA_DATA(a));
            if (netif != NULL) {
                return __netif_update_link(netif, ifi, len);
            }
        }
    }
    return TT_SUCCESS;
}

tt_result_t __netlink_update_addr(IN tt_netif_group_t *group,
                                  IN struct ifaddrmsg *ifa,
                                  IN tt_u32_t len)
{
    struct rtattr *a;
    tt_netif_t *netif;
    struct sockaddr_storage addr;
    tt_result_t result;
    tt_netif_addr_t *netif_addr;

    if ((ifa->ifa_family != AF_INET) && (ifa->ifa_family != AF_INET6)) {
        return TT_SUCCESS;
    }

    netif = NULL;
    for (a = TT_PTR_INC(struct rtattr,
                        ifa,
                        NLMSG_ALIGN(sizeof(struct ifaddrmsg)));
         RTA_OK(a, len);
         a = RTA_NEXT(a, len)) {
        if ((a->rta_type == IFA_LABEL) &&
            (RTA_PAYLOAD(a) <= TT_NETIF_MAX_NAME_LEN)) {
            netif = tt_netif_group_find(group, (tt_char_t *)RTA_DATA(a));
            break;
        }
    }
    if (netif == NULL) {
        return TT_SUCCESS;
    }

    if (ifa->ifa_family == AF_INET) {
        result =
            __netif_make_addr4(netif, ifa, len, (struct sockaddr_in *)&addr);
    } else {
        result =
            __netif_make_addr6(netif, ifa, len, (struct sockaddr_in6 *)&addr);
    }
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    netif_addr = tt_netif_group_find_addr(netif, (struct sockaddr *)&addr);
    if (netif_addr != NULL) {
        __netif_addr_update(netif_addr, (struct sockaddr *)&addr);
    } else {
        netif_addr = tt_netif_addr_create(TT_COND(ifa->ifa_family == AF_INET,
                                                  TT_NET_AF_INET,
                                                  TT_NET_AF_INET6));
        if (netif_addr == NULL) {
            return TT_FAIL;
        }

        __netif_addr_update(netif_addr, (struct sockaddr *)&addr);
        tt_list_push_tail(&netif->addr_list, &netif_addr->node);
    }
    return TT_SUCCESS;
}

tt_result_t __netlink_send(IN __netlink_t *nlink, IN __u16 type)
{
    struct sockaddr_nl addr;
    tt_u8_t buf[NLMSG_ALIGN(sizeof(struct nlmsghdr)) +
                NLMSG_ALIGN(sizeof(struct rtgenmsg))];
    struct nlmsghdr *hdr = (struct nlmsghdr *)buf;
    struct rtgenmsg *msg = (struct rtgenmsg *)NLMSG_DATA(hdr);

    memset(&addr, 0, sizeof(struct sockaddr_nl));
    addr.nl_family = AF_NETLINK;

    memset(buf, 0, sizeof(buf));
    hdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    hdr->nlmsg_type = type;
    hdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT | NLM_F_MATCH;
    hdr->nlmsg_seq = nlink->seq++;
    hdr->nlmsg_pid = nlink->pid;
    msg->rtgen_family = AF_UNSPEC;

again:
    if (sendto(nlink->s,
               hdr,
               hdr->nlmsg_len,
               0,
               (struct sockaddr *)&addr,
               sizeof(struct sockaddr_nl)) > 0) {
        return TT_SUCCESS;
    } else if (errno == EINTR) {
        goto again;
    } else {
        TT_ERROR_NTV("netlink send failed");
        return TT_FAIL;
    }
}

tt_u32_t __netlink_recv(IN __netlink_t *nlink, IN tt_buf_t *buf)
{
    struct iovec iov;
    struct sockaddr_nl addr;
#if __ANDROID_API__ >= 21
    struct mmsghdr msgvec;
    struct msghdr *msg = &msgvec.msg_hdr;
#else
    struct msghdr m;
    struct msghdr *msg = &m;
#endif
    struct timespec timeout;
    ssize_t n;
    struct nlmsghdr *hdr;

    iov.iov_base = TT_BUF_WPOS(buf);
    iov.iov_len = TT_BUF_WLEN(buf);

    msg->msg_name = (void *)&addr;
    msg->msg_namelen = sizeof(struct sockaddr_nl);
    msg->msg_iov = &iov;
    msg->msg_iovlen = 1;
    msg->msg_control = NULL;
    msg->msg_controllen = 0;
    msg->msg_flags = 0;

again:
#if __ANDROID_API__ >= 21
    msgvec.msg_len = 0;
    timeout.tv_sec = 3; // congfiguable?
    timeout.tv_nsec = 0;
    n = recvmmsg(nlink->s, &msgvec, 1, 0, &timeout);
    if (n > 0) {
        n = msgvec.msg_len;
    }
#else
    (void)timeout;
    n = recvmsg(nlink->s, msg, 0);
#endif
    if (n <= 0) {
        if (errno == EINTR) {
            goto again;
        } else {
            TT_ERROR_NTV("netlink rev failed");
            return __NLRECV_ERROR;
        }
    } else if (msg->msg_flags & MSG_TRUNC) {
        tt_buf_clear(buf);
        n = TT_BUF_WLEN(buf) << 1;
        if (TT_OK(tt_buf_reserve(buf, n))) {
            return __NLRECV_AGAIN;
        } else {
            return __NLRECV_ERROR;
        }
    }
    tt_buf_inc_wp(buf, (tt_u32_t)n);

    for (hdr = (struct nlmsghdr *)iov.iov_base; NLMSG_OK(hdr, n);
         hdr = NLMSG_NEXT(hdr, n)) {
        if ((hdr->nlmsg_pid != nlink->pid) ||
            (hdr->nlmsg_seq != (nlink->seq - 1))) {
            continue;
        }

        if (hdr->nlmsg_type == NLMSG_DONE) {
            return __NLRECV_OK;
        }

        if (hdr->nlmsg_type == NLMSG_ERROR) {
            TT_ERROR_NTV("netlink return NLMSG_ERROR");
            return __NLRECV_ERROR;
        }
    }
    return __NLRECV_CONTINUE;
}

tt_result_t __netif_update_link(IN tt_netif_t *netif,
                                IN struct ifinfomsg *ifi,
                                IN tt_u32_t len)
{
    if (ifi->ifi_flags & IFF_UP) {
        netif->internal_flag |= __NETIF_INTERNAL_UP;
    } else {
        netif->internal_flag &= ~__NETIF_INTERNAL_UP;
    }

    return TT_SUCCESS;
}

tt_result_t __netif_make_addr4(IN tt_netif_t *netif,
                               IN struct ifaddrmsg *ifa,
                               IN tt_u32_t len,
                               OUT struct sockaddr_in *addr)
{
    struct rtattr *a;

    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;

    for (a = TT_PTR_INC(struct rtattr,
                        ifa,
                        NLMSG_ALIGN(sizeof(struct ifaddrmsg)));
         RTA_OK(a, len);
         a = RTA_NEXT(a, len)) {
        if (a->rta_type == IFA_LOCAL) {
            memcpy(&addr->sin_addr, RTA_DATA(a), sizeof(struct in_addr));
        }
    }
    // may need check if all necessary fields are set

    return TT_SUCCESS;
}

tt_result_t __netif_make_addr6(IN tt_netif_t *netif,
                               IN struct ifaddrmsg *ifa,
                               IN tt_u32_t len,
                               OUT struct sockaddr_in6 *addr)
{
    struct rtattr *a;

    memset(&addr, 0, sizeof(struct sockaddr_in6));
    addr->sin6_family = AF_INET6;

    for (a = TT_PTR_INC(struct rtattr,
                        ifa,
                        NLMSG_ALIGN(sizeof(struct ifaddrmsg)));
         RTA_OK(a, len);
         a = RTA_NEXT(a, len)) {
        if (a->rta_type == IFA_LOCAL) {
            memcpy(&addr->sin6_addr, RTA_DATA(a), sizeof(struct in6_addr));
        }
        // scope id?
    }
    // may need check if all necessary fields are set

    return TT_SUCCESS;
}

tt_netif_addr_t *tt_netif_group_find_addr(IN tt_netif_t *netif,
                                          IN struct sockaddr *addr)
{
    tt_lnode_t *node = tt_list_head(&netif->addr_list);
    while (node != NULL) {
        tt_netif_addr_t *cur_addr = TT_CONTAINER(node, tt_netif_addr_t, node);

        node = node->next;

        if (addr->sa_family == AF_INET) {
            // ipv4, compare:
            //  - ip
            if (__cmp_addr(cur_addr, addr) == 0) {
                return cur_addr;
            }
        } else {
            TT_ASSERT(addr->sa_family == AF_INET6);

            // ipv6, compare:
            //  - ip
            //  - scope id??
            if (__cmp_addr6(cur_addr, addr) == 0) {
                return cur_addr;
            }
        }
    }

    return NULL;
}

void __netif_addr_update(IN tt_netif_addr_t *netif_addr,
                         IN struct sockaddr *addr)
{
    sa_family_t family = addr->sa_family;

    netif_addr->internal_flag |= __NETIF_INTERNAL_TOUCHED;

    if (family == AF_INET) {
        if (__cmp_addr(netif_addr, addr) != 0) {
            __copy_addr(netif_addr, addr);
            netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
        }
    } else {
        TT_ASSERT(family == AF_INET6);

        if (__cmp_addr6(netif_addr, addr) != 0) {
            __copy_addr6(netif_addr, addr);
            netif_addr->internal_flag |= TT_NETIF_DIFF_ADDR;
        }

        // any other fields
    }
}
