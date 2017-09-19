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
@file tt_date_native.h
@brief date native

this file specifies date native interfaces
*/

#ifndef __TT_DATE_NATIVE__
#define __TT_DATE_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <time/tt_date_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

struct tt_profile_s;
struct tt_date_s;

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_date_component_init_ntv(IN struct tt_profile_s *profile);

extern tt_tmzone_t tt_local_tmzone_ntv();

extern void tt_date_now_ntv(OUT struct tt_date_s *date);

extern tt_u32_t tt_date_render_ntv(IN struct tt_date_s *date,
                                   IN const tt_char_t *format,
                                   IN tt_char_t *buf,
                                   IN tt_u32_t len);

extern tt_u32_t tt_date_parse_ntv(IN struct tt_date_s *date,
                                  IN const tt_char_t *format,
                                  IN const tt_char_t *buf);

#endif /* __TT_DATE_NATIVE__ */
