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

#include <param/tt_param_cli.h>

#include <algorithm/tt_buffer_format.h>
#include <param/tt_param_dir.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __DEFAULT_COL_SEP "    "

#define __DEFAULT_LINE_SEP "\n"

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_char_t __s_type_name[TT_PARAM_TYPE_NUM][7] = {
    "u32   ", "s32   ", "dir   ", "string", "bool  ", "float ", "exe   ",
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_result_t __param_cli_dir_ls(IN tt_param_dir_t *pd,
                               IN const tt_char_t *col_sep,
                               IN const tt_char_t *line_sep,
                               OUT tt_buf_t *output);

tt_result_t __param_cli_line(IN tt_param_t *p,
                             IN OPT const tt_char_t *col_sep,
                             IN OPT tt_u32_t max_name_len,
                             OUT tt_buf_t *output);

static tt_result_t __line_perm(IN tt_param_t *p, OUT tt_buf_t *output);

static tt_result_t __line_type(IN tt_param_t *p, OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_param_cli_ls(IN tt_param_t *p,
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

    if (p->type == TT_PARAM_DIR) {
        return __param_cli_dir_ls(TT_PARAM_CAST(p, tt_param_dir_t),
                                  col_sep,
                                  line_sep,
                                  output);
    } else {
        return __param_cli_line(p, col_sep, 0, output);
    }
}

tt_result_t __param_cli_dir_ls(IN tt_param_dir_t *pd,
                               IN const tt_char_t *col_sep,
                               IN const tt_char_t *line_sep,
                               OUT tt_buf_t *output)
{
    tt_lnode_t *node;
    tt_u32_t len = (tt_u32_t)tt_strlen(line_sep);
    tt_bool_t empty = TT_TRUE;

    for (node = tt_list_head(&pd->child); node != NULL; node = node->next) {
        tt_param_t *child = TT_CONTAINER(node, tt_param_t, node);

        TT_DO(__param_cli_line(child, col_sep, pd->child_name_len, output));
        TT_DO(tt_buf_put(output, (tt_u8_t *)line_sep, len));
        empty = TT_FALSE;
    }
    if (!empty) {
        // do not need final line's line_sep
        tt_buf_dec_wp(output, len);
    }

    return TT_SUCCESS;
}

tt_result_t __param_cli_line(IN tt_param_t *p,
                             IN OPT const tt_char_t *col_sep,
                             IN OPT tt_u32_t max_name_len,
                             OUT tt_buf_t *output)
{
    tt_u32_t n, name_len;

    TT_ASSERT(p != NULL);
    TT_ASSERT(output != NULL);
    TT_ASSERT(p->itf != NULL);

    if (col_sep == NULL) {
        col_sep = __DEFAULT_COL_SEP;
    }

    // permissions
    __line_perm(p, output);
    TT_DO(tt_buf_put_cstr(output, col_sep));

    // type
    __line_type(p, output);
    TT_DO(tt_buf_put_cstr(output, col_sep));

    // name
    n = TT_COND(p->type == TT_PARAM_DIR, 1, 0);
    name_len = (tt_u32_t)tt_strlen(p->name) + n;
    if (max_name_len != 0) {
        if (max_name_len < name_len) {
            TT_DO(tt_buf_put_rep(output, '?', max_name_len));
        } else {
            TT_DO(tt_buf_put_cstr(output, p->name));
            TT_DO(tt_buf_put_subcstr(output, "/", n));
            TT_DO(tt_buf_put_rep(output, ' ', max_name_len - name_len));
        }
    } else {
        TT_DO(tt_buf_put_cstr(output, p->name));
        TT_DO(tt_buf_put_subcstr(output, "/", n));
    }
    TT_DO(tt_buf_put_cstr(output, col_sep));

    // description
    TT_DO(tt_buf_put_cstr(output, tt_param_brief(p)));

    return TT_SUCCESS;
}

tt_result_t __line_perm(IN tt_param_t *p, OUT tt_buf_t *output)
{
    tt_param_itf_t *itf = p->itf;
    tt_char_t perm[2];

    perm[0] = TT_COND(p->can_read && (itf->read != NULL), 'r', '-');
    perm[1] = TT_COND(p->can_write && (itf->write != NULL), 'w', '-');

    return tt_buf_put(output, (tt_u8_t *)perm, sizeof(perm));
}

tt_result_t __line_type(IN tt_param_t *p, OUT tt_buf_t *output)
{
    return tt_buf_put_cstr(output, __s_type_name[p->type]);
}
