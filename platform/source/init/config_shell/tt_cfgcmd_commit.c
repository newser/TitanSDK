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

#include <init/config_shell/tt_cfgcmd_commit.h>

#include <algorithm/tt_buffer_format.h>
#include <init/config_shell/tt_config_shell.h>
#include <init/tt_config_group.h>

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

static const tt_char_t __commit_info[] = "commit cached configurations";

static const tt_char_t __commit_usage[] = "testing commit";

static tt_u32_t __commit_run(IN tt_cfgsh_t *sh,
                             IN tt_u32_t argc,
                             IN tt_char_t *arv[],
                             OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_commit = {
    TT_CFGCMD_NAME_COMMIT, __commit_info, __commit_usage, __commit_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __commit_on_read(IN struct tt_cli_s *cli,
                                 IN const tt_char_t *content,
                                 IN tt_buf_t *output);

static tt_bool_t __need_reboot(IN tt_cfgnode_t *cnode, OUT tt_bool_t *changed);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __commit_run(IN tt_cfgsh_t *sh,
                      IN tt_u32_t argc,
                      IN tt_char_t *argv[],
                      OUT tt_buf_t *output)
{
    tt_bool_t changed = TT_FALSE;

    if (argc != 0) {
        tt_buf_put_cstr(output, "usage: commit");
        return TT_CLIOC_OUT;
    }

    if (__need_reboot(sh->root, &changed)) {
        tt_buf_put_cstr(output, "committing current configuration MAY reboot");
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);
        tt_buf_put_cstr(output, "are you sure to commit? [y/N] ");

        tt_cfgsh_read_line(sh, __commit_on_read);

        return TT_CLIOC_OUT;
    }

    if (changed) {
        tt_cfgnode_commit(sh->root);
        tt_buf_put_cstr(output, "committing configuration...done");
        return TT_CLIOC_OUT;
    }

    return TT_CLIOC_NOOUT;
}

tt_u32_t __commit_on_read(IN struct tt_cli_s *cli,
                          IN const tt_char_t *content,
                          IN tt_buf_t *output)
{
    if ((tt_strlen(content) == 1) &&
        ((content[0] == 'y') || (content[0] == 'Y'))) {
        tt_cfgnode_commit(TT_CONTAINER(cli, tt_cfgsh_t, cli)->root);
        tt_buf_put_cstr(output, "committing configuration...done");
        tt_buf_put_u8(output, TT_CLI_EV_ENTER);

        return TT_CLIOR_END;
    } else {
        // stop reading
        return TT_CLIOR_DONE;
    }
}

tt_bool_t __need_reboot(IN tt_cfgnode_t *cnode, OUT tt_bool_t *changed)
{
    if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
        tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
        tt_lnode_t *node;

        // check added children
        node = tt_list_head(&cgrp->new_child);
        while (node != NULL) {
            tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

            node = node->next;

            if (__need_reboot(child, changed)) {
                return TT_TRUE;
            }

            // has new node
            *changed = TT_TRUE;
        }

        // check children
        node = tt_list_head(&cgrp->child);
        while (node != NULL) {
            tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

            node = node->next;

            if (__need_reboot(child, changed)) {
                return TT_TRUE;
            }
        }

        return TT_FALSE;
    } else if (cnode->modified || cnode->removing) {
        *changed = TT_TRUE;
        return cnode->need_reboot;
    } else {
        return TT_FALSE;
    }
}
