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
@file tt_param.h
@brief config object

this file defines config object
*/

#ifndef __TT_PARAM__
#define __TT_PARAM__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <param/html/bootstrap4/tt_param_bs4_control.h>
#include <param/tt_param_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_PARAM_CAST(p, type) TT_PTR_INC(type, p, sizeof(tt_param_t))

#define TT_PARAM_OF(p) TT_PTR_DEC(tt_param_t, p, sizeof(tt_param_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_PARAM_LV_INFO, // always read only
    TT_PARAM_LV_GUEST, // configurable by guest
    TT_PARAM_LV_USER, // configurable by guest
    TT_PARAM_LV_ADMIN, // configurable by admin
    TT_PARAM_LV_SECURE_INFO, // can only be seen by admin
    TT_PARAM_LV_SECURE, // can only configurable by admin

    TT_PARAM_LV_NUM
} tt_param_level_t;
#define TT_PARAM_LV_VALID(l) ((l) < TT_PARAM_LV_NUM)

typedef struct
{
    const tt_char_t *display;
    const tt_char_t *brief;
    const tt_char_t *detail;

    tt_bool_t need_reboot : 1;
    tt_bool_t can_read : 1;
    tt_bool_t can_write : 1;
    tt_param_level_t level : 2;
} tt_param_attr_t;

typedef struct tt_param_s
{
    const tt_char_t *name;
    const tt_char_t *display;
    const tt_char_t *brief;
    const tt_char_t *detail;

    tt_param_itf_t *itf;
    void *opaque;
    tt_lnode_t node;
    tt_param_type_t type;
    tt_param_bs4_ctrl_t bs4_ctrl;

    tt_bool_t need_reboot : 1;
    tt_bool_t can_read : 1;
    tt_bool_t can_write : 1;
    tt_param_level_t level : 3;
} tt_param_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_param_t *tt_g_param_root;

tt_export tt_param_t *tt_g_param_platform;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_param_component_register();

tt_export tt_result_t tt_param_add2root(IN tt_param_t *p);

tt_export tt_result_t tt_param_add2platform(IN tt_param_t *p);

tt_export tt_param_t *tt_param_create(IN tt_u32_t len,
                                      IN tt_param_type_t type,
                                      IN const tt_char_t *name,
                                      IN tt_param_itf_t *itf,
                                      IN OPT void *opaque,
                                      IN OPT tt_param_attr_t *attr);

tt_export void tt_param_destroy(IN tt_param_t *p);

tt_export void tt_param_attr_default(IN tt_param_attr_t *attr);

tt_inline const tt_char_t *tt_param_name(IN tt_param_t *p)
{
    TT_ASSERT(p->name != NULL);
    return p->name;
}

tt_inline const tt_char_t *tt_param_display(IN tt_param_t *p)
{
    return TT_COND(p->display != NULL, p->display, "");
}

tt_inline const tt_char_t *tt_param_brief(IN tt_param_t *p)
{
    return TT_COND(p->brief != NULL, p->brief, "");
}

tt_inline const tt_char_t *tt_param_detail(IN tt_param_t *p)
{
    return TT_COND(p->detail != NULL, p->detail, "");
}

tt_export tt_result_t tt_param_read(IN tt_param_t *p,
                                    OUT struct tt_buf_s *output);

tt_export tt_result_t tt_param_write(IN tt_param_t *p,
                                     IN tt_u8_t *val,
                                     IN tt_u32_t val_len);

tt_export tt_param_t *tt_param_parent(IN tt_param_t *p);

#endif /* __TT_PARAM__ */
