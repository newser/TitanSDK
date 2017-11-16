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

#include <log/io/tt_log_io_syslog.h>

#include <log/io/tt_log_io.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

#ifdef TT_HAVE_SYSLOG
#include <syslog.h>
#endif

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

static void __lio_syslog_output(IN tt_logio_t *lio,
                                IN const tt_char_t *data,
                                IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_std_itf = {
    TT_LOGIO_SYSLOG,

    NULL,
    NULL,
    __lio_syslog_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_syslog_create(IN OPT tt_logio_syslog_attr_t *attr)
{
    tt_logio_syslog_attr_t __attr;
    tt_logio_t *lio;
    tt_logio_syslog_t *lio_syslog;

    if (attr == NULL) {
        tt_logio_syslog_attr_default(&__attr);
        attr = &__attr;
    }

    lio = tt_logio_create(sizeof(tt_logio_syslog_t), &tt_s_logio_std_itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_syslog = TT_LOGIO_CAST(lio, tt_logio_syslog_t);

    lio_syslog->facility = attr->facility;
    lio_syslog->level = attr->level;

    return lio;
}

void tt_logio_syslog_attr_default(IN tt_logio_syslog_attr_t *attr)
{
    attr->facility = TT_SYSLOG_USER;
    attr->level = TT_SYSLOG_INFO;
}

void __lio_syslog_output(IN tt_logio_t *lio,
                         IN const tt_char_t *data,
                         IN tt_u32_t data_len)
{
    tt_logio_syslog_t *lio_syslog = TT_LOGIO_CAST(lio, tt_logio_syslog_t);

#ifdef TT_HAVE_SYSLOG
    syslog(TT_SYSLOG_PRIORITY(lio_syslog->facility, lio_syslog->level),
           "%s",
           data);
#endif
}
