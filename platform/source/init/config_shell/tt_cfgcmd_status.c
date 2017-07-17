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

#include <init/config_shell/tt_cfgcmd_status.h>

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

static const tt_char_t __status_info[] = "show cached configurations";

static const tt_char_t __status_usage[] = "testing status";

static tt_u32_t __status_run(IN tt_cfgsh_t *sh,
                             IN tt_u32_t argc,
                             IN tt_char_t *arv[],
                             OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_status = {
    TT_CFGCMD_NAME_STATUS, __status_info, __status_usage, __status_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __status_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output);

static tt_u32_t __status_single(IN tt_cfgsh_t *sh,
                                IN tt_char_t *path,
                                OUT tt_buf_t *output);

static tt_u32_t __status_multiple(IN tt_cfgsh_t *sh,
                                  IN tt_char_t *path[],
                                  IN tt_u32_t path_num,
                                  OUT tt_buf_t *output);

static tt_u32_t __max_path_len(IN tt_cfgobj_t *cnode, IN tt_buf_t *path);

static tt_u32_t __max_path_len_of(IN tt_cfgobj_t *cnode,
                                  IN tt_cfgobj_t *root,
                                  IN tt_buf_t *path);

static tt_bool_t __status_node(IN tt_cfgobj_t *cnode,
                               IN tt_cfgobj_t *root,
                               IN tt_u32_t max_name_len,
                               IN tt_buf_t *buf,
                               OUT tt_buf_t *output);

static tt_bool_t __status_grp(IN tt_cfgdir_t *cgrp,
                              IN tt_cfgobj_t *root,
                              IN tt_u32_t max_name_len,
                              IN tt_buf_t *buf,
                              OUT tt_buf_t *output);

static tt_bool_t __status_val(IN tt_cfgobj_t *cnode,
                              IN tt_cfgobj_t *root,
                              IN tt_u32_t max_name_len,
                              IN tt_buf_t *buf,
                              OUT tt_buf_t *output);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __status_run(IN tt_cfgsh_t *sh,
                      IN tt_u32_t argc,
                      IN tt_char_t *argv[],
                      OUT tt_buf_t *output)
{
    if (argc == 0) {
        return __status_current(sh, output);
    } else if (argc == 1) {
        return __status_single(sh, argv[0], output);
    } else {
        return __status_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __status_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output)
{
    tt_cfgobj_t *cnode;
    tt_u32_t max_name_len;
    tt_buf_t buf;

    cnode = sh->current;
    if (cnode == NULL) {
        tt_buf_putf(output, "internal error");
        return TT_CLIOC_OUT;
    }

    tt_buf_init(&buf, NULL);

    max_name_len = __max_path_len(cnode, &buf);
    max_name_len = TT_MAX(max_name_len, 4);

    // title
    tt_buf_put_cstr(output, "STATUS    NAME");
    tt_buf_put_rep(output, ' ', max_name_len);
    tt_buf_put_cstr(output, "DESCRIPTION");
    tt_buf_put_u8(output, TT_CLI_EV_ENTER);

    // cached values
    __status_node(cnode, cnode, max_name_len, &buf, output);

    tt_buf_destroy(&buf);

    return TT_CLIOC_OUT;
}

tt_u32_t __status_single(IN tt_cfgsh_t *sh,
                         IN tt_char_t *path,
                         OUT tt_buf_t *output)
{
    tt_cfgobj_t *cnode;
    tt_blob_t path_blob;
    tt_u32_t max_name_len;
    tt_buf_t buf;

    path_blob.addr = (tt_u8_t *)path;
    path_blob.len = (tt_u32_t)tt_strlen(path);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &path_blob);
    if (cnode == NULL) {
        tt_buf_putf(output, "can not find: %s", path);
        return TT_CLIOC_OUT;
    }

    tt_buf_init(&buf, NULL);

    max_name_len = __max_path_len(cnode, &buf);
    max_name_len = TT_MAX(max_name_len, 4);

    // title
    tt_buf_put_cstr(output, "STATUS    NAME");
    tt_buf_put_rep(output, ' ', max_name_len);
    tt_buf_put_cstr(output, "DESCRIPTION");
    tt_buf_put_u8(output, TT_CLI_EV_ENTER);

    // cached values
    __status_node(cnode, cnode, max_name_len, &buf, output);

    tt_buf_destroy(&buf);

    return TT_CLIOC_OUT;
}

tt_u32_t __status_multiple(IN tt_cfgsh_t *sh,
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

        __status_single(sh, path[i], output);
        tt_buf_put_rep(output, TT_CLI_EV_ENTER, 2);

        has_enter = TT_TRUE;
    }
    if (has_enter) {
        tt_buf_dec_wp(output, 1);
    }

    return TT_CLIOC_OUT;
}

tt_u32_t __max_path_len(IN tt_cfgobj_t *cnode, IN tt_buf_t *pathbuf)
{
    return __max_path_len_of(cnode, cnode, pathbuf);
}

tt_u32_t __max_path_len_of(IN tt_cfgobj_t *cnode,
                           IN tt_cfgobj_t *root,
                           IN tt_buf_t *path)
{
    if (cnode->type == TT_CFGOBJ_DIR) {
        tt_cfgdir_t *cgrp = TT_CFGOBJ_CAST(cnode, tt_cfgdir_t);
        tt_lnode_t *node;
        tt_u32_t max_len = 0;

        node = tt_list_head(&cgrp->child);
        while (node != NULL) {
            tt_cfgobj_t *child = TT_CONTAINER(node, tt_cfgobj_t, node);
            tt_u32_t len;

            node = node->next;

            len = __max_path_len_of(child, root, path);
            max_len = TT_MAX(max_len, len);
        }

        return max_len;
    } else if (cnode->modified || cnode->removing) {
        if (cnode == root) {
            return (tt_u32_t)tt_strlen(cnode->name);
        } else {
            tt_buf_clear(path);
            tt_cfgpath_n2p(root, cnode, path);
            return TT_BUF_RLEN(path);
        }
    } else {
        return 0;
    }
}

tt_bool_t __status_node(IN tt_cfgobj_t *cnode,
                        IN tt_cfgobj_t *root,
                        IN tt_u32_t max_name_len,
                        IN tt_buf_t *buf,
                        OUT tt_buf_t *output)
{
    if (cnode->type == TT_CFGOBJ_DIR) {
        return __status_grp(TT_CFGOBJ_CAST(cnode, tt_cfgdir_t),
                            root,
                            max_name_len,
                            buf,
                            output);
    } else {
        return __status_val(cnode, root, max_name_len, buf, output);
    }
}

tt_bool_t __status_grp(IN tt_cfgdir_t *cgrp,
                       IN tt_cfgobj_t *root,
                       IN tt_u32_t max_name_len,
                       IN tt_buf_t *buf,
                       OUT tt_buf_t *output)
{
    tt_lnode_t *node;
    tt_bool_t has_enter = TT_FALSE;

    node = tt_list_head(&cgrp->child);
    while (node != NULL) {
        tt_cfgobj_t *child = TT_CONTAINER(node, tt_cfgobj_t, node);

        node = node->next;

        if (__status_node(child, root, max_name_len, buf, output)) {
            tt_buf_put_u8(output, TT_CLI_EV_ENTER);
            has_enter = TT_TRUE;
        }
    }
    if (has_enter) {
        tt_buf_dec_wp(output, 1);
    }

    // has_enter means has output
    return has_enter;
}

tt_bool_t __status_val(IN tt_cfgobj_t *cnode,
                       IN tt_cfgobj_t *root,
                       IN tt_u32_t max_name_len,
                       IN tt_buf_t *buf,
                       OUT tt_buf_t *output)
{
    tt_u8_t st[10]; // sizeof("STATUS") + 3

    tt_memset(st, ' ', sizeof(st));
    if (cnode->removing) {
        // STATUS
        st[0] = 'R';
        TT_DO(tt_buf_put(output, st, sizeof(st)));

        // NAME
        tt_buf_clear(buf);
        tt_cfgpath_n2p(root, cnode, buf);
        TT_DO(tt_buf_put(output, TT_BUF_RPOS(buf), TT_BUF_RLEN(buf)));

        // DESCRIPTION

        return TT_TRUE;
    } else if (cnode->modified) {
        tt_u32_t rp, wp, len;
        tt_result_t result;

        // STATUS
        st[0] = 'M';
        TT_DO(tt_buf_put(output, st, sizeof(st)));

        // NAME
        if (cnode == root) {
            len = (tt_u32_t)tt_strlen(cnode->name);
            TT_DO(tt_buf_put_subcstr(output, cnode->name, len));
        } else {
            tt_buf_clear(buf);
            tt_cfgpath_n2p(root, cnode, buf);
            len = TT_BUF_RLEN(buf);
            TT_DO(tt_buf_put(output, TT_BUF_RPOS(buf), len));
        }
        TT_ASSERT(len <= max_name_len);
        TT_DO(tt_buf_put_rep(output, ' ', max_name_len - len + 4));

        // DESCRIPTION
        tt_buf_backup_rwp(output, &rp, &wp);
        result = tt_cfgobj_get(cnode, output);
        if (!TT_OK(result)) {
            tt_buf_restore_rwp(output, &rp, &wp);
            if (result == TT_BAD_PARAM) {
                tt_buf_putf(output, "not supported operation");
            } else {
                tt_buf_putf(output, "internal error");
            }
        }

        return TT_TRUE;
    } else {
        return TT_FALSE;
    }
}
