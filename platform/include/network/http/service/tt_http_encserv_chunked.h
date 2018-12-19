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
@file tt_http_enserv_chunked.h
 @brief http encoding service: chunked

this file defines http encoding service chunked APIs
*/

#ifndef __TT_HTTP_ENCSERV_CHUNKED__
#define __TT_HTTP_ENCSERV_CHUNKED__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <network/http/tt_http_encoding_service.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_component_s;
struct tt_profile_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_http_encserv_t *tt_g_http_encserv_chunked;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_http_encserv_chunked_component_init(
    IN struct tt_component_s *comp, IN struct tt_profile_s *profile);

tt_export void tt_http_encserv_chunked_component_exit(
    IN struct tt_component_s *comp);

tt_export tt_http_encserv_t *tt_http_encserv_chunked_create();

#endif /* __TT_HTTP_ENCSERV_CHUNKED__ */
