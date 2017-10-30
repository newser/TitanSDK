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

#include <init/tt_config_bool.h>

#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __CFG_TRUE "true"
#define __CFG_TRUE_LEN (sizeof(__CFG_TRUE) - 1)

#define __CFG_FALSE "false"
#define __CFG_FALSE_LEN (sizeof(__CFG_FALSE) - 1)

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __cfgbool_read(IN tt_cfgobj_t *co,
                                  IN const tt_char_t *line_sep,
                                  OUT tt_buf_t *output);

static tt_result_t __cfgbool_write(IN tt_cfgobj_t *co,
                                   IN tt_u8_t *val,
                                   IN tt_u32_t val_len);

static tt_cfgobj_itf_t __cfgbool_itf = {
    NULL, __cfgbool_read, __cfgbool_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgobj_t *tt_cfgbool_create(IN const tt_char_t *name,
                               IN tt_bool_t *p_bool,
                               IN OPT tt_cfgobj_attr_t *attr,
                               IN OPT tt_cfgbool_cb_t *cb)
{
    tt_cfgobj_t *co;
    tt_cfgbool_t *cbool;

    co = tt_cfgobj_create(sizeof(tt_cfgbool_t),
                          TT_CFGOBJ_BOOL,
                          name,
                          &__cfgbool_itf,
                          p_bool,
                          attr);
    if (co == NULL) {
        return NULL;
    }

    cbool = TT_CFGOBJ_CAST(co, tt_cfgbool_t);

    tt_memcpy(&cbool->cb, cb, sizeof(tt_cfgbool_cb_t));

    return co;
}

tt_result_t __cfgbool_read(IN tt_cfgobj_t *co,
                           IN const tt_char_t *line_sep,
                           OUT tt_buf_t *output)
{
    tt_cfgbool_t *cbool = TT_CFGOBJ_CAST(co, tt_cfgbool_t);

    return tt_buf_put_cstr(output,
                           TT_COND(*(tt_bool_t *)co->opaque, "true", "false"));
}

tt_result_t __cfgbool_write(IN tt_cfgobj_t *co,
                            IN tt_u8_t *val,
                            IN tt_u32_t val_len)
{
    tt_cfgbool_t *cbool = TT_CFGOBJ_CAST(co, tt_cfgbool_t);
    tt_bool_t bool_val;

    if ((val_len == __CFG_TRUE_LEN) &&
        (tt_strncmp((tt_char_t *)val, __CFG_TRUE, __CFG_TRUE_LEN) == 0)) {
        bool_val = TT_TRUE;
    } else if ((val_len == __CFG_FALSE_LEN) &&
               (tt_strncmp((tt_char_t *)val, __CFG_FALSE, __CFG_FALSE_LEN) ==
                0)) {
        bool_val = TT_FALSE;
    } else {
        return TT_E_BADARG;
    }

    if (cbool->cb.on_set != NULL) {
        cbool->cb.on_set(co, bool_val);
    }

    return TT_SUCCESS;
}
