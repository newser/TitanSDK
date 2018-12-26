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
@file tt_http_inserv_file.h
@brief http service

this file defines http incoming service: get file
*/

#ifndef __TT_HTTP_INSERVICE_FILE__
#define __TT_HTTP_INSERVICE_FILE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>
#include <network/http/tt_http_in_service.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_http_contype_map_s;

typedef struct
{
    struct tt_http_contype_map_s *contype_map;
    tt_s32_t chunk_size;
    tt_bool_t can_have_path_param : 1;
    tt_bool_t can_have_query_param : 1;
    tt_bool_t process_post : 1;
    tt_bool_t enable_etag : 1;
} tt_http_inserv_file_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_http_inserv_t *tt_http_inserv_file_create(
    IN OPT tt_http_inserv_file_attr_t *attr);

tt_export void tt_http_inserv_file_attr_default(
    IN tt_http_inserv_file_attr_t *attr);

#endif /* __TT_HTTP_INSERVICE_FILE__ */
