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
@file tt_fiber_windows.h
@brief fiber wrapper windows
*/

#ifndef __TT_FIBER_WINDOWS__
#define __TT_FIBER_WINDOWS__

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

struct tt_fiber_s;
struct tt_fiber_sched_s;

typedef struct tt_fiber_wrap_s
{
    LPVOID fb;
    struct tt_fiber_s *from;
} tt_fiber_wrap_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_fiber_create_wrap(IN tt_fiber_wrap_t *wrap_fb,
                                           IN tt_u32_t stack_size);

tt_export void tt_fiber_destroy_wrap(IN tt_fiber_wrap_t *wrap_fb);

tt_export tt_result_t tt_fiber_create_local_wrap(IN tt_fiber_wrap_t *wrap_fb);

tt_export void tt_fiber_destroy_local_wrap(IN tt_fiber_wrap_t *wrap_fb);

tt_export void tt_fiber_switch_wrap(IN struct tt_fiber_sched_s *fs,
                                    IN struct tt_fiber_s *from,
                                    IN struct tt_fiber_s *to);

#endif /* __TT_FIBER_WINDOWS__ */
