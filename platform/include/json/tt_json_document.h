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
@file tt_json_document.h
@brief json document

this file specifies json document APIs
*/

#ifndef __TT_JSON_DOCUMENT__
#define __TT_JSON_DOCUMENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <json/tt_json_value_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

typedef struct tt_jdoc_s
{
    tt_ptr_t p;
} tt_jdoc_t;

typedef enum {
    TT_JDOC_AUTO,
    TT_JDOC_UTF8,
    TT_JDOC_UTF16_LE,
    TT_JDOC_UTF16_BE,
    TT_JDOC_UTF32_LE,
    TT_JDOC_UTF32_BE,

    TT_JDOC_ENCODING_NUM
} tt_jdoc_encoding_t;
#define TT_JDOC_ENCODING_VALID(e) ((e) < TT_JDOC_ENCODING_NUM)

typedef struct
{
    tt_jdoc_encoding_t encoding;
} tt_jdoc_parse_attr_t;

typedef struct
{
    tt_jdoc_encoding_t encoding;
    tt_bool_t bom : 1;
} tt_jdoc_render_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_jdoc_create(IN tt_jdoc_t *jd);

tt_export void tt_jdoc_destroy(IN tt_jdoc_t *jd);

tt_export void tt_jdoc_clear(IN tt_jdoc_t *jd);

tt_export void tt_jdoc_parse_attr_default(IN tt_jdoc_parse_attr_t *attr);

tt_export tt_jval_t *tt_jdoc_get_root(IN tt_jdoc_t *jd);

tt_export void tt_jdoc_set_root(IN tt_jdoc_t *jd, IN TO tt_jval_t *jv);

tt_export tt_result_t tt_jdoc_parse(IN tt_jdoc_t *jd,
                                    IN void *buf,
                                    IN tt_u32_t len,
                                    IN OPT tt_jdoc_parse_attr_t *attr);

tt_export tt_result_t tt_jdoc_parse_file(IN tt_jdoc_t *jd,
                                         IN const tt_char_t *path,
                                         IN OPT tt_jdoc_parse_attr_t *attr);

tt_export void tt_jdoc_render_attr_default(IN tt_jdoc_render_attr_t *attr);

tt_export tt_result_t tt_jdoc_render(IN tt_jdoc_t *jd,
                                     IN struct tt_buf_s *buf,
                                     IN OPT tt_jdoc_render_attr_t *attr);

tt_export tt_result_t tt_jdoc_render_file(IN tt_jdoc_t *jd,
                                          IN const tt_char_t *path,
                                          IN OPT tt_jdoc_render_attr_t *attr);

#endif /* __TT_JSON_DOCUMENT__ */
