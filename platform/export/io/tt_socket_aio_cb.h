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
@file tt_socket_aio_cb.h
@brief async socket io callback

this file defines async socket io callback
*/

#ifndef __TT_SOCKET_AIO_CB__
#define __TT_SOCKET_AIO_CB__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_skt_s;

typedef struct
{
    tt_result_t result;
    void *cb_param;
} tt_skt_aioctx_t;

typedef void (*tt_skt_on_accept_t)(IN struct tt_skt_s *listening_skt,
                                   IN struct tt_skt_s *new_skt,
                                   IN tt_skt_aioctx_t *aioctx);

typedef void (*tt_skt_on_connect_t)(IN struct tt_skt_s *skt,
                                    IN tt_sktaddr_t *remote_addr,
                                    IN tt_skt_aioctx_t *aioctx);

/**
@return(by aioctx->result)
- TT_SUCCESS if any data has been sent
- TT_FAIL if no data has been sent due to some error
*/
typedef void (*tt_skt_on_send_t)(IN struct tt_skt_s *skt,
                                 IN tt_blob_t *blob,
                                 IN tt_u32_t blob_num,
                                 IN tt_skt_aioctx_t *aioctx,
                                 IN tt_u32_t send_len);

/**
@return(by aioctx->result)
- TT_SUCCESS if any data has been received and filled to buffer
- TT_FAIL if no data has been received due to some error
- TT_END if no data has been received due to disconnection
*/
typedef void (*tt_skt_on_recv_t)(IN struct tt_skt_s *skt,
                                 IN tt_blob_t *blob,
                                 IN tt_u32_t blob_num,
                                 IN tt_skt_aioctx_t *aioctx,
                                 IN tt_u32_t recv_len);

typedef void (*tt_skt_on_sendto_t)(IN struct tt_skt_s *skt,
                                   IN tt_blob_t *blob,
                                   IN tt_u32_t blob_num,
                                   IN tt_sktaddr_t *remote_addr,
                                   IN tt_skt_aioctx_t *aioctx,
                                   IN tt_u32_t send_len);

typedef void (*tt_skt_on_recvfrom_t)(IN struct tt_skt_s *skt,
                                     IN tt_blob_t *blob,
                                     IN tt_u32_t blob_num,
                                     IN tt_skt_aioctx_t *aioctx,
                                     IN tt_u32_t recv_len,
                                     IN tt_sktaddr_t *remote_addr);

typedef void (*tt_skt_on_destroy_t)(IN struct tt_skt_s *skt, IN void *cb_param);

typedef struct
{
    tt_skt_on_destroy_t on_destroy;
    void *cb_param;
} tt_skt_exit_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_SOCKET_AIO_CB__ */
