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

#include <init/tt_config_s32.h>

#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_S32_LEN 11

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __cfgs32_read(IN tt_cfgobj_t *co,
                                 IN const tt_char_t *line_sep,
                                 OUT tt_buf_t *output);

static tt_result_t __cfgs32_write(IN tt_cfgobj_t *co,
                                  IN tt_u8_t *val,
                                  IN tt_u32_t val_len);

static tt_cfgobj_itf_t __cfgs32_itf = {
    NULL, __cfgs32_read, __cfgs32_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgobj_t *tt_cfgs32_create(IN const tt_char_t *name,
                              IN tt_s32_t *p_s32,
                              IN OPT tt_cfgobj_attr_t *attr,
                              IN OPT tt_cfgs32_cb_t *cb)
{
    tt_cfgobj_t *co;
    tt_cfgs32_t *cs32;

    co = tt_cfgobj_create(sizeof(tt_cfgs32_t),
                          TT_CFGOBJ_S32,
                          name,
                          &__cfgs32_itf,
                          p_s32,
                          attr);
    if (co == NULL) {
        return NULL;
    }

    cs32 = TT_CFGOBJ_CAST(co, tt_cfgs32_t);

    if (cb != NULL) {
        tt_memcpy(&cs32->cb, cb, sizeof(tt_cfgs32_cb_t));
    } else {
        tt_memset(&cs32->cb, 0, sizeof(tt_cfgs32_cb_t));
    }

    return co;
}

tt_result_t __cfgs32_read(IN tt_cfgobj_t *co,
                          IN const tt_char_t *line_sep,
                          OUT tt_buf_t *output)
{
    tt_cfgs32_t *cs32 = TT_CFGOBJ_CAST(co, tt_cfgs32_t);
    tt_char_t buf[32] = {0};

    tt_snprintf(buf, sizeof(buf) - 1, "%d", *(tt_s32_t *)co->opaque);
    return tt_buf_put_cstr(output, buf);
}

tt_result_t __cfgs32_write(IN tt_cfgobj_t *co,
                           IN tt_u8_t *val,
                           IN tt_u32_t val_len)
{
    tt_cfgs32_t *cs32 = TT_CFGOBJ_CAST(co, tt_cfgs32_t);
    tt_u8_t buf[__MAX_S32_LEN + 1] = {0};
    tt_s32_t s32_val;

    if ((val_len == 0) || (val_len > __MAX_S32_LEN)) {
        return TT_BAD_PARAM;
    }

    tt_memcpy(buf, val, val_len);
    if (!TT_OK(tt_strtos32((const char *)buf, NULL, 0, &s32_val))) {
        return TT_BAD_PARAM;
    }

    if (cs32->cb.on_set != NULL) {
        cs32->cb.on_set(co, s32_val);
    }

    return TT_SUCCESS;
}
