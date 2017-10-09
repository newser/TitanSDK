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
@file tt_date_format.h
@brief date format

this file specifies date format interfaces
*/

#ifndef __TT_DATE_FORMAT__
#define __TT_DATE_FORMAT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <time/tt_date.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/*
 %Y    year without centry, padded by zero (01, ...)
 %y    year without centry (1, ...)
 %C    year with centry (2001, ...)
 %B    full month (January, ...)
 %b    abbreviated month (Jan, ...)
 %D    month day, padded by zero (01, ...)
 %d    month day, padded by blank ( 1, ...)
 %H    hour, padded by zero (01, ...)
 %h    hour (1, ...)
 %A    AM/PM
 %a    am/pm
 %M    minute, padded by zero (01, ...)
 %m    minute (1, ...)
 %S    second, padded by zero (01, ...)
 %s    second (1, ...)
 %W    full week day (Monday, ...)
 %w    abbreviated week day (Mon, ...)
 %Z    time zone, RFC1123 (GMT, +0800, ...)
 %Z    time zone, ISO8601 (Z, +08:00, ...)
 */
tt_export tt_u32_t tt_date_render(IN tt_date_t *date,
                                  IN const tt_char_t *format,
                                  IN tt_char_t *buf,
                                  IN tt_u32_t len);

tt_inline tt_u32_t tt_date_render_now(IN const tt_char_t *format,
                                      IN tt_char_t *buf,
                                      IN tt_u32_t len)
{
    tt_date_t d;
    tt_date_now(&d);
    return tt_date_render_ntv(&d, format, buf, len);
}

tt_export tt_u32_t tt_date_parse(IN tt_date_t *date,
                                 IN const tt_char_t *format,
                                 IN tt_char_t *buf,
                                 IN tt_u32_t len);

#endif /* __TT_DATE_FORMAT__ */
