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

/**
@file tt_data_node.h
@brief data node
*/

#ifndef __TT_DATA_NODE__
#define __TT_DATA_NODE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_DTNODE_CAST(dtn, type) TT_PTR_INC(type, dtn, sizeof(tt_dtnode_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_dtnode_s;

typedef void (*tt_dtnode_destroy_t)(IN struct tt_dtnode_s *dtn);

typedef tt_result_t (*tt_dtnode_read_t)(IN struct tt_dtnode_s *dtn,
                                        OUT tt_u8_t **p,
                                        OUT tt_u32_t *len);

typedef void (*tt_dtnode_read_update_t)(IN struct tt_dtnode_s *dtn,
                                        IN tt_u32_t len);

typedef tt_result_t (*tt_dtnode_write_t)(IN struct tt_dtnode_s *dtn,
                                         IN tt_u8_t *p,
                                         IN tt_u32_t len,
                                         OUT OPT tt_u32_t *write_len);

typedef struct tt_dtnode_itf_s
{
    tt_dtnode_destroy_t destroy;
    tt_dtnode_read_t read;
    tt_dtnode_read_update_t read_update;
    tt_dtnode_write_t write;
} tt_dtnode_itf_t;

typedef struct tt_dtnode_s
{
    tt_dtnode_itf_t *itf;
} tt_dtnode_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_dtnode_t *tt_dtnode_create(IN tt_u32_t size,
                                        IN tt_dtnode_itf_t *itf);

tt_export void tt_dtnode_destroy(IN tt_dtnode_t *dtn);

tt_inline tt_result_t tt_dtnode_read(IN tt_dtnode_t *dtn,
                                     OUT tt_u8_t **p,
                                     OUT tt_u32_t *len)
{
    if (dtn->itf->read != NULL) {
        return dtn->itf->read(dtn, p, len);
    } else {
        return TT_E_UNSUPPORT;
    }
}

tt_inline void tt_dtnode_read_update(IN tt_dtnode_t *dtn, IN tt_u32_t len)
{
    if (dtn->itf->read != NULL) {
        dtn->itf->read_update(dtn, len);
    }
}

tt_inline tt_result_t tt_dtnode_write(IN tt_dtnode_t *dtn,
                                      IN tt_u8_t *p,
                                      IN tt_u32_t len,
                                      OUT OPT tt_u32_t *write_len)
{
    if (dtn->itf->write != NULL) {
        return dtn->itf->write(dtn, p, len, write_len);
    } else {
        return TT_E_UNSUPPORT;
    }
}

#endif // __TT_DATA_NODE__
