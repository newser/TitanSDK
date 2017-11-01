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

#include <log/io/tt_log_io_standard.h>

#include <log/io/tt_log_io.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

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

static void __lio_std_output(IN tt_logio_t *lio,
                             IN const tt_char_t *data,
                             IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_std_itf = {
    TT_LOGIO_STANDARD,

    NULL,
    NULL,
    __lio_std_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_std_create(IN OPT tt_logio_std_attr_t *attr)
{
    tt_logio_std_attr_t __attr;
    tt_logio_t *lio;
    tt_logio_std_t *lio_std;

    if (attr == NULL) {
        tt_logio_std_attr_default(&__attr);
        attr = &__attr;
    }

    lio = tt_logio_create(sizeof(tt_logio_std_t), &tt_s_logio_std_itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_std = TT_LOGIO_CAST(lio, tt_logio_std_t);

    return lio;
}

void tt_logio_std_attr_default(IN tt_logio_std_attr_t *attr)
{
    attr->reserved = 0;
}

void __lio_std_output(IN tt_logio_t *lio,
                      IN const tt_char_t *data,
                      IN tt_u32_t data_len)
{
    tt_printf("%s", data);
}
