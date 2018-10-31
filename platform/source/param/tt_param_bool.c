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

#include <param/tt_param_bool.h>

#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __PARBOOL_TRUE "true"
#define __PARBOOL_TRUE_LEN (sizeof(__PARBOOL_TRUE) - 1)

#define __PARBOOL_FALSE "false"
#define __PARBOOL_FALSE_LEN (sizeof(__PARBOOL_FALSE) - 1)

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __bool_read(IN tt_param_t *p, OUT tt_buf_t *output);

static tt_result_t __bool_write(IN tt_param_t *p,
                                IN tt_u8_t *val,
                                IN tt_u32_t val_len);

static tt_param_itf_t __bool_itf = {
    NULL, __bool_read, __bool_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_param_t *tt_param_bool_create(IN const tt_char_t *name,
                                 IN tt_bool_t *p_val,
                                 IN OPT tt_param_attr_t *attr,
                                 IN OPT tt_param_bool_cb_t *cb)
{
    tt_param_t *p;
    tt_param_bool_t *pb;

    p = tt_param_create(sizeof(tt_param_bool_t),
                        TT_PARAM_BOOL,
                        name,
                        &__bool_itf,
                        p_val,
                        attr);
    if (p == NULL) {
        return NULL;
    }

    pb = TT_PARAM_CAST(p, tt_param_bool_t);

    tt_memcpy(&pb->cb, cb, sizeof(tt_param_bool_cb_t));

    return p;
}

tt_result_t __bool_read(IN tt_param_t *p, OUT tt_buf_t *output)
{
    tt_param_bool_t *pb = TT_PARAM_CAST(p, tt_param_bool_t);

    return tt_buf_put_cstr(output,
                           TT_COND(*(tt_bool_t *)p->opaque, "true", "false"));
}

tt_result_t __bool_write(IN tt_param_t *p, IN tt_u8_t *val, IN tt_u32_t val_len)
{
    tt_param_bool_t *pb = TT_PARAM_CAST(p, tt_param_bool_t);
    tt_bool_t bool_val;

    if ((val_len == __PARBOOL_TRUE_LEN) &&
        (tt_strncmp((tt_char_t *)val, __PARBOOL_TRUE, __PARBOOL_TRUE_LEN) ==
         0)) {
        bool_val = TT_TRUE;
    } else if ((val_len == __PARBOOL_FALSE_LEN) &&
               (tt_strncmp((tt_char_t *)val,
                           __PARBOOL_FALSE,
                           __PARBOOL_FALSE_LEN) == 0)) {
        bool_val = TT_FALSE;
    } else {
        return TT_E_BADARG;
    }

    if (pb->cb.on_set != NULL) {
        pb->cb.on_set(p, bool_val);
    }

    return TT_SUCCESS;
}
