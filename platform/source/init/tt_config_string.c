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

static void __cfgstr_on_destroy(IN tt_cfgobj_t *co);

static tt_result_t __cfgstr_read(IN tt_cfgobj_t *co,
                                 IN const tt_char_t *line_sep,
                                 OUT tt_buf_t *output);

static tt_result_t __cfgstr_write(IN tt_cfgobj_t *co,
                                  IN tt_u8_t *val,
                                  IN tt_u32_t val_len);

static tt_cfgobj_itf_t __cfgstr_itf = {
    __cfgstr_on_destroy, __cfgstr_read, __cfgstr_write, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgobj_t *tt_cfgstr_create(IN const tt_char_t *name,
                              IN tt_string_t *p_str,
                              IN OPT tt_cfgobj_attr_t *attr,
                              IN OPT tt_cfgstr_cb_t *cb)
{
    tt_cfgobj_attr_t __attr;
    tt_cfgobj_t *co;
    tt_cfgstr_t *cs;

    if (attr == NULL) {
        tt_cfgobj_attr_default(&__attr);
        attr = &__attr;
    }
    attr->can_exec = TT_FALSE;

    co = tt_cfgobj_create(sizeof(tt_cfgstr_t),
                          TT_CFGOBJ_STRING,
                          name,
                          &__cfgstr_itf,
                          p_str,
                          attr);
    if (co == NULL) {
        return NULL;
    }

    cs = TT_CFGOBJ_CAST(co, tt_cfgstr_t);

    if (cb != NULL) {
        tt_memcpy(&cs->cb, cb, sizeof(tt_cfgstr_cb_t));
    } else {
        tt_memset(&cs->cb, 0, sizeof(tt_cfgstr_cb_t));
    }
    tt_string_init(&cs->str, NULL);

    return co;
}

void __cfgstr_on_destroy(IN tt_cfgobj_t *co)
{
    tt_cfgstr_t *cs = TT_CFGOBJ_CAST(co, tt_cfgstr_t);

    tt_string_destroy(&cs->str);
}

tt_result_t __cfgstr_read(IN tt_cfgobj_t *co,
                          IN const tt_char_t *line_sep,
                          OUT tt_buf_t *output)
{
    tt_cfgstr_t *cs = TT_CFGOBJ_CAST(co, tt_cfgstr_t);

    TT_DO(tt_buf_put_u8(output, '"'));
    TT_DO(tt_buf_put_cstr(output, tt_string_cstr((tt_string_t *)co->opaque)));
    TT_DO(tt_buf_put_u8(output, '"'));

    return TT_SUCCESS;
}

tt_result_t __cfgstr_write(IN tt_cfgobj_t *co,
                           IN tt_u8_t *val,
                           IN tt_u32_t val_len)
{
    tt_cfgstr_t *cs = TT_CFGOBJ_CAST(co, tt_cfgstr_t);

    tt_string_clear(&cs->str);
    TT_DO(tt_string_append_sub(&cs->str, (tt_char_t *)val, 0, val_len));
    if (tt_string_cmp(&cs->str, tt_string_cstr((tt_string_t *)co->opaque)) ==
        0) {
        return TT_SUCCESS;
    }

    if (cs->cb.on_set != NULL) {
        cs->cb.on_set(co, &cs->str);
    }

    return TT_SUCCESS;
}
