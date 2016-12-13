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
@file tt_log_io.h
@brief log io

this file defines log io
*/

#ifndef __TT_LOG_IO__
#define __TT_LOG_IO__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_LOGIO_CAST(lio, type) TT_PTR_INC(type, lio, sizeof(tt_logio_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logio_s;

typedef enum {
    TT_LOGIO_TYPE_STD,

    TT_LOGIO_TYPE_NUM
} tt_logio_type_t;
#define TT_LOGIO_TYPE_VALID(t) ((t) < TT_LOGIO_TYPE_NUM)

// return how many bytes sent
typedef tt_u32_t (*tt_logio_output_t)(IN struct tt_logio_s *lio,
                                      IN tt_u8_t *data,
                                      IN tt_u32_t data_len);

typedef void (*tt_logio_destroy_t)(IN struct tt_logio_s *lio);

typedef struct
{
    tt_logio_output_t output;
    tt_logio_destroy_t destroy;
} tt_logio_itf_t;

// usage of log destination is to transfer log data, and it
// may copy log data until data is transferred or confirmed
typedef struct tt_logio_s
{
    tt_logio_type_t type;
    struct tt_spinlock_s *lock;

    tt_logio_itf_t itf;
} tt_logio_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// size does not count sizeof(tt_logio_t)
extern tt_logio_t *tt_logio_create(IN tt_u32_t size,
                                   IN tt_logio_type_t type,
                                   IN tt_logio_itf_t *itf);

extern void tt_logio_destroy(IN tt_logio_t *lio);

extern tt_u32_t tt_logio_output(IN tt_logio_t *lio,
                                IN tt_u8_t *data,
                                IN tt_u32_t data_len);

extern tt_result_t tt_logio_enable_lock(IN tt_logio_t *lio);

#endif /* __TT_LOG_IO__ */
