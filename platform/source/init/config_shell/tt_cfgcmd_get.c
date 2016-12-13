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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/config_shell/tt_cfgcmd_get.h>

#include <algorithm/tt_buffer_format.h>
#include <init/config_shell/tt_config_shell.h>
#include <init/tt_config_group.h>
#include <init/tt_config_path.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static const tt_char_t __get_info[] = "get value";

static const tt_char_t __get_usage[] = "testing get";

static tt_u32_t __get_run(IN tt_cfgsh_t *sh,
                          IN tt_u32_t argc,
                          IN tt_char_t *arv[],
                          OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_get = {
    TT_CFGCMD_NAME_GET, __get_info, __get_usage, __get_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __get_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output);

static tt_u32_t __get_single(IN tt_cfgsh_t *sh,
                             IN tt_char_t *path,
                             OUT tt_buf_t *output);

static tt_u32_t __get_multiple(IN tt_cfgsh_t *sh,
                               IN tt_char_t *path[],
                               IN tt_u32_t path_num,
                               OUT tt_buf_t *output);

static tt_u32_t __get_grp(IN tt_cfgnode_t *cnode, OUT tt_buf_t *output);

static tt_u32_t __get_val(IN tt_cfgnode_t *cnode,
                          IN tt_u32_t name_len,
                          OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __get_run(IN tt_cfgsh_t *sh,
                   IN tt_u32_t argc,
                   IN tt_char_t *argv[],
                   OUT tt_buf_t *output)
{
    if (argc == 0) {
        return __get_current(sh, output);
    } else if (argc == 1) {
        return __get_single(sh, argv[0], output);
    } else {
        return __get_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __get_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output)
{
    if (sh->current == NULL) {
        tt_buf_vput(output, "internal error");
        return TT_CLIOC_OUT;
    }

    return __get_grp(sh->current, output);
}

tt_u32_t __get_single(IN tt_cfgsh_t *sh,
                      IN tt_char_t *path,
                      OUT tt_buf_t *output)
{
    tt_blob_t path_blob;
    tt_cfgnode_t *cnode;

    path_blob.addr = (tt_u8_t *)path;
    path_blob.len = (tt_u32_t)tt_strlen(path);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &path_blob);
    if (cnode == NULL) {
        tt_buf_vput(output, "can not find: %s", path);
        return TT_CLIOC_OUT;
    }

    if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
        return __get_grp(cnode, output);
    } else {
        return __get_val(cnode, 0, output);
    }
}

tt_u32_t __get_multiple(IN tt_cfgsh_t *sh,
                        IN tt_char_t *path[],
                        IN tt_u32_t path_num,
                        OUT tt_buf_t *output)
{
    tt_u32_t i;
    tt_bool_t has_enter = TT_FALSE;

    for (i = 0; i < path_num; ++i) {
        tt_buf_put_cstr(output, path[i]);
        tt_buf_put_u8(output, ':');
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);

        __get_single(sh, path[i], output);
        tt_buf_put_rep(output, TT_CLI_EV_ENTER, 2);

        has_enter = TT_TRUE;
    }
    if (has_enter) {
        tt_buf_dec_wp(output, 1);
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __get_grp(IN tt_cfgnode_t *cnode, OUT tt_buf_t *output)
{
    tt_cfggrp_t *cgrp;
    tt_lnode_t *node;
    tt_u32_t max_len = 8;

    TT_ASSERT(cnode->type == TT_CFGNODE_TYPE_GROUP);
    cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);

    // calc name length
    node = tt_list_head(&cgrp->child);
    while (node != NULL) {
        tt_cfgnode_t *cnode = TT_CONTAINER(node, tt_cfgnode_t, node);
        tt_u32_t len = (tt_u32_t)tt_strlen(cnode->name) + 4;

        node = node->next;

        max_len = TT_MAX(max_len, len);
    }

    tt_buf_put_cstr(output, "NAME");
    tt_buf_put_rep(output, ' ', TT_COND(max_len >= 8, max_len - 4, 4));
    tt_buf_put_cstr(output, "VALUE");
    tt_buf_put_u8(output, TT_CLI_EV_ENTER);

    node = tt_list_head(&cgrp->child);
    while (node != NULL) {
        tt_cfgnode_t *cnode = TT_CONTAINER(node, tt_cfgnode_t, node);

        node = node->next;

        if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
            // put name
            tt_buf_put_cstr(output, cnode->name);
            tt_buf_put_rep(output,
                           ' ',
                           max_len - (tt_u32_t)tt_strlen(cnode->name));

            // put value
            tt_buf_put_cstr(output, "n/a(group)");
        } else {
            __get_val(cnode, max_len, output);
        }
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);
    }
    tt_buf_dec_wp(output, 1);

    return TT_CLIOC_OUT;
}

tt_u32_t __get_val(IN tt_cfgnode_t *cnode,
                   IN tt_u32_t max_len,
                   OUT tt_buf_t *output)
{
    tt_u32_t len = (tt_u32_t)tt_strlen(cnode->name);
    tt_u32_t rp, wp;
    tt_result_t result;

    // put title
    if (max_len == 0) {
        max_len = (tt_u32_t)tt_strlen(cnode->name) + 4;
        max_len = TT_MAX(max_len, 8);
        tt_buf_put_cstr(output, "NAME");
        tt_buf_put_rep(output, ' ', TT_COND(max_len >= 8, max_len - 4, 4));
        tt_buf_put_cstr(output, "VALUE");
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);
    }

    // put name
    TT_ASSERT(len < max_len);
    tt_buf_put_cstr(output, cnode->name);
    tt_buf_put_rep(output, ' ', max_len - len);

    // put value
    tt_buf_backup_rwp(output, &rp, &wp);
    result = tt_cfgnode_get(cnode, output);
    if (!TT_OK(result)) {
        tt_buf_restore_rwp(output, &rp, &wp);
        if (result == TT_BAD_PARAM) {
            tt_buf_vput(output, "not supported operation");
        } else {
            tt_buf_vput(output, "internal error");
        }
    }

    return TT_CLIOC_OUT;
}
