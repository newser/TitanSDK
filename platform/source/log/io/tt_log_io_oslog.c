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

#include <log/io/tt_log_io_oslog.h>

#include <log/io/tt_log_io.h>

#include <os/log.h>

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

static void __lio_oslog_destroy(IN tt_logio_t *lio);

static void __lio_oslog_output(IN tt_logio_t *lio,
                               IN const tt_char_t *data,
                               IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_winev_itf = {
    TT_LOGIO_OSLOG,

    NULL,
    __lio_oslog_destroy,
    __lio_oslog_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_oslog_create(IN const tt_char_t *subsystem,
                                  IN const tt_char_t *category,
                                  IN tt_oslog_type_t type)
{
    tt_logio_t *lio;
    tt_logio_oslog_t *lio_oslog;

    lio = tt_logio_create(sizeof(tt_logio_oslog_t), &tt_s_logio_winev_itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_oslog = TT_LOGIO_CAST(lio, tt_logio_oslog_t);

#ifdef TT_HAVE_OSLOG
    lio_oslog->l = os_log_create(subsystem, category);
#endif

    lio_oslog->type = type;

    return lio;
}

void __lio_oslog_destroy(IN tt_logio_t *lio)
{
    tt_logio_oslog_t *lio_oslog = TT_LOGIO_CAST(lio, tt_logio_oslog_t);

#ifdef TT_HAVE_OSLOG
    os_release(lio_oslog->l);
#endif
}

void __lio_oslog_output(IN tt_logio_t *lio,
                        IN const tt_char_t *data,
                        IN tt_u32_t data_len)
{
    tt_logio_oslog_t *lio_oslog = TT_LOGIO_CAST(lio, tt_logio_oslog_t);

#ifdef TT_HAVE_OSLOG
    os_log_with_type(lio_oslog->l, lio_oslog->type, "%s", data);
#endif
}
