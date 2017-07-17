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
@file tt_config_object.h
@brief config object

this file defines config object
*/

#ifndef __TT_CONFIG_OBJECT__
#define __TT_CONFIG_OBJECT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <init/tt_config_object_def.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CFGOBJ_CAST(co, type) TT_PTR_INC(type, co, sizeof(tt_cfgobj_t))
#define TT_CFGOBJ_OF(p) TT_PTR_DEC(tt_cfgobj_t, p, sizeof(tt_cfgobj_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *display;
    const tt_char_t *brief;
    const tt_char_t *detail;

    tt_bool_t need_reboot : 1;
    tt_bool_t can_read : 1;
    tt_bool_t can_write : 1;
    tt_bool_t can_exec : 1;
} tt_cfgobj_attr_t;

typedef struct tt_cfgobj_s
{
    const tt_char_t *name;
    const tt_char_t *display;
    const tt_char_t *brief;
    const tt_char_t *detail;

    tt_cfgobj_itf_t *itf;
    void *opaque;
    tt_lnode_t node;
    tt_cfgobj_type_t type;

    tt_bool_t need_reboot : 1;
    tt_bool_t can_read : 1;
    tt_bool_t can_write : 1;
    tt_bool_t can_exec : 1;
} tt_cfgobj_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_cfgobj_t *tt_cfgobj_create(IN tt_u32_t len,
                                     IN tt_cfgobj_type_t type,
                                     IN const tt_char_t *name,
                                     IN tt_cfgobj_itf_t *itf,
                                     IN OPT void *opaque,
                                     IN OPT tt_cfgobj_attr_t *attr);

extern void tt_cfgobj_destroy(IN tt_cfgobj_t *co);

extern void tt_cfgobj_attr_default(IN tt_cfgobj_attr_t *attr);

tt_inline const tt_char_t *tt_cfgobj_name(IN tt_cfgobj_t *co)
{
    TT_ASSERT(co->name != NULL);
    return co->name;
}

tt_inline const tt_char_t *tt_cfgobj_display(IN tt_cfgobj_t *co)
{
    return TT_COND(co->display != NULL, co->display, "");
}

tt_inline const tt_char_t *tt_cfgobj_brief(IN tt_cfgobj_t *co)
{
    return TT_COND(co->brief != NULL, co->brief, "");
}

tt_inline const tt_char_t *tt_cfgobj_detail(IN tt_cfgobj_t *co)
{
    return TT_COND(co->detail != NULL, co->detail, "");
}

// - if name_len is less than node name length, then at most name_len of
//   name would be outputted followed by a space
// - if name_len is larger, spaces are filled
extern tt_result_t tt_cfgobj_line(IN tt_cfgobj_t *co,
                                  IN OPT const tt_char_t *col_sep,
                                  IN OPT tt_u32_t max_name_len,
                                  OUT struct tt_buf_s *output);

// ========================================
// node operations
// ========================================

extern tt_result_t tt_cfgobj_read(IN tt_cfgobj_t *co,
                                  IN OPT const tt_char_t *line_sep,
                                  OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgobj_write(IN tt_cfgobj_t *co,
                                   IN tt_u8_t *val,
                                   IN tt_u32_t val_len);

extern tt_result_t tt_cfgobj_exec(IN tt_cfgobj_t *co,
                                  IN tt_u8_t *arg,
                                  IN tt_u32_t arg_len,
                                  IN OPT const tt_char_t *line_sep,
                                  OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgobj_ls(IN tt_cfgobj_t *co,
                                IN OPT const tt_char_t *col_sep,
                                IN OPT const tt_char_t *line_sep,
                                OUT struct tt_buf_s *output);

#endif /* __TT_CONFIG_OBJECT__ */
