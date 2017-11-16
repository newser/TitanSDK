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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_config_object.h>

#include <algorithm/tt_buffer_format.h>
#include <init/tt_config_directory.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __DEFAULT_COL_SEP "    "

#define __DEFAULT_LINE_SEP "\n"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_char_t __s_type_name[TT_CFGOBJ_TYPE_NUM][7] = {
    "u32   ", "s32   ", "dir   ", "string", "bool  ", "float ", "exe   ",
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_bool_t __name_ok(IN const tt_char_t *name);

static tt_result_t __line_perm(IN tt_cfgobj_t *co, OUT tt_buf_t *output);

static tt_result_t __line_type(IN tt_cfgobj_t *co, OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgobj_t *tt_cfgobj_create(IN tt_u32_t len,
                              IN tt_cfgobj_type_t type,
                              IN const tt_char_t *name,
                              IN tt_cfgobj_itf_t *itf,
                              IN OPT void *opaque,
                              IN OPT tt_cfgobj_attr_t *attr)
{
    tt_cfgobj_t *co;
    tt_cfgobj_attr_t __attr;

    TT_ASSERT(TT_CFGOBJ_TYPE_VALID(type));
    TT_ASSERT(name != NULL);
    TT_ASSERT(itf != NULL);

    if (!__name_ok(name)) {
        return NULL;
    }

    if (attr == NULL) {
        tt_cfgobj_attr_default(&__attr);
        attr = &__attr;
    }

    co = tt_zalloc(sizeof(tt_cfgobj_t) + len);
    if (co == NULL) {
        TT_ERROR("no mem for cfg node");
        return NULL;
    }

    co->name = name;
    co->display = attr->display;
    co->brief = attr->brief;
    co->detail = attr->detail;

    co->itf = itf;
    co->opaque = opaque;
    tt_lnode_init(&co->node);
    co->type = type;

    co->need_reboot = attr->need_reboot;
    co->can_read = attr->can_read;
    co->can_write = attr->can_write;

    return co;
}

void tt_cfgobj_destroy(IN tt_cfgobj_t *co)
{
    TT_ASSERT(co != NULL);

    if (co->itf->on_destroy != NULL) {
        co->itf->on_destroy(co);
    }

    tt_free(co);
}

void tt_cfgobj_attr_default(IN tt_cfgobj_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->display = "";
    attr->brief = "";
    attr->detail = "";

    attr->need_reboot = TT_FALSE;
    attr->can_read = TT_TRUE;
    attr->can_write = TT_TRUE;
}

tt_result_t tt_cfgobj_line(IN tt_cfgobj_t *co,
                           IN OPT const tt_char_t *col_sep,
                           IN OPT tt_u32_t max_name_len,
                           OUT tt_buf_t *output)
{
    tt_u32_t n, name_len;

    TT_ASSERT(co != NULL);
    TT_ASSERT(output != NULL);
    TT_ASSERT(co->itf != NULL);

    if (col_sep == NULL) {
        col_sep = __DEFAULT_COL_SEP;
    }

    // permissions
    __line_perm(co, output);
    TT_DO(tt_buf_put_cstr(output, col_sep));

    // type
    __line_type(co, output);
    TT_DO(tt_buf_put_cstr(output, col_sep));

    // name
    n = TT_COND(co->type == TT_CFGOBJ_DIR, 1, 0);
    name_len = (tt_u32_t)tt_strlen(co->name) + n;
    if (max_name_len != 0) {
        if (max_name_len < name_len) {
            TT_DO(tt_buf_put_rep(output, '?', max_name_len));
        } else {
            TT_DO(tt_buf_put_cstr(output, co->name));
            TT_DO(tt_buf_put_subcstr(output, "/", n));
            TT_DO(tt_buf_put_rep(output, ' ', max_name_len - name_len));
        }
    } else {
        TT_DO(tt_buf_put_cstr(output, co->name));
        TT_DO(tt_buf_put_subcstr(output, "/", n));
    }
    TT_DO(tt_buf_put_cstr(output, col_sep));

    // description
    TT_DO(tt_buf_put_cstr(output, tt_cfgobj_brief(co)));

    return TT_SUCCESS;
}

tt_bool_t __name_ok(IN const tt_char_t *name)
{
    tt_u32_t i, n;

    n = (tt_u32_t)tt_strlen(name);
    for (i = 0; i < n; ++i) {
        tt_char_t c = name[i];

        if (!tt_isalnum(c) && (c != '-')) {
            TT_ERROR("invalid char: %c, 0x%x", c, c);
            return TT_FALSE;
        }
    }

    return TT_TRUE;
}

tt_result_t __line_perm(IN tt_cfgobj_t *co, OUT tt_buf_t *output)
{
    tt_cfgobj_itf_t *itf = co->itf;
    tt_char_t perm[2];

    perm[0] = TT_COND(co->can_read && (itf->read != NULL), 'r', '-');
    perm[1] = TT_COND(co->can_write && (itf->write != NULL), 'w', '-');

    return tt_buf_put(output, (tt_u8_t *)perm, sizeof(perm));
}

tt_result_t __line_type(IN tt_cfgobj_t *co, OUT tt_buf_t *output)
{
    return tt_buf_put_cstr(output, __s_type_name[co->type]);
}

// ========================================
// node operations
// ========================================

tt_result_t tt_cfgobj_read(IN tt_cfgobj_t *co,
                           IN OPT const tt_char_t *line_sep,
                           OUT tt_buf_t *output)
{
    TT_ASSERT(co != NULL);
    TT_ASSERT(co->itf != NULL);
    TT_ASSERT(output != NULL);

    if (!co->can_read || (co->itf->read == NULL)) {
        return TT_E_UNSUPPORT;
    }

    if (line_sep == NULL) {
        line_sep = __DEFAULT_LINE_SEP;
    }

    return co->itf->read(co, line_sep, output);
}

tt_result_t tt_cfgobj_write(IN tt_cfgobj_t *co,
                            IN tt_u8_t *val,
                            IN tt_u32_t val_len)
{
    TT_ASSERT(co != NULL);
    TT_ASSERT(co->itf != NULL);
    TT_ASSERT(val != NULL);

    if (!co->can_write || (co->itf->write == NULL)) {
        return TT_E_UNSUPPORT;
    }

    return co->itf->write(co, val, val_len);
}

tt_result_t tt_cfgobj_ls(IN tt_cfgobj_t *co,
                         IN OPT const tt_char_t *col_sep,
                         IN OPT const tt_char_t *line_sep,
                         OUT tt_buf_t *output)
{
    if (col_sep == NULL) {
        col_sep = __DEFAULT_COL_SEP;
    }

    if (line_sep == NULL) {
        line_sep = __DEFAULT_LINE_SEP;
    }

    if (co->type == TT_CFGOBJ_DIR) {
        return tt_cfgdir_ls(TT_CFGOBJ_CAST(co, tt_cfgdir_t),
                            col_sep,
                            line_sep,
                            output);
    } else {
        return tt_cfgobj_line(co, col_sep, 0, output);
    }
}
