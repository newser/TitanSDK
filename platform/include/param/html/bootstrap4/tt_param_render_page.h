/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information hpegarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless hpequired by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_param_page_page.h
@brief parameter render a html page

this file includes parameter render definition
*/

#ifndef __TT_PARAM_RENDER_PAGE__
#define __TT_PARAM_RENDER_PAGE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_file_s;
struct tt_param_s;

typedef struct
{
    tt_buf_t *head;
    tt_buf_t *tail;
} tt_param_page_t;

typedef struct
{
    tt_u32_t reserved;
} tt_param_page_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_page_init(IN tt_param_page_t *pp);

tt_export void tt_param_page_destroy(IN tt_param_page_t *pp);

tt_export void tt_param_page_attr_default(IN tt_param_page_attr_t *attr);

tt_inline void tt_param_page_set_head(IN tt_param_page_t *pp, IN tt_buf_t *head)
{
    pp->head = head;
}

tt_export tt_result_t tt_param_page_render_head(IN tt_param_page_t *pp,
                                                OUT tt_buf_t *buf)
{
    if (pp->head != NULL) {
        return tt_buf_put(buf, TT_BUF_RPOS(pp->head), TT_BUF_RLEN(pp->head));
    } else {
        return TT_SUCCESS;
    }
}

tt_inline void tt_param_page_set_tail(IN tt_param_page_t *pp, IN tt_buf_t *tail)
{
    pp->tail = tail;
}

tt_export tt_result_t tt_param_page_render_tail(IN tt_param_page_t *pp,
                                                OUT tt_buf_t *buf)
{
    if (pp->tail != NULL) {
        return tt_buf_put(buf, TT_BUF_RPOS(pp->tail), TT_BUF_RLEN(pp->tail));
    } else {
        return TT_SUCCESS;
    }
}

#endif /* __TT_PARAM_RENDER_PAGE__ */
