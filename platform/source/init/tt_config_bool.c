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

#include <init/tt_config_bool.h>

#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __cfgbool_on_destroy(IN tt_cfgnode_t *cnode,
                                 IN tt_bool_t committed);

static tt_cfgnode_itf_t __cbool_itf_g = {__cfgbool_on_destroy,
                                         NULL,
                                         NULL,
                                         tt_cfgbool_ls,
                                         tt_cfgbool_get,
                                         NULL,
                                         tt_cfgbool_check,
                                         NULL};

static tt_cfgnode_itf_t __cbool_itf_gs = {
    __cfgbool_on_destroy,
    NULL,
    NULL,
    tt_cfgbool_ls,
    tt_cfgbool_get,
    tt_cfgbool_set,
    tt_cfgbool_check,
    tt_cfgbool_commit,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgnode_t *tt_cfgbool_create(IN const tt_char_t *name,
                                IN OPT tt_cfgnode_itf_t *itf,
                                IN OPT void *opaque,
                                IN tt_bool_t *val_ptr,
                                IN OPT tt_cfgbool_cb_t *cb,
                                IN OPT tt_cfgbool_attr_t *attr)
{
    tt_cfgbool_attr_t cbool_attr;
    tt_cfgnode_t *cnode;
    tt_cfgbool_t *cbool;

    if (attr == NULL) {
        tt_cfgbool_attr_default(&cbool_attr);
        attr = &cbool_attr;
    }

    if (itf == NULL) {
        if (attr->mode == TT_CFGVAL_MODE_GS) {
            itf = &__cbool_itf_gs;
        } else {
            itf = &__cbool_itf_g;
        }
    }

    cnode = tt_cfgnode_create(sizeof(tt_cfgbool_t),
                              TT_CFGNODE_TYPE_BOOL,
                              name,
                              itf,
                              opaque,
                              &attr->cnode_attr);
    if (cnode == NULL) {
        return NULL;
    }

    cbool = TT_CFGNODE_CAST(cnode, tt_cfgbool_t);

    cbool->val_ptr = val_ptr;
    cbool->new_val = 0;

    cbool->cb = cb;

    return cnode;
}

void tt_cfgbool_attr_default(IN tt_cfgbool_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_cfgnode_attr_default(&attr->cnode_attr);

    attr->mode = TT_CFGVAL_MODE_G;
}

tt_result_t tt_cfgbool_ls(IN tt_cfgnode_t *cnode,
                          IN const tt_char_t *seperator,
                          OUT tt_buf_t *output)
{
    tt_u32_t n;

    // 1st line
    TT_DO(tt_buf_put_cstr(output, "PERM    TYPE    NAME"));

    n = (tt_u32_t)tt_strlen(cnode->name) + 4;
    n = TT_MAX(n, 8);
    TT_DO(tt_buf_put_rep(output, ' ', n - 4));

    TT_DO(tt_buf_put_cstr(output, "DESCRIPTION"));
    TT_DO(tt_buf_put_cstr(output, TT_COND(seperator != NULL, seperator, "\n")));

    return tt_cfgnode_describe(cnode, n - 4, output);
}

tt_result_t tt_cfgbool_get(IN tt_cfgnode_t *cnode, OUT tt_buf_t *output)
{
    tt_cfgbool_t *cbool = TT_CFGNODE_CAST(cnode, tt_cfgbool_t);

    if (cnode->modified) {
        TT_DO(
            tt_buf_put_cstr(output, TT_COND(*cbool->val_ptr, "true", "false")));
        TT_DO(tt_buf_put_cstr(output, " --> "));
        TT_DO(
            tt_buf_put_cstr(output, TT_COND(cbool->new_val, "true", "false")));
        return TT_SUCCESS;
    } else {
        return tt_buf_put_cstr(output,
                               TT_COND(*cbool->val_ptr, "true", "false"));
    }
}

tt_result_t tt_cfgbool_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    tt_cfgbool_t *cbool = TT_CFGNODE_CAST(cnode, tt_cfgbool_t);

    if (tt_blob_cmpcstr(val, "true")) {
        cbool->new_val = TT_TRUE;
    } else if (tt_blob_cmpcstr(val, "false")) {
        cbool->new_val = TT_FALSE;
    } else {
        return TT_BAD_PARAM;
    }

    if (*cbool->val_ptr == cbool->new_val) {
        cnode->modified = TT_FALSE;
    } else {
        cnode->modified = TT_TRUE;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cfgbool_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    if (tt_blob_cmpcstr(val, "true")) {
        return TT_SUCCESS;
    } else if (tt_blob_cmpcstr(val, "false")) {
        return TT_SUCCESS;
    } else {
        return TT_BAD_PARAM;
    }
}

tt_result_t tt_cfgbool_commit(IN tt_cfgnode_t *cnode)
{
    tt_cfgbool_t *cbool = TT_CFGNODE_CAST(cnode, tt_cfgbool_t);

    if (!cnode->modified) {
        return TT_SUCCESS;
    }

    TT_ASSERT(cbool->new_val != *cbool->val_ptr);
    *cbool->val_ptr = cbool->new_val;
    cnode->modified = TT_FALSE;

    if ((cbool->cb != NULL) && (cbool->cb->on_set != NULL) &&
        cbool->cb->on_set(cnode, cbool->new_val)) {
        return TT_END;
    }

    return TT_SUCCESS;
}

void __cfgbool_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed)
{
    tt_cfgbool_t *cbool = TT_CFGNODE_CAST(cnode, tt_cfgbool_t);

    if (!committed) {
        return;
    }

    if ((cbool->cb != NULL) && (cbool->cb->on_destroy != NULL)) {
        cbool->cb->on_destroy(cnode, TT_TRUE);
    }
}
