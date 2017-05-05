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

const tt_char_t *tt_weekday_name[TT_WEEKDAY_NUM] = {"Sunday",
                                                    "Monday",
                                                    "Tuesday",
                                                    "Wednesday",
                                                    "Thursday",
                                                    "Friday",
                                                    "Saturday"};

#define SEC(h, m) (((h)*3600) + ((m)*60))
static tt_s64_t __tmzone_offset[TT_TMZONE_NUM] =
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

tt_s64_t tt_tmzone2offsec(IN tt_tmzone_t tmzone)
{
    TT_ASSERT(TT_TMZONE_VALID(tmzone));
    return __tmzone_offset[tmzone];
}

tt_tmzone_t tt_offsec2tmzone(IN tt_s64_t offset)
{
    tt_tmzone_t z = TT_UTC_MINUS_12_00;
    while ((z < TT_UTC_14_00) && (offset > __tmzone_offset[z])) {
        ++z;
    }
    return z;
}
