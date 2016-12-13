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
@file tt_ssl_aio_native.h
@brief ssl aio portlayer

this file defines ssl aio portlayer APIs
*/

#ifndef __TT_SSL_AIO_NATIVE__
#define __TT_SSL_AIO_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_socket.h>
#include <network/ssl/tt_ssl_aio_cb.h>
#include <network/ssl/tt_ssl_context.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ssl_s;
struct tt_ssl_attr_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_async_ssl_create_ntv(IN struct tt_ssl_s *ssl,
                                           IN TO tt_skt_t *skt,
                                           IN tt_sslctx_t *ssl_ctx,
                                           IN OPT struct tt_ssl_attr_s *attr,
                                           IN tt_ssl_exit_t *exit);

extern void tt_async_ssl_destroy_ntv(IN struct tt_ssl_s *ssl,
                                     IN tt_bool_t immediate);

extern tt_result_t tt_async_ssl_shutdown_ntv(IN struct tt_ssl_s *ssl,
                                             IN tt_u32_t mode);

extern tt_result_t tt_ssl_accept_async_ntv(
    IN struct tt_ssl_s *listening_ssl,
    IN struct tt_ssl_s *new_ssl,
    IN tt_skt_t *new_skt,
    IN OPT tt_skt_attr_t *new_skt_attr,
    IN OPT struct tt_ssl_attr_s *new_ssl_attr,
    IN tt_ssl_exit_t *new_ssl_exit,
    IN tt_ssl_on_accept_t on_accept,
    IN OPT void *cb_param);

extern tt_result_t tt_ssl_connect_async_ntv(IN struct tt_ssl_s *skt,
                                            IN tt_sktaddr_t *remote_addr,
                                            IN tt_ssl_on_connect_t on_connect,
                                            IN OPT void *cb_param);

extern tt_result_t tt_ssl_send_async_ntv(IN struct tt_ssl_s *ssl,
                                         IN tt_blob_t *blob,
                                         IN tt_u32_t blob_num,
                                         IN tt_ssl_on_send_t on_send,
                                         IN OPT void *cb_param);

extern tt_result_t tt_ssl_recv_async_ntv(IN struct tt_ssl_s *ssl,
                                         IN tt_blob_t *blob,
                                         IN tt_u32_t blob_num,
                                         IN tt_ssl_on_recv_t on_recv,
                                         IN OPT void *cb_param);

#endif /* __TT_SSL_AIO_NATIVE__ */
