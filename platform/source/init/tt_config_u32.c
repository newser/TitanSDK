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

#include <init/tt_config_u32.h>

#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_U32_LEN 10

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __cfgu32_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed);

static tt_cfgnode_itf_t __cu32_itf_g = {__cfgu32_on_destroy,
                                        NULL,
                                        NULL,
                                        tt_cfgu32_ls,
                                        tt_cfgu32_get,
                                        NULL,
                                        tt_cfgu32_check,
                                        NULL};

static tt_cfgnode_itf_t __cu32_itf_gs = {
    __cfgu32_on_destroy,
    NULL,
    NULL,
    tt_cfgu32_ls,
    tt_cfgu32_get,
    tt_cfgu32_set,
    tt_cfgu32_check,
    tt_cfgu32_commit,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgnode_t *tt_cfgu32_create(IN const tt_char_t *name,
                               IN OPT tt_cfgnode_itf_t *itf,
                               IN OPT void *opaque,
                               IN tt_u32_t *val_ptr,
                               IN OPT tt_cfgu32_cb_t *cb,
                               IN OPT tt_cfgu32_attr_t *attr)
{
    tt_cfgu32_attr_t cu32_attr;
    tt_cfgnode_t *cnode;
    tt_cfgu32_t *cu32;

    if (attr == NULL) {
        tt_cfgu32_attr_default(&cu32_attr);
        attr = &cu32_attr;
    }

    if (itf == NULL) {
        if (attr->mode == TT_CFGVAL_MODE_GS) {
            itf = &__cu32_itf_gs;
        } else {
            itf = &__cu32_itf_g;
        }
    }

    cnode = tt_cfgnode_create(sizeof(tt_cfgu32_t),
                              TT_CFGNODE_TYPE_U32,
                              name,
                              itf,
                              opaque,
                              &attr->cnode_attr);
    if (cnode == NULL) {
        return NULL;
    }

    cu32 = TT_CFGNODE_CAST(cnode, tt_cfgu32_t);

    cu32->val_ptr = val_ptr;
    cu32->new_val = 0;

    cu32->cb = cb;

    return cnode;
}

void tt_cfgu32_attr_default(IN tt_cfgu32_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_cfgnode_attr_default(&attr->cnode_attr);

    attr->mode = TT_CFGVAL_MODE_G;
}

tt_result_t tt_cfgu32_ls(IN tt_cfgnode_t *cnode,
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

tt_result_t tt_cfgu32_get(IN tt_cfgnode_t *cnode, OUT tt_buf_t *output)
{
    tt_cfgu32_t *cu32 = TT_CFGNODE_CAST(cnode, tt_cfgu32_t);
    tt_char_t buf[32] = {0};

    if (cnode->modified) {
        tt_snprintf(buf, sizeof(buf) - 1, "%u", *cu32->val_ptr);
        TT_DO(tt_buf_put_cstr(output, buf));

        TT_DO(tt_buf_put_cstr(output, " --> "));

        tt_snprintf(buf, sizeof(buf) - 1, "%u", cu32->new_val);
        TT_DO(tt_buf_put_cstr(output, buf));

        return TT_SUCCESS;
    } else {
        tt_snprintf(buf, sizeof(buf) - 1, "%u", *cu32->val_ptr);
        return tt_buf_put_cstr(output, buf);
    }
}

tt_result_t tt_cfgu32_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    tt_cfgu32_t *cu32 = TT_CFGNODE_CAST(cnode, tt_cfgu32_t);
    tt_u8_t buf[__MAX_U32_LEN + 1] = {0};
    tt_u32_t u32_val;

    if (val->len == 0) {
        return TT_BAD_PARAM;
    } else if (val->len > __MAX_U32_LEN) {
        return TT_BAD_PARAM;
    }

    tt_memcpy(buf, val->addr, val->len);
    if (!TT_OK(tt_strtou32((const char *)buf, NULL, 0, &u32_val))) {
        return TT_BAD_PARAM;
    }
    cu32->new_val = u32_val;

    if (*cu32->val_ptr == u32_val) {
        cnode->modified = TT_FALSE;
    } else {
        cnode->modified = TT_TRUE;
    }

    return TT_SUCCESS;
}

tt_result_t tt_cfgu32_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    tt_cfgu32_t *cu32 = TT_CFGNODE_CAST(cnode, tt_cfgu32_t);
    tt_u8_t buf[__MAX_U32_LEN + 1] = {0};
    tt_u32_t u32_val;

    if (val->len == 0) {
        return TT_BAD_PARAM;
    } else if (val->len > __MAX_U32_LEN) {
        return TT_BAD_PARAM;
    }

    tt_memcpy(buf, val->addr, val->len);
    if (!TT_OK(tt_strtou32((const char *)buf, NULL, 0, &u32_val))) {
        return TT_BAD_PARAM;
    }
    return TT_SUCCESS;
}

tt_result_t tt_cfgu32_commit(IN tt_cfgnode_t *cnode)
{
    tt_cfgu32_t *cu32 = TT_CFGNODE_CAST(cnode, tt_cfgu32_t);

    if (!cnode->modified) {
        return TT_SUCCESS;
    }

    TT_ASSERT(cu32->new_val != *cu32->val_ptr);
    *cu32->val_ptr = cu32->new_val;
    cnode->modified = TT_FALSE;

    if ((cu32->cb != NULL) && (cu32->cb->on_set != NULL) &&
        cu32->cb->on_set(cnode, cu32->new_val)) {
        return TT_END;
    }

    return TT_SUCCESS;
}

void __cfgu32_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed)
{
    tt_cfgu32_t *cu32 = TT_CFGNODE_CAST(cnode, tt_cfgu32_t);

    if (!committed) {
        return;
    }

    if ((cu32->cb != NULL) && (cu32->cb->on_destroy != NULL)) {
        cu32->cb->on_destroy(cnode, TT_TRUE);
    }
}
