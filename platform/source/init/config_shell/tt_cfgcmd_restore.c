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

#include <init/config_shell/tt_cfgcmd_restore.h>

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

static const tt_char_t __restore_info[] = "restore cached configurations";

static const tt_char_t __restore_usage[] = "testing restore";

static tt_u32_t __restore_run(IN tt_cfgsh_t *sh,
                              IN tt_u32_t argc,
                              IN tt_char_t *arv[],
                              OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_restore = {
    TT_CFGCMD_NAME_RESTORE, __restore_info, __restore_usage, __restore_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __restore_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output);

static tt_u32_t __restore_single(IN tt_cfgsh_t *sh,
                                 IN tt_char_t *path,
                                 OUT tt_buf_t *output);

static tt_u32_t __restore_multiple(IN tt_cfgsh_t *sh,
                                   IN tt_char_t *path[],
                                   IN tt_u32_t path_num,
                                   OUT tt_buf_t *output);

static tt_u32_t __restore_node(IN tt_cfgnode_t *node);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __restore_run(IN tt_cfgsh_t *sh,
                       IN tt_u32_t argc,
                       IN tt_char_t *argv[],
                       OUT tt_buf_t *output)
{
    if (argc == 0) {
        return __restore_current(sh, output);
    } else if (argc == 1) {
        return __restore_single(sh, argv[0], output);
    } else {
        return __restore_multiple(sh, argv, argc, output);
    }
}

tt_u32_t __restore_current(IN tt_cfgsh_t *sh, OUT tt_buf_t *output)
{
    tt_cfgnode_t *cnode;

    cnode = sh->current;
    if (cnode == NULL) {
        tt_buf_putf(output, "internal error");
        return TT_CLIOC_OUT;
    }

    tt_cfgnode_restore(cnode);
    return TT_CLIOC_NOOUT;
}

tt_u32_t __restore_single(IN tt_cfgsh_t *sh,
                          IN tt_char_t *path,
                          OUT tt_buf_t *output)
{
    tt_cfgnode_t *cnode;
    tt_blob_t path_blob;

    path_blob.addr = (tt_u8_t *)path;
    path_blob.len = (tt_u32_t)tt_strlen(path);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &path_blob);
    if (cnode == NULL) {
        tt_buf_putf(output, "can not find: %s", path);
        return TT_CLIOC_OUT;
    }

    tt_cfgnode_restore(cnode);
    return TT_CLIOC_NOOUT;
}

tt_u32_t __restore_multiple(IN tt_cfgsh_t *sh,
                            IN tt_char_t *path[],
                            IN tt_u32_t path_num,
                            OUT tt_buf_t *output)
{
    tt_u32_t i;
    tt_u32_t result = TT_CLIOC_NOOUT;

    for (i = 0; i < path_num; ++i) {
        if (__restore_single(sh, path[i], output) == TT_CLIOC_OUT) {
            tt_buf_put_u8(output, TT_CLI_EV_ENTER);
            result = TT_CLIOC_OUT;
        }
    }
    if (result == TT_CLIOC_OUT) {
        tt_buf_dec_wp(output, 1);
    }

    return result;
}

tt_u32_t __restore_node(IN tt_cfgnode_t *cnode)
{
    if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
        tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
        tt_lnode_t *node;

        // restore children
        node = tt_list_head(&cgrp->child);
        while (node != NULL) {
            tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

            node = node->next;

            __restore_node(child);
        }

        // remove added children
        while ((node = tt_list_pophead(&cgrp->new_child)) != NULL) {
            tt_cfgnode_destroy(TT_CONTAINER(node, tt_cfgnode_t, node),
                               TT_FALSE);
        }
    } else {
        cnode->modified = TT_FALSE;
        cnode->removing = TT_FALSE;
    }

    return TT_CLIOC_NOOUT;
}
