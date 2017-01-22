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
@file tt_ssl_aio_cb.h
@brief ssl aio callback

this file defines ssl aio callback
*/

#ifndef __TT_SSL_AIO_CB__
#define __TT_SSL_AIO_CB__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>
#include <io/tt_socket_addr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ssl_s;

typedef struct tt_ssl_aioctx_s
{
    tt_result_t result;
    void *cb_param;
} tt_ssl_aioctx_t;

typedef void (*tt_ssl_on_accept_t)(IN struct tt_ssl_s *listening_ssl,
                                   IN struct tt_ssl_s *new_ssl,
                                   IN tt_ssl_aioctx_t *aioctx);

typedef void (*tt_ssl_on_connect_t)(IN struct tt_ssl_s *ssl,
                                    IN tt_sktaddr_t *remote_addr,
                                    IN tt_ssl_aioctx_t *aioctx);

typedef void (*tt_ssl_on_send_t)(IN struct tt_ssl_s *ssl,
                                 IN tt_blob_t *blob,
                                 IN tt_u32_t blob_num,
                                 IN tt_ssl_aioctx_t *aioctx,
                                 IN tt_u32_t send_len);

typedef void (*tt_ssl_on_recv_t)(IN struct tt_ssl_s *ssl,
                                 IN tt_blob_t *blob,
                                 IN tt_u32_t blob_num,
                                 IN tt_ssl_aioctx_t *aioctx,
                                 IN tt_u32_t recv_len);

typedef void (*tt_ssl_on_destroy_t)(IN struct tt_ssl_s *ssl, IN void *cb_param);

typedef struct
{
    tt_ssl_on_destroy_t on_destroy;
    void *cb_param;
} tt_ssl_exit_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_SSL_AIO_CB__ */
