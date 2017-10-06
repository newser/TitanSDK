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

/**
@file tt_date_def.h
@brief date definitions

this file specifies date definitions
*/

#ifndef __TT_DATE_DEF__
#define __TT_DATE_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_GREGORIAN_BEGIN 1583

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_UTC_MINUS_12_00,
    TT_UTC_MINUS_11_00,
    TT_UTC_MINUS_10_00,
    TT_UTC_MINUS_09_30,
    TT_UTC_MINUS_09_00,
    TT_UTC_MINUS_08_00,
    TT_UTC_MINUS_07_00,
    TT_UTC_MINUS_06_00,
    TT_UTC_MINUS_05_00,
    TT_UTC_MINUS_04_00,
    TT_UTC_MINUS_03_30,
    TT_UTC_MINUS_03_00,
    TT_UTC_MINUS_02_00,
    TT_UTC_MINUS_01_00,
    TT_UTC_00_00,
    TT_UTC_01_00,
    TT_UTC_02_00,
    TT_UTC_03_00,
    TT_UTC_03_30,
    TT_UTC_04_00,
    TT_UTC_04_30,
    TT_UTC_05_00,
    TT_UTC_05_30,
    TT_UTC_05_45,
    TT_UTC_06_00,
    TT_UTC_06_30,
    TT_UTC_07_00,
    TT_UTC_08_00,
    TT_UTC_08_30,
    TT_UTC_08_45,
    TT_UTC_09_00,
    TT_UTC_09_30,
    TT_UTC_10_00,
    TT_UTC_10_30,
    TT_UTC_11_00,
    TT_UTC_12_00,
    TT_UTC_12_45,
    TT_UTC_13_00,
    TT_UTC_14_00,

    TT_TMZONE_NUM,
} tt_tmzone_t;
#define TT_TMZONE_VALID(z) ((z) <= TT_TMZONE_NUM)

typedef enum {
    TT_JANUARY,
    TT_FEBRUARY,
    TT_MARCH,
    TT_APRIL,
    TT_MAY,
    TT_JUNE,
    TT_JULY,
    TT_AUGUST,
    TT_SEPTEMBER,
    TT_OCTOBER,
    TT_NOVEMBER,
    TT_DECEMBER,

    TT_MONTH_NUM,
} tt_month_t;
#define TT_MONTH_VALID(m) ((m) < TT_MONTH_NUM)

typedef enum {
    TT_SUNDAY,
    TT_MONDAY,
    TT_TUESDAY,
    TT_WEDNESDAY,
    TT_THURSDAY,
    TT_FRIDAY,
    TT_SATURDAY,

    TT_WEEKDAY_NUM,
} tt_weekday_t;
#define TT_WEEKDAY_VALID(m) ((m) < TT_WEEKDAY_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export const tt_char_t *tt_tmzone_name[TT_TMZONE_NUM];

tt_export const tt_char_t *tt_month_name[TT_MONTH_NUM];

tt_export const tt_char_t *tt_weekday_name[TT_WEEKDAY_NUM];

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_s64_t tt_tmzone2offsec(IN tt_tmzone_t tmzone);

tt_export tt_tmzone_t tt_offsec2tmzone(IN tt_s64_t offset);

#endif /* __TT_DATE_DEF__ */
