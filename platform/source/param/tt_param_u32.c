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

#include <param/tt_param_u32.h>

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

static tt_result_t __u32_read(IN tt_param_t *p, OUT tt_buf_t *output);

static tt_result_t __u32_write(IN tt_param_t *p, IN tt_u8_t *val,
                               IN tt_u32_t val_len);

static tt_param_itf_t __u32_itf = {
    NULL,
    __u32_read,
    __u32_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_param_t *tt_param_u32_create(IN const tt_char_t *name, IN tt_u32_t *p_val,
                                IN OPT tt_param_attr_t *attr,
                                IN OPT tt_param_u32_cb_t *cb)
{
    tt_param_t *p;
    tt_param_u32_t *pu;

    p = tt_param_create(sizeof(tt_param_u32_t), TT_PARAM_U32, name, &__u32_itf,
                        p_val, attr);
    if (p == NULL) { return NULL; }

    pu = TT_PARAM_CAST(p, tt_param_u32_t);

    if (cb != NULL) {
        tt_memcpy(&pu->cb, cb, sizeof(tt_param_u32_cb_t));
    } else {
        tt_memset(&pu->cb, 0, sizeof(tt_param_u32_cb_t));
    }

    return p;
}

tt_u32_t tt_param_get_u32(IN tt_param_t *p)
{
    TT_ASSERT(p->type == TT_PARAM_U32);
    return *(tt_u32_t *)p->opaque;
}

void tt_param_set_u32(IN tt_param_t *p, IN tt_u32_t val)
{
    TT_ASSERT(p->type == TT_PARAM_U32);
    *(tt_u32_t *)p->opaque = val;
}

tt_result_t __u32_read(IN tt_param_t *p, OUT tt_buf_t *output)
{
    tt_param_u32_t *pu = TT_PARAM_CAST(p, tt_param_u32_t);
    tt_char_t buf[32] = {0};

    tt_snprintf(buf, sizeof(buf) - 1, "%u", *(tt_u32_t *)p->opaque);
    return tt_buf_put_cstr(output, buf);
}

tt_result_t __u32_write(IN tt_param_t *p, IN tt_u8_t *val, IN tt_u32_t val_len)
{
    tt_param_u32_t *pu = TT_PARAM_CAST(p, tt_param_u32_t);
    tt_u8_t buf[__MAX_U32_LEN + 1] = {0};
    tt_u32_t u32_val;

    if ((val_len == 0) || (val_len > __MAX_U32_LEN)) { return TT_E_BADARG; }

    tt_memcpy(buf, val, val_len);
    if (!TT_OK(tt_strtou32((const char *)buf, NULL, 0, &u32_val))) {
        return TT_E_BADARG;
    }

    if ((pu->cb.pre_set != NULL) && !pu->cb.pre_set(p, u32_val)) {
        return TT_E_UNSUPPORT;
    }

    *((tt_u32_t *)p->opaque) = u32_val;

    if (pu->cb.post_set != NULL) { pu->cb.post_set(p, u32_val); }

    return TT_SUCCESS;
}
