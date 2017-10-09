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

#include <time/tt_date_format.h>

#include <algorithm/tt_string_common.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__sym_render_t)(IN tt_date_t *d, OUT tt_string_t *s);

typedef struct
{
    tt_char_t c;
    __sym_render_t render;
} __sym_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __Y_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __y_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __C_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __B_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __b_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __D_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __d_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __H_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __h_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __M_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __S_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __W_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __w_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __A_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __a_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __Z_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __z_render(IN tt_date_t *d, OUT tt_string_t *s);

static __sym_t __sym[] = {
    {
        'Y', __Y_render,
    },
    {
        'y', __y_render,
    },
    {
        'C', __C_render,
    },
    {
        'B', __B_render,
    },
    {
        'b', __b_render,
    },
    {
        'D', __D_render,
    },
    {
        'd', __d_render,
    },
    {
        'H', __H_render,
    },
    {
        'h', __h_render,
    },
    {
        'M', __M_render,
    },
    {
        'S', __S_render,
    },
    {
        'W', __W_render,
    },
    {
        'w', __w_render,
    },
    {
        'A', __A_render,
    },
    {
        'a', __a_render,
    },
    {
        'Z', __Z_render,
    },
    {
        'z', __z_render,
    },
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __sym_t *__find_sym(IN tt_char_t c);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t tt_date_render(IN tt_date_t *date,
                        IN const tt_char_t *format,
                        IN tt_char_t *buf,
                        IN tt_u32_t len)
{
    tt_string_t s;
    const tt_char_t *p, *end, *prev;
    tt_u32_t n;

    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    if (len == 0) {
        return 0;
    }

    tt_string_init(&s, NULL);
    p = format;
    end = p + tt_strlen(p);
    n = 0;

    prev = p;
    while (p < end) {
        if (*p != '%') {
            p += 1;
        } else if ((p + 2) > end) {
            TT_ASSERT((p + 1) == end);
            TT_DO_G(done,
                    tt_string_append_sub(&s, prev, 0, (tt_u32_t)(end - prev)));
            break;
        } else if (*(p + 1) == '%') {
            p += 2;
        } else {
            __sym_t *sym;
            const tt_char_t *str;

            TT_DO_G(done,
                    tt_string_append_sub(&s, prev, 0, (tt_u32_t)(p - prev)));

            sym = __find_sym(*(p + 1));
            if (sym == NULL) {
                TT_ERROR("invalid symbol: %c", *(p + 1));
                goto done;
            }
            p += 2;
            prev = p;

            TT_DO_G(done, sym->render(date, &s));
        }
    }

    n = tt_string_len(&s);
    if (n >= len) {
        n = len - 1;
    }
    tt_memcpy(buf, tt_string_cstr(&s), n);
    buf[n] = 0;

done:
    tt_string_destroy(&s);
    return n;
}

tt_u32_t tt_date_parse(IN tt_date_t *date,
                       IN const tt_char_t *format,
                       IN tt_char_t *buf,
                       IN tt_u32_t len)
{
    tt_char_t tmp[128], *s;
    tt_u32_t result;

    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    if (len < sizeof(tmp)) {
        s = tmp;
    } else {
        s = tt_malloc(len + 1);
        if (s == NULL) {
            TT_ERROR("no mem to parse date");
            return 0;
        }
    }
    tt_memcpy(s, buf, len);
    s[len] = 0;

    result = tt_date_parse_ntv(date, format, s);
    if (s != tmp) {
        tt_free(s);
    }
    return result;
}

__sym_t *__find_sym(IN tt_char_t c)
{
    tt_u32_t i;
    for (i = 0; i < sizeof(__sym) / sizeof(__sym[0]); ++i) {
        __sym_t *sym = &__sym[i];
        if (sym->c == c) {
            return sym;
        }
    }
    return NULL;
}

tt_result_t __Y_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t y = tt_date_get_year(d) % 100;
    tt_char_t c[3] = {0};

    c[0] = y / 10 + '0';
    c[1] = y % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __y_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t y = tt_date_get_year(d) % 100, y1;
    tt_char_t c[3] = {0};

    y1 = y / 10;
    if (y1 != 0) {
        c[0] = y1 + '0';
        c[1] = y % 10 + '0';
    } else {
        c[0] = y % 10 + '0';
    }
    return tt_string_append(s, c);
}

tt_result_t __C_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t y = tt_date_get_year(d);
    tt_char_t c[10] = {0};

    snprintf(c, sizeof(c) - 1, "%d", y);
    return tt_string_append(s, c);
}

tt_result_t __B_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s, tt_month_name[tt_date_get_month(d)]);
}

tt_result_t __b_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s, tt_month_name_abbr[tt_date_get_month(d)]);
}

tt_result_t __D_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t day = tt_date_get_monthday(d);
    tt_char_t c[3] = {0};

    c[0] = day / 10 + '0';
    c[1] = day % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __d_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t day = tt_date_get_monthday(d), d1;
    tt_char_t c[3] = {0};

    d1 = day / 10;
    if (d1 != 0) {
        c[0] = d1 + '0';
        c[1] = day % 10 + '0';
    } else {
        c[0] = ' ';
        c[1] = day % 10 + '0';
    }
    return tt_string_append(s, c);
}

tt_result_t __H_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d);
    tt_char_t c[3] = {0};

    c[0] = h / 10 + '0';
    c[1] = h % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __h_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d) % 12;
    tt_char_t c[3] = {0};

    c[0] = h / 10 + '0';
    c[1] = h % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __M_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t m = tt_date_get_minute(d);
    tt_char_t c[3] = {0};

    c[0] = m / 10 + '0';
    c[1] = m % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __S_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t sec = tt_date_get_second(d);
    tt_char_t c[3] = {0};

    c[0] = sec / 10 + '0';
    c[1] = sec % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __W_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s, tt_weekday_name[tt_date_get_weekday(d)]);
}

tt_result_t __w_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s, tt_weekday_name_abbr[tt_date_get_weekday(d)]);
}

tt_result_t __A_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d);
    return tt_string_append(s, TT_COND(h < 12, "AM", "PM"));
}

tt_result_t __a_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d);
    return tt_string_append(s, TT_COND(h < 12, "am", "pm"));
}

tt_result_t __Z_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s, tt_tmzone_name_rfc1123[tt_date_get_tmzone(d)]);
}

tt_result_t __z_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s, tt_tmzone_name_iso8601[tt_date_get_tmzone(d)]);
}
