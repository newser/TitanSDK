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

#include <algorithm/tt_quick_sort.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

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

void __short_sort(IN void *base,
                  IN tt_u32_t num,
                  IN tt_u32_t width,
                  IN tt_cmp_t cmp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_qsort(IN void *base,
              IN tt_u32_t num,
              IN tt_u32_t width,
              IN tt_cmp_t cmp)
{
    tt_u8_t *cur_start, *cur_end, *cur_middle;
    tt_u32_t cur_num;
    tt_u8_t *less_pos, *larger_pos;

    tt_u8_t *start_stack[30], *end_stack[30];
    tt_u32_t stack_top;

    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    if (num < 2) {
        // no need to sort
        return;
    }

    stack_top = 0;

    cur_start = (tt_u8_t *)base;
    cur_end = cur_start + width * (num - 1);

sub_sort:

    cur_num = (tt_u32_t)((cur_end - cur_start) / width + 1);

#define SHORT_SORT_THRESHOLD 8

    if (cur_num <= SHORT_SORT_THRESHOLD) {
        __short_sort(cur_start, cur_num, width, cmp);
    } else {
        // swap middle elem with first elem, preventing
        // from bad performance while sorting already
        // sorted array
        cur_middle = cur_start + (cur_num >> 1) * width;
        tt_swap(cur_start, cur_middle, width);

        // to find the right position of the elem pointed
        // by cur_start
        less_pos = cur_start;
        larger_pos = cur_end + width;
        while (1) {
            // find a elem which is larger then cur_start
            // from left to right
            do {
                less_pos += width;
            } while ((less_pos <= cur_end) && (cmp(less_pos, cur_start) <= 0));

            // find a elem which is less then cur_start
            // from right to left
            do {
                larger_pos -= width;
            } while ((larger_pos > cur_start) &&
                     (cmp(larger_pos, cur_start) >= 0));

            if (less_pos > larger_pos) {
                // proc ends, now all elems at (cur_start, larger_pos]
                // are less than cur_start, elems at [less_pos, cur_end]
                // are larger than cur_start
                break;
            }

            // keep processing
            tt_swap(less_pos, larger_pos, width);
        }

        // break from while(1), put the cur_start to right place
        tt_swap(cur_start, larger_pos, width);

        // put larger interval to stack
        if ((larger_pos - cur_start) >= (cur_end - larger_pos)) {
            // check if need to push to stack
            if ((cur_start + width) < larger_pos) {
                start_stack[stack_top] = cur_start;
                end_stack[stack_top] = larger_pos - width;
                ++stack_top;
            }

            // keep processing
            if (less_pos < cur_end) {
                cur_start = less_pos;
                goto sub_sort;
            }
        } else {
            // check if need to push to stack
            if (less_pos < cur_end) {
                start_stack[stack_top] = less_pos;
                end_stack[stack_top] = cur_end;
                ++stack_top;
            }

            if ((cur_start + width) < larger_pos) {
                cur_end = larger_pos - width;
                goto sub_sort;
            }
        }
    }

    // if current sorting is done, then process that pop
    // from stack
    if (stack_top > 0) {
        --stack_top;
        cur_start = start_stack[stack_top];
        cur_end = end_stack[stack_top];
        goto sub_sort;
    }

// sorting done

#undef SHORT_SORT_THRESHOLD
}

void __short_sort(IN void *base,
                  IN tt_u32_t num,
                  IN tt_u32_t width,
                  IN tt_cmp_t cmp)
{
    tt_u8_t *cur_start, *cur_end, *cur_max;

    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    cur_start = (tt_u8_t *)base;
    cur_end = cur_start + width * (num - 1);

    while (cur_start < cur_end) {
        tt_u8_t *elem = NULL;

        cur_max = cur_start;
        for (elem = cur_start + width; elem <= cur_end; elem += width) {
            if (cmp(elem, cur_max) > 0) {
                cur_max = elem;
            }
        }
        tt_swap(cur_max, cur_end, width);

        cur_end -= width;
    }
}
