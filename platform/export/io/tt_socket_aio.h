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

/**
@file tt_socket_aio.h
@brief async socket io

this file defines async socket io APIs
*/

#ifndef __TT_SKT_AIO__
#define __TT_SKT_AIO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_socket_aio_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_profile_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_skt_aio_component_init(IN struct tt_profile_s *profile)
{
    return tt_skt_aio_component_init_ntv(profile);
}

tt_inline tt_result_t tt_async_skt_create(OUT tt_skt_t *skt,
                                          IN tt_net_family_t family,
                                          IN tt_net_protocol_t protocol,
                                          IN tt_skt_role_t role,
                                          IN OPT tt_skt_attr_t *attr,
                                          IN tt_skt_exit_t *exit)
{
    return tt_async_skt_create_ntv(skt, family, protocol, role, attr, exit);
}

/**
 @note
 - all callbacks would be called either set @ref immediate to TT_TRUE or
 TT_FALSE. setting @ref immediate to TT_TRUE will calls all callback
 before this function return. setting to TT_FALSE will execute the
 callbacks asynchronously
 */
tt_inline void tt_async_skt_destroy(IN tt_skt_t *skt, IN tt_bool_t immediate)
{
    tt_async_skt_destroy_ntv(skt, immediate);
}

tt_inline tt_result_t tt_async_skt_shutdown(IN tt_skt_t *skt, IN tt_u32_t mode)
{
    return tt_async_skt_shutdown_ntv(skt, mode);
}

/**
@fn tt_result_t tt_skt_accept_async(IN tt_skt_t *listening_skt,
                                       IN tt_skt_t *new_skt,
                                       IN tt_u32_t new_skt_role,
                                       IN OPT tt_skt_attr_t *new_skt_attr,
                                       IN tt_skt_exit_t
*new_skt_exit,
                                       IN tt_skt_on_accept_t on_accept,
                                       IN OPT void *cb_param)
accept on a socket asynchronous

@param [in] listening_skt server socket
@param [in] new_skt new created socket
@param [in] new_skt_role role of accepted socket
@param [in] new_skt_attr attribute to create new socket
@param [in] on_accept callback when a new connection is accepted
@param [in] cb_param callback param that would be passed to on_accept
@param [in] new_skt_exit callback when accepted socket is destroyed

@return
- TT_PROCEEDING async request is being handled
- TT_FAIL, otherwise

@note
- this function does not guarantee that the new socket would inherite
  all attributes of @ref listening_skt, although os may do something for it.
  caller should explicitly set options by apis defined in tt_socket_option.h
- param @ref new_skt is merely a structure to store accepted socket, MUST
  NOT be a socket that is already created by @ref tt_async_skt_create or
  it causes resouce leak
- DO NOT assume @ref on_accept are called with same order as they are
  submitted, this depends on platform
- new_skt_exit->on_destroy would be called to notify that the new
  accepted socket is destroyed only when on_accept is ever called returning
  TT_SUCCESS
*/
tt_inline tt_result_t tt_skt_accept_async(IN tt_skt_t *listening_skt,
                                          IN tt_skt_t *new_skt,
                                          IN OPT tt_skt_attr_t *new_skt_attr,
                                          IN tt_skt_exit_t *new_skt_exit,
                                          IN tt_skt_on_accept_t on_accept,
                                          IN OPT void *cb_param)
{
    return tt_skt_accept_async_ntv(listening_skt,
                                   new_skt,
                                   new_skt_attr,
                                   new_skt_exit,
                                   on_accept,
                                   cb_param);
}

/**
@fn tt_result_t tt_skt_connect_async(IN tt_skt_t *skt,
                                        IN tt_sktaddr_t *remote_addr,
                                        IN tt_skt_on_connect_t *on_connect,
                                        IN void *cb_param)
connect a socket asynchronous

@param [in] skt client socket
@param [in] remote_addr remote address
@param [in] on_connect callback when it's connected
@param [in] cb_param callback param that would be passed to on_connect

@return
- TT_PROCEEDING async request is being handled
- TT_FAIL, otherwise

@note
- @ref current MUST be current thread, or it would lead to crash
- submit one connect aio at one time, do nothing before the callback
*/
tt_inline tt_result_t tt_skt_connect_async(IN tt_skt_t *skt,
                                           IN tt_sktaddr_t *remote_addr,
                                           IN tt_skt_on_connect_t on_connect,
                                           IN OPT void *cb_param)
{
    return tt_skt_connect_async_ntv(skt, remote_addr, on_connect, cb_param);
}

tt_inline tt_result_t tt_skt_connect_n_async(IN tt_skt_t *skt,
                                             IN tt_net_family_t family,
                                             IN tt_sktaddr_addr_t *addr,
                                             IN tt_u16_t port,
                                             IN tt_skt_on_connect_t on_connect,
                                             IN OPT void *cb_param)
{
    tt_sktaddr_t remote_addr;

    tt_sktaddr_init(&remote_addr, family);
    tt_sktaddr_set_addr_n(&remote_addr, addr);
    tt_sktaddr_set_port(&remote_addr, port);

    return tt_skt_connect_async_ntv(skt, &remote_addr, on_connect, cb_param);
}

tt_inline tt_result_t tt_skt_connect_p_async(IN tt_skt_t *skt,
                                             IN tt_net_family_t family,
                                             IN tt_char_t *addr,
                                             IN tt_u16_t port,
                                             IN tt_skt_on_connect_t on_connect,
                                             IN OPT void *cb_param)
{
    tt_sktaddr_t remote_addr;

    tt_sktaddr_init(&remote_addr, family);
    tt_sktaddr_set_addr_p(&remote_addr, addr);
    tt_sktaddr_set_port(&remote_addr, port);

    return tt_skt_connect_async_ntv(skt, &remote_addr, on_connect, cb_param);
}

/**
@fn tt_result_t tt_skt_send_async(IN tt_skt_t *skt,
                                     IN tt_blob_t *blob,
                                     IN tt_u32_t blob_num,
                                     IN tt_skt_on_send_t *on_send,
                                     IN void *cb_param)
send through a socket asynchronous

@param [in] skt client socket
@param [in] blob buffers storing received data
@param [in] blob_num number of buffers
@param [in] on_send callback when sending completed
@param [in] cb_param callback param that would be passed to on_send

@return
- TT_PROCEEDING async request is being handled
- TT_FAIL, otherwise

@note
- this function guarantee that @ref on_send is called only when all data
  in buf has been sent or when some error occurs, note it may happen that
  partial data was sent when error occurs
- order of calling this function can be kept, that means order of data sent
  would be same as calling sequence. if it's called by multi threads,
  sequence of sent data is same as that of calling sequence by multiple
  threads
*/
tt_inline tt_result_t tt_skt_send_async(IN tt_skt_t *skt,
                                        IN tt_blob_t *blob,
                                        IN tt_u32_t blob_num,
                                        IN tt_skt_on_send_t on_send,
                                        IN OPT void *cb_param)
{
    return tt_skt_send_async_ntv(skt, blob, blob_num, on_send, cb_param);
}

/**
@fn tt_result_t tt_skt_recv_async(IN tt_skt_t *skt,
                                     IN tt_blob_t *blob,
                                     IN tt_u32_t blob_num,
                                     IN tt_skt_on_recv_t on_recv,
                                     IN void *cb_param)
receive from a socket asynchronous

@param [in] skt client socket
@param [in] blob buffers storing received data
@param [in] blob_num number of buffers
@param [in] on_recv callback when some data has been received
@param [in] cb_param callback param that would be passed to on_recv

@return
- TT_PROCEEDING async request is being handled
- TT_FAIL, otherwise

@note
- @ref on_recv may be called when buffer is not full, which means
  "some data" is coming not "buffers are fulfilled"
- order of calling this function can be kept, for example, 3 threads called
  this function and the sequence of passed in buffer is buf1, buf2, buf3,
  it can guarantee buf1 would be first filled(but maybe partial buffer is
  filled), and then it fill buf2 with some bytes and finally to buf3
*/
tt_inline tt_result_t tt_skt_recv_async(IN tt_skt_t *skt,
                                        IN tt_blob_t *blob,
                                        IN tt_u32_t blob_num,
                                        IN tt_skt_on_recv_t on_recv,
                                        IN OPT void *cb_param)
{
    return tt_skt_recv_async_ntv(skt, blob, blob_num, on_recv, cb_param);
}

tt_inline tt_result_t tt_skt_sendto_async(IN tt_skt_t *skt,
                                          IN tt_blob_t *blob,
                                          IN tt_u32_t blob_num,
                                          IN tt_sktaddr_t *remote_addr,
                                          IN tt_skt_on_sendto_t on_sendto,
                                          IN OPT void *cb_param)
{
    return tt_skt_sendto_async_ntv(skt,
                                   blob,
                                   blob_num,
                                   remote_addr,
                                   on_sendto,
                                   cb_param);
}

// - it's IMPORTANT to make sure buffers have enough size to store incoming
//   packets, otherwise skt may be unable to be destroyed(no callback after
//   called tt_async_skt_destroy())!
tt_inline tt_result_t tt_skt_recvfrom_async(IN tt_skt_t *skt,
                                            IN tt_blob_t *blob,
                                            IN tt_u32_t blob_num,
                                            IN tt_skt_on_recvfrom_t on_recvfrom,
                                            IN OPT void *cb_param)
{
    return tt_skt_recvfrom_async_ntv(skt,
                                     blob,
                                     blob_num,
                                     on_recvfrom,
                                     cb_param);
}

tt_inline tt_result_t tt_tcp_server_async(OUT tt_skt_t *skt,
                                          IN tt_net_family_t family,
                                          IN tt_skt_attr_t *attr,
                                          IN tt_sktaddr_t *local_addr,
                                          IN tt_u32_t backlog,
                                          IN tt_skt_exit_t *exit)
{
    return tt_tcp_server_async_ntv(skt,
                                   family,
                                   attr,
                                   local_addr,
                                   backlog,
                                   exit);
}

tt_inline tt_result_t tt_udp_server_async(OUT tt_skt_t *skt,
                                          IN tt_net_family_t family,
                                          IN OPT tt_skt_attr_t *attr,
                                          IN tt_sktaddr_t *local_addr,
                                          IN tt_skt_exit_t *exit)
{
    return tt_udp_server_async_ntv(skt, family, attr, local_addr, exit);
}

#endif /* __TT_SKT_AIO__ */
