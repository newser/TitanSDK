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
@file tt_memory_spring.h
@brief memory spring APIs

APIs to extend/compress memory
*/

#ifndef __TT_MEMORY_SPRING__
#define __TT_MEMORY_SPRING__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_u32_t min_extend;
    tt_u32_t max_extend;
    tt_u32_t max_limit;
} tt_memspg_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// must: 0 < min_extend < max_extend < max_limit, or max_limit equals 0
tt_export void tt_memspg_init(IN tt_memspg_t *mspg,
                              IN tt_u32_t min_extend,
                              IN tt_u32_t max_extend,
                              IN tt_u32_t max_limit);

tt_export tt_result_t tt_memspg_extend_ex(IN tt_memspg_t *mspg,
                                          IN OUT tt_u8_t **p,
                                          IN OUT tt_u32_t *size,
                                          IN tt_u32_t to_size,
                                          IN tt_u32_t flag);
#define TT_MSPGEXT_ZERO (1 << 0)
#define TT_MSPGEXT_NOFREE (1 << 1)

tt_inline tt_result_t tt_memspg_extend(IN tt_memspg_t *mspg,
                                       IN OUT tt_u8_t **p,
                                       IN OUT tt_u32_t *size,
                                       IN tt_u32_t to_size)
{
    return tt_memspg_extend_ex(mspg, p, size, to_size, 0);
}

tt_export tt_result_t tt_memspg_compress(IN tt_memspg_t *mspg,
                                         IN OUT tt_u8_t **p,
                                         IN OUT tt_u32_t *size,
                                         IN tt_u32_t to_size);

tt_export tt_result_t tt_memspg_compress_range(IN tt_memspg_t *mspg,
                                               IN OUT tt_u8_t **p,
                                               IN OUT tt_u32_t *size,
                                               IN tt_u32_t from,
                                               IN tt_u32_t to);

// return 0 if exceeding limit
tt_export tt_u32_t tt_memspg_next_size(IN tt_memspg_t *mspg, IN tt_u32_t size);

#endif // __TT_MEMORY_SPRING__
