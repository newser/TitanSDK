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

#include <time/tt_date_def.h>

#include <misc/tt_assert.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __BLOB(s)                                                              \
    {                                                                          \
        (tt_u8_t *)s, sizeof(s) - 1,                                           \
    }

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

tt_blob_t tt_g_tmzone_name[TT_TMZONE_NUM] = {
    __BLOB("UTC-12:00"), __BLOB("UTC-11:00"), __BLOB("UTC-10:00"),
    __BLOB("UTC-09:30"), __BLOB("UTC-09:00"), __BLOB("UTC-08:00"),
    __BLOB("UTC-07:00"), __BLOB("UTC-06:00"), __BLOB("UTC-05:00"),
    __BLOB("UTC-04:00"), __BLOB("UTC-03:30"), __BLOB("UTC-03:00"),
    __BLOB("UTC-02:00"), __BLOB("UTC-01:00"), __BLOB("UTC+00:00"),
    __BLOB("UTC+01:00"), __BLOB("UTC+02:00"), __BLOB("UTC+03:00"),
    __BLOB("UTC+03:30"), __BLOB("UTC+04:00"), __BLOB("UTC+04:30"),
    __BLOB("UTC+05:00"), __BLOB("UTC+05:30"), __BLOB("UTC+05:45"),
    __BLOB("UTC+06:00"), __BLOB("UTC+06:30"), __BLOB("UTC+07:00"),
    __BLOB("UTC+08:00"), __BLOB("UTC+08:30"), __BLOB("UTC+08:45"),
    __BLOB("UTC+09:00"), __BLOB("UTC+09:30"), __BLOB("UTC+10:00"),
    __BLOB("UTC+10:30"), __BLOB("UTC+11:00"), __BLOB("UTC+12:00"),
    __BLOB("UTC+12:45"), __BLOB("UTC+13:00"), __BLOB("UTC+14:00"),
};

tt_blob_t tt_g_tmzone_name_rfc1123[TT_TMZONE_NUM] = {
    __BLOB("-1200"), __BLOB("-1100"), __BLOB("-1000"), __BLOB("-0930"),
    __BLOB("-0900"), __BLOB("-0800"), __BLOB("-0700"), __BLOB("-0600"),
    __BLOB("-0500"), __BLOB("-0400"), __BLOB("-0330"), __BLOB("-0300"),
    __BLOB("-0200"), __BLOB("-0100"), __BLOB("GMT"),   __BLOB("+0100"),
    __BLOB("+0200"), __BLOB("+0300"), __BLOB("+0330"), __BLOB("+0400"),
    __BLOB("+0430"), __BLOB("+0500"), __BLOB("+0530"), __BLOB("+0545"),
    __BLOB("+0600"), __BLOB("+0630"), __BLOB("+0700"), __BLOB("+0800"),
    __BLOB("+0830"), __BLOB("+0845"), __BLOB("+0900"), __BLOB("+0930"),
    __BLOB("+1000"), __BLOB("+1030"), __BLOB("+1100"), __BLOB("+1200"),
    __BLOB("+1245"), __BLOB("+1300"), __BLOB("+1400"),
};

tt_blob_t tt_g_tmzone_name_iso8601[TT_TMZONE_NUM] = {
    __BLOB("-12:00"), __BLOB("-11:00"), __BLOB("-10:00"), __BLOB("-09:30"),
    __BLOB("-09:00"), __BLOB("-08:00"), __BLOB("-07:00"), __BLOB("-06:00"),
    __BLOB("-05:00"), __BLOB("-04:00"), __BLOB("-03:30"), __BLOB("-03:00"),
    __BLOB("-02:00"), __BLOB("-01:00"), __BLOB("Z"),      __BLOB("+01:00"),
    __BLOB("+02:00"), __BLOB("+03:00"), __BLOB("+03:30"), __BLOB("+04:00"),
    __BLOB("+04:30"), __BLOB("+05:00"), __BLOB("+05:30"), __BLOB("+05:45"),
    __BLOB("+06:00"), __BLOB("+06:30"), __BLOB("+07:00"), __BLOB("+08:00"),
    __BLOB("+08:30"), __BLOB("+08:45"), __BLOB("+09:00"), __BLOB("+09:30"),
    __BLOB("+10:00"), __BLOB("+10:30"), __BLOB("+11:00"), __BLOB("+12:00"),
    __BLOB("+12:45"), __BLOB("+13:00"), __BLOB("+14:00"),
};

tt_blob_t tt_g_month_name[TT_MONTH_NUM] = {
    __BLOB("January"),
    __BLOB("February"),
    __BLOB("March"),
    __BLOB("April"),
    __BLOB("May"),
    __BLOB("June"),
    __BLOB("July"),
    __BLOB("August"),
    __BLOB("September"),
    __BLOB("October"),
    __BLOB("November"),
    __BLOB("December"),
};

tt_blob_t tt_g_month_name_abbr[TT_MONTH_NUM] = {
    __BLOB("Jan"),
    __BLOB("Feb"),
    __BLOB("Mar"),
    __BLOB("Apr"),
    __BLOB("May"),
    __BLOB("June"),
    __BLOB("July"),
    __BLOB("Aug"),
    __BLOB("Sept"),
    __BLOB("Oct"),
    __BLOB("Nov"),
    __BLOB("Dec"),
};

tt_blob_t tt_g_weekday_name[TT_WEEKDAY_NUM] = {__BLOB("Sunday"),
                                               __BLOB("Monday"),
                                               __BLOB("Tuesday"),
                                               __BLOB("Wednesday"),
                                               __BLOB("Thursday"),
                                               __BLOB("Friday"),
                                               __BLOB("Saturday")};

tt_blob_t tt_g_weekday_name_abbr[TT_WEEKDAY_NUM] = {__BLOB("Sun"),
                                                    __BLOB("Mon"),
                                                    __BLOB("Tue"),
                                                    __BLOB("Wed"),
                                                    __BLOB("Thu"),
                                                    __BLOB("Fri"),
                                                    __BLOB("Sat")};

#define SEC(h, m) (((h)*3600) + ((m)*60))
static tt_s32_t __tmzone_offset[TT_TMZONE_NUM] =
    {-SEC(12, 0), -SEC(11, 0), -SEC(10, 0), -SEC(9, 30), -SEC(9, 0),
     -SEC(8, 0),  -SEC(7, 0),  -SEC(6, 0),  -SEC(5, 0),  -SEC(4, 0),
     -SEC(3, 30), -SEC(3, 0),  -SEC(2, 0),  -SEC(1, 0),  SEC(0, 0),
     SEC(1, 0),   SEC(2, 0),   SEC(3, 0),   SEC(3, 30),  SEC(4, 0),
     SEC(4, 30),  SEC(5, 0),   SEC(5, 30),  SEC(5, 45),  SEC(6, 0),
     SEC(6, 30),  SEC(7, 30),  SEC(8, 0),   SEC(8, 30),  SEC(8, 45),
     SEC(9, 0),   SEC(9, 30),  SEC(10, 0),  SEC(10, 30), SEC(11, 0),
     SEC(12, 0),  SEC(12, 45), SEC(13, 0),  SEC(14, 0)};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_s32_t tt_tmzone2offsec(IN tt_tmzone_t tmzone)
{
    TT_ASSERT(TT_TMZONE_VALID(tmzone));
    return __tmzone_offset[tmzone];
}

tt_tmzone_t tt_offsec2tmzone(IN tt_s32_t offset)
{
    tt_tmzone_t z = TT_UTC_MINUS_12_00;
    while ((z < TT_UTC_14_00) && (offset > __tmzone_offset[z])) {
        ++z;
    }
    return z;
}
