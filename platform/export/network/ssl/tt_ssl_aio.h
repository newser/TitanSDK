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
@file tt_ssl_aio.h
@brief ssl async io

this file defines ssl async io APIs
*/

#ifndef __TT_SSL_AIO__
#define __TT_SSL_AIO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/ssl/tt_ssl.h>

#include <tt_ssl_aio_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ssl_s;
struct tt_sslctx_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// - exit of skt would be overwritten by the new entry specified
//   by exit
// - be sure the skt is not doing io when calling this function
tt_inline tt_result_t tt_async_ssl_create(IN struct tt_ssl_s *ssl,
                                          IN TO tt_skt_t *skt,
                                          IN struct tt_sslctx_s *sslctx,
                                          IN OPT tt_ssl_attr_t *attr,
                                          IN tt_ssl_exit_t *exit)
{
    return tt_async_ssl_create_ntv(ssl, skt, sslctx, attr, exit);
}

tt_inline void tt_async_ssl_destroy(IN struct tt_ssl_s *ssl,
                                    IN tt_bool_t immediate)
{
    tt_async_ssl_destroy_ntv(ssl, immediate);
}

tt_inline void tt_ssl_attr_default(IN tt_ssl_attr_t *attr)
{
    attr->from_alloc = TT_FALSE;

    attr->session_resume = TT_TRUE;
}

/**
 @note
 - best practice is shutting down both read and write whenever sees TT_END
   in tt_ssl_on_recv_t and shutting down write when app confirms all data
   are transferred
 */
tt_inline tt_result_t tt_async_ssl_shutdown(IN struct tt_ssl_s *ssl,
                                            IN tt_u32_t mode)
{
    return tt_async_ssl_shutdown_ntv(ssl, mode);
}

tt_inline tt_result_t tt_ssl_accept_async(IN struct tt_ssl_s *listening_ssl,
                                          IN struct tt_ssl_s *new_ssl,
                                          IN tt_skt_t *new_skt,
                                          IN OPT tt_skt_attr_t *new_skt_attr,
                                          IN OPT tt_ssl_attr_t *new_ssl_attr,
                                          IN tt_ssl_exit_t *new_ssl_exit,
                                          IN tt_ssl_on_accept_t on_accept,
                                          IN OPT void *cb_param)
{
    return tt_ssl_accept_async_ntv(listening_ssl,
                                   new_ssl,
                                   new_skt,
                                   new_skt_attr,
                                   new_ssl_attr,
                                   new_ssl_exit,
                                   on_accept,
                                   cb_param);
}

tt_inline tt_result_t tt_ssl_connect_async(IN struct tt_ssl_s *ssl,
                                           IN tt_sktaddr_t *remote_addr,
                                           IN tt_ssl_on_connect_t on_connect,
                                           IN OPT void *cb_param)
{
    return tt_ssl_connect_async_ntv(ssl, remote_addr, on_connect, cb_param);
}

// it's possible that on_send return TT_END, when peer send a close notify
tt_inline tt_result_t tt_ssl_send_async(IN struct tt_ssl_s *ssl,
                                        IN tt_blob_t *blob,
                                        IN tt_u32_t blob_num,
                                        IN tt_ssl_on_send_t on_send,
                                        IN OPT void *cb_param)
{
    return tt_ssl_send_async_ntv(ssl, blob, blob_num, on_send, cb_param);
}

tt_inline tt_result_t tt_ssl_recv_async(IN struct tt_ssl_s *ssl,
                                        IN tt_blob_t *blob,
                                        IN tt_u32_t blob_num,
                                        IN tt_ssl_on_recv_t on_recv,
                                        IN OPT void *cb_param)
{
    return tt_ssl_recv_async_ntv(ssl, blob, blob_num, on_recv, cb_param);
}

#endif /* __TT_SSL_AIO__ */
