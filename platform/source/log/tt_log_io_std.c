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

#include <log/tt_log_io_std.h>

#include <log/tt_log_io.h>
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

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __lio_std_output(IN struct tt_logio_s *lio,
                                 IN tt_u8_t *data,
                                 IN tt_u32_t data_len);
static void __lio_std_destroy(IN struct tt_logio_s *lio);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

struct tt_logio_s *tt_logio_std_create(IN OPT tt_logio_std_attr_t *attr)
{
    tt_logio_itf_t itf;
    tt_logio_t *lio;
    tt_logio_std_t *lio_std;

    itf.output = __lio_std_output;
    itf.destroy = __lio_std_destroy;

    lio = tt_logio_create(sizeof(tt_logio_std_t), TT_LOGIO_TYPE_STD, &itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_std = TT_LOGIO_CAST(lio, tt_logio_std_t);

    if (attr != NULL) {
        tt_memcpy(&lio_std->attr, attr, sizeof(tt_logio_std_attr_t));
    } else {
        tt_logio_std_attr_default(&lio_std->attr);
    }

    return lio;
}

void tt_logio_std_attr_default(IN tt_logio_std_attr_t *attr)
{
    attr->reserved = 0;
}

// returned account does not include terminating null
tt_u32_t __lio_std_output(IN struct tt_logio_s *lio,
                          IN tt_u8_t *data,
                          IN tt_u32_t data_len)
{
    return printf("%s", data);
}

void __lio_std_destroy(IN struct tt_logio_s *lio)
{
}
