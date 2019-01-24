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

#include <param/tt_param_string.h>

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

static void __str_on_destroy(IN tt_param_t *p);

static tt_result_t __str_read(IN tt_param_t *p, OUT tt_buf_t *output);

static tt_result_t __str_write(IN tt_param_t *p,
                               IN tt_u8_t *val,
                               IN tt_u32_t val_len);

static tt_param_itf_t __str_itf = {
    __str_on_destroy, __str_read, __str_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_param_t *tt_param_str_create(IN const tt_char_t *name,
                                IN tt_string_t *p_val,
                                IN OPT tt_param_attr_t *attr,
                                IN OPT tt_param_str_cb_t *cb)
{
    tt_param_t *p;
    tt_param_str_t *ps;

    p = tt_param_create(sizeof(tt_param_str_t),
                        TT_PARAM_STRING,
                        name,
                        &__str_itf,
                        p_val,
                        attr);
    if (p == NULL) {
        return NULL;
    }

    ps = TT_PARAM_CAST(p, tt_param_str_t);

    if (cb != NULL) {
        tt_memcpy(&ps->cb, cb, sizeof(tt_param_str_cb_t));
    } else {
        tt_memset(&ps->cb, 0, sizeof(tt_param_str_cb_t));
    }
    tt_string_init(&ps->str, NULL);

    return p;
}

const tt_char_t *tt_param_get_str(IN tt_param_t *p, OUT tt_string_t *val)
{
    tt_string_t *s;

    TT_ASSERT(p->type == TT_PARAM_STRING);
    s = (tt_string_t *)p->opaque;

    tt_string_clear(val);
    if (TT_OK(tt_string_copy(val, s))) {
        return tt_string_cstr(val);
    } else {
        return NULL;
    }
}

tt_result_t tt_param_set_str_n(IN tt_param_t *p,
                               IN const tt_char_t *val,
                               IN tt_u32_t len)
{
    TT_ASSERT(p->type == TT_PARAM_STRING);
    return tt_string_set_sub((tt_string_t *)p->opaque, val, 0, len);
}

void __str_on_destroy(IN tt_param_t *p)
{
    tt_param_str_t *ps = TT_PARAM_CAST(p, tt_param_str_t);

    tt_string_destroy(&ps->str);
}

tt_result_t __str_read(IN tt_param_t *p, OUT tt_buf_t *output)
{
    tt_param_str_t *ps = TT_PARAM_CAST(p, tt_param_str_t);

    // TT_DO(tt_buf_put_u8(output, '"'));
    TT_DO(tt_buf_put_cstr(output, tt_string_cstr((tt_string_t *)p->opaque)));
    // TT_DO(tt_buf_put_u8(output, '"'));

    return TT_SUCCESS;
}

tt_result_t __str_write(IN tt_param_t *p, IN tt_u8_t *val, IN tt_u32_t val_len)
{
    tt_param_str_t *ps = TT_PARAM_CAST(p, tt_param_str_t);

    tt_string_clear(&ps->str);
    TT_DO(tt_string_append_sub(&ps->str, (tt_char_t *)val, 0, val_len));
    if (tt_string_cmp(&ps->str, tt_string_cstr((tt_string_t *)p->opaque)) ==
        0) {
        return TT_SUCCESS;
    }

    if ((ps->cb.pre_set != NULL) &&
        !ps->cb.pre_set(p, (tt_char_t *)val, val_len)) {
        return TT_E_UNSUPPORT;
    }

    if (!TT_OK(tt_string_set_sub((tt_string_t *)p->opaque,
                                 (tt_char_t *)val,
                                 0,
                                 val_len))) {
        return TT_FAIL;
    }

    if (ps->cb.post_set != NULL) {
        ps->cb.post_set(p, (tt_char_t *)val, val_len);
    }

    return TT_SUCCESS;
}
