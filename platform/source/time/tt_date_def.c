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

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

const tt_char_t *tt_tmzone_name[TT_TMZONE_NUM] = {
    "UTC-12:00", "UTC-11:00", "UTC-10:00", "UTC-09:30", "UTC-09:00",
    "UTC-08:00", "UTC-07:00", "UTC-06:00", "UTC-05:00", "UTC-04:00",
    "UTC-03:30", "UTC-03:00", "UTC-02:00", "UTC-01:00", "UTC+00:00",
    "UTC+01:00", "UTC+02:00", "UTC+03:00", "UTC+03:30", "UTC+04:00",
    "UTC+04:30", "UTC+05:00", "UTC+05:30", "UTC+05:45", "UTC+06:00",
    "UTC+06:30", "UTC+07:00", "UTC+08:00", "UTC+08:30", "UTC+08:45",
    "UTC+09:00", "UTC+09:30", "UTC+10:00", "UTC+10:30", "UTC+11:00",
    "UTC+12:00", "UTC+12:45", "UTC+13:00", "UTC+14:00",
};

const tt_char_t *tt_tmzone_name_rfc1123[TT_TMZONE_NUM] = {
    "-1200", "-1100", "-1000", "-0930", "-0900", "-0800", "-0700", "-0600",
    "-0500", "-0400", "-0330", "-0300", "-0200", "-0100", "GMT",   "+0100",
    "+0200", "+0300", "+0330", "+0400", "+0430", "+0500", "+0530", "+0545",
    "+0600", "+0630", "+0700", "+0800", "+0830", "+0845", "+0900", "+0930",
    "+1000", "+1030", "+1100", "+1200", "+1245", "+1300", "+1400",
};

const tt_char_t *tt_tmzone_name_iso8601[TT_TMZONE_NUM] = {
    "-12:00", "-11:00", "-10:00", "-09:30", "-09:00", "-08:00", "-07:00",
    "-06:00", "-05:00", "-04:00", "-03:30", "-03:00", "-02:00", "-01:00",
    "Z",      "+01:00", "+02:00", "+03:00", "+03:30", "+04:00", "+04:30",
    "+05:00", "+05:30", "+05:45", "+06:00", "+06:30", "+07:00", "+08:00",
    "+08:30", "+08:45", "+09:00", "+09:30", "+10:00", "+10:30", "+11:00",
    "+12:00", "+12:45", "+13:00", "+14:00",
};

const tt_char_t *tt_month_name[TT_MONTH_NUM] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

const tt_char_t *tt_month_name_abbr[TT_MONTH_NUM] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "June",
    "July",
    "Aug",
    "Sept",
    "Oct",
    "Nov",
    "Dec",
};

const tt_char_t *tt_weekday_name[TT_WEEKDAY_NUM] = {"Sunday",
                                                    "Monday",
                                                    "Tuesday",
                                                    "Wednesday",
                                                    "Thursday",
                                                    "Friday",
                                                    "Saturday"};

const tt_char_t *tt_weekday_name_abbr[TT_WEEKDAY_NUM] =
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

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
