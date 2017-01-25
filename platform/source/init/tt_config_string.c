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

#include <init/tt_config_string.h>

#include <algorithm/tt_buffer_format.h>
#include <algorithm/tt_string_common.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __cfgstr_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed);

static tt_cfgnode_itf_t __cfgstr_itf_g = {
    __cfgstr_on_destroy,
    NULL,
    NULL,
    tt_cfgstr_ls,
    tt_cfgstr_get,
    NULL,
    tt_cfgstr_check,
    NULL,
};

static tt_cfgnode_itf_t __cfgstr_itf_gs = {
    __cfgstr_on_destroy,
    NULL,
    NULL,
    tt_cfgstr_ls,
    tt_cfgstr_get,
    tt_cfgstr_set,
    tt_cfgstr_check,
    tt_cfgstr_commit,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgnode_t *tt_cfgstr_create(IN const tt_char_t *name,
                               IN OPT tt_cfgnode_itf_t *itf,
                               IN OPT void *opaque,
                               IN tt_string_t *val_ptr,
                               IN OPT tt_cfgstr_cb_t *cb,
                               IN OPT tt_cfgstr_attr_t *attr)
{
    tt_cfgstr_attr_t cfgstr_attr;
    tt_cfgnode_t *cnode;
    tt_cfgstr_t *cfgstr;

    if (attr == NULL) {
        tt_cfgstr_attr_default(&cfgstr_attr);
        attr = &cfgstr_attr;
    }

    if (itf == NULL) {
        if (attr->mode == TT_CFGVAL_MODE_GS) {
            itf = &__cfgstr_itf_gs;
        } else {
            itf = &__cfgstr_itf_g;
        }
    }

    cnode = tt_cfgnode_create(sizeof(tt_cfgstr_t),
                              TT_CFGNODE_TYPE_STRING,
                              name,
                              itf,
                              opaque,
                              &attr->cnode_attr);
    if (cnode == NULL) {
        return NULL;
    }

    cfgstr = TT_CFGNODE_CAST(cnode, tt_cfgstr_t);

    cfgstr->val_ptr = val_ptr;
    tt_string_init(&cfgstr->new_val, NULL);

    cfgstr->cb = cb;

    return cnode;
}

void tt_cfgstr_attr_default(IN tt_cfgstr_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_cfgnode_attr_default(&attr->cnode_attr);

    attr->mode = TT_CFGVAL_MODE_G;
}

tt_result_t tt_cfgstr_ls(IN tt_cfgnode_t *cnode,
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

tt_result_t tt_cfgstr_get(IN tt_cfgnode_t *cnode, OUT tt_buf_t *output)
{
    tt_cfgstr_t *cfgstr = TT_CFGNODE_CAST(cnode, tt_cfgstr_t);

    if (cnode->modified) {
        TT_DO(tt_buf_put_u8(output, '"'));
        TT_DO(tt_buf_put_cstr(output, tt_string_cstr(cfgstr->val_ptr)));
        TT_DO(tt_buf_put_cstr(output, "\" --> \""));
        TT_DO(tt_buf_put_cstr(output, tt_string_cstr(&cfgstr->new_val)));
        TT_DO(tt_buf_put_u8(output, '"'));
    } else {
        TT_DO(tt_buf_put_u8(output, '"'));
        TT_DO(tt_buf_put_cstr(output, tt_string_cstr(cfgstr->val_ptr)));
        TT_DO(tt_buf_put_u8(output, '"'));
    }
    return TT_SUCCESS;
}

tt_result_t tt_cfgstr_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    tt_cfgstr_t *cfgstr = TT_CFGNODE_CAST(cnode, tt_cfgstr_t);
    tt_string_t *new_val = &cfgstr->new_val;

    tt_string_clear(new_val);
    TT_DO(tt_string_append_sub(new_val, (tt_char_t *)val->addr, 0, val->len));

    if (tt_string_cmp(cfgstr->val_ptr, tt_string_cstr(&cfgstr->new_val)) != 0) {
        cnode->modified = TT_TRUE;
    } else {
        cnode->modified = TT_FALSE;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cfgstr_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    tt_u32_t i;

    for (i = 0; i < val->len; ++i) {
        tt_u8_t c = val->addr[i];
        if (!((c >= 0x20) && (c <= 0x7e))) {
            return TT_BAD_PARAM;
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_cfgstr_commit(IN tt_cfgnode_t *cnode)
{
    tt_cfgstr_t *cstr = TT_CFGNODE_CAST(cnode, tt_cfgstr_t);

    if (!cnode->modified) {
        return TT_SUCCESS;
    }

    TT_ASSERT(tt_string_cmp(cstr->val_ptr, tt_string_cstr(&cstr->new_val)) !=
              0);
    tt_string_copy(cstr->val_ptr, &cstr->new_val);
    cnode->modified = TT_FALSE;

    if ((cstr->cb != NULL) && (cstr->cb->on_set != NULL) &&
        cstr->cb->on_set(cnode, &cstr->new_val)) {
        return TT_END;
    }

    return TT_SUCCESS;
}

void __cfgstr_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed)
{
    tt_cfgstr_t *cfgstr = TT_CFGNODE_CAST(cnode, tt_cfgstr_t);

    TT_ASSERT(cnode->node.lst == NULL);

    if (committed && (cfgstr->cb != NULL) && (cfgstr->cb->on_destroy != NULL)) {
        cfgstr->cb->on_destroy(cnode, TT_TRUE);
    }

    tt_string_destroy(&cfgstr->new_val);
}
