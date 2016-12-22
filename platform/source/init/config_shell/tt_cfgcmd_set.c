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

#include <init/config_shell/tt_cfgcmd_set.h>

#include <algorithm/tt_buffer_format.h>
#include <init/config_shell/tt_config_shell.h>
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

static const tt_char_t __set_info[] = "set value";

static const tt_char_t __set_usage[] = "testing set";

static tt_u32_t __set_run(IN tt_cfgsh_t *sh,
                          IN tt_u32_t argc,
                          IN tt_char_t *arv[],
                          OUT tt_buf_t *output);

tt_cfgcmd_t tt_g_cfgcmd_set = {
    TT_CFGCMD_NAME_SET, __set_info, __set_usage, __set_run,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t __set_run(IN tt_cfgsh_t *sh,
                   IN tt_u32_t argc,
                   IN tt_char_t *argv[],
                   OUT tt_buf_t *output)
{
    tt_blob_t path_blob, val_blob;
    tt_cfgnode_t *cnode;
    tt_u32_t rp, wp;
    tt_result_t result;

    if (argc < 2) {
        tt_buf_putf(output, "usage: set name value");
        return TT_CLIOC_OUT;
    }

    path_blob.addr = (tt_u8_t *)argv[0];
    path_blob.len = (tt_u32_t)tt_strlen(argv[0]);
    cnode = tt_cfgpath_p2n(sh->root, sh->current, &path_blob);
    if (cnode == NULL) {
        tt_buf_putf(output, "can not find: %s", argv[0]);
        return TT_CLIOC_OUT;
    }

    val_blob.addr = (tt_u8_t *)argv[1];
    val_blob.len = (tt_u32_t)tt_strlen(argv[1]);

    if (!TT_OK(tt_cfgnode_check(cnode, &val_blob))) {
        tt_buf_putf(output, "bad value: %s", argv[1]);
        return TT_CLIOC_OUT;
    }

    tt_buf_backup_rwp(output, &rp, &wp);
    result = tt_cfgnode_set(cnode, &val_blob);
    if (TT_OK(result)) {
        return TT_CLIOC_NOOUT;
    } else {
        tt_buf_restore_rwp(output, &rp, &wp);
        if (result == TT_BAD_PARAM) {
            tt_buf_putf(output, "not supported operation");
        } else {
            tt_buf_putf(output, "internal error");
        }
        return TT_CLIOC_OUT;
    }
}
