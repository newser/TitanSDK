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

#include <tt_platform.h>
#include <unit_test/tt_test_framework.h>

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

#if TT_ENV_OS_IS_IOS
void (*g_ios_display)(const char *str);
void __ios_display(const char *str);

#define __IOS_BUF_SIZE 1000
tt_char_t __ios_buf[__IOS_BUF_SIZE + 1];
#endif

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_test_class_t *s_test_class_head;
static tt_test_class_t *s_test_class_tail;

static void tt_test_class_insert(IN tt_test_class_t *test_class);
static void tt_test_item_insert(IN tt_test_class_t *test_class,
                                IN tt_test_item_t *item);

static tt_result_t tt_test_list_class(IN tt_test_class_t *test_class,
                                      IN OUT tt_u32_t *succ_num,
                                      IN OUT tt_u32_t *fail_num);
static tt_result_t tt_test_list_item(IN tt_test_item_t *item,
                                     IN OUT tt_u32_t *succ_num,
                                     IN OUT tt_u32_t *fail_num);

static tt_result_t tt_test_run_class(IN tt_test_class_t *test_class);
static tt_result_t tt_test_run_item(IN tt_test_item_t *item);

static tt_result_t tt_test_run_item_of(IN tt_test_class_t *test_class,
                                       IN const tt_char_t *name);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_test_framework_init(IN tt_ptr_t attribute)
{
    s_test_class_head = NULL;
    s_test_class_tail = NULL;

    return TT_SUCCESS;
}

void tt_test_framework_exit()
{
    tt_test_class_t *c = s_test_class_head;
    while (c != NULL) {
        tt_test_class_t *cur = c;
        c = c->next;
        tt_test_class_destroy(cur);
    }

    // help to check memory leak
    s_test_class_head = NULL;
    s_test_class_tail = NULL;
}

tt_result_t tt_test_class_create(IN const tt_char_t *name,
                                 IN tt_u32_t attribute,
                                 OUT tt_test_class_t **test_class)
{
    tt_test_class_t *created_class = NULL;

    if ((name == NULL) || (test_class == NULL)) {
        TT_TEST_ERROR("invailid param for creating test class");
        return TT_FAIL;
    }

    created_class = (tt_test_class_t *)tt_c_malloc(sizeof(tt_test_class_t));
    if (created_class == NULL) {
        TT_TEST_ERROR("no memory for creating test class struct");
        return TT_FAIL;
    }
    tt_memset(created_class, 0, sizeof(tt_test_class_t));

    tt_strncpy(created_class->name, name, TT_TEST_CLASS_NAME_LEN);
    // other values use 0 as default

    tt_test_class_insert(created_class);

    *test_class = created_class;
    return TT_SUCCESS;
}

void tt_test_class_destroy(IN tt_test_class_t *test_class)
{
    tt_test_item_t *i = test_class->head;
    while (i != NULL) {
        tt_test_item_t *cur = i;
        i = i->next;

        tt_c_free(cur);
    }

    tt_c_free(test_class);
}

tt_result_t tt_test_item_add(IN tt_test_class_t *test_class,
                             IN tt_test_entry_t *entry,
                             IN const tt_char_t *name,
                             IN const tt_char_t *comment)
{
    tt_test_item_t *created_item = NULL;

    if ((test_class == NULL) || (entry == NULL) ||
        (name == NULL)) // param "comment" can be null
    {
        TT_TEST_ERROR("invailid param for creating test item");
        return TT_FAIL;
    }

    created_item = (tt_test_item_t *)tt_c_malloc(sizeof(tt_test_item_t));
    if (created_item == NULL) {
        TT_TEST_ERROR("no memory for creating test item struct");
        return TT_FAIL;
    }
    tt_memset(created_item, 0, sizeof(tt_test_item_t));

    tt_memcpy(&created_item->entry, entry, sizeof(tt_test_entry_t));
    created_item->test_result = TT_E_PROCEED;
    tt_strncpy(created_item->name, name, TT_TEST_ITEM_NAME_LEN);
    if (comment != NULL) {
        tt_strncpy(created_item->comment, comment, TT_TEST_ITEM_COMMENT_LEN);
    }
    // other values use 0 as default

    tt_test_item_insert(test_class, created_item);
    return TT_SUCCESS;
}

tt_result_t tt_test_list(IN const tt_char_t *class_name,
                         IN OUT tt_u32_t *succ_num,
                         IN OUT tt_u32_t *fail_num)
{
    tt_test_class_t *test_class = s_test_class_head;

    if (test_class == NULL) {
        // no class
        return TT_SUCCESS;
    }

    TT_TEST_INFO("test result: \r\n");
#if TT_ENV_OS_IS_IOS
    __ios_display("test result: \n\n");
#elif TT_ENV_OS_IS_ANDROID
    __android_display("test result: \n\n");
#endif

    if (class_name == NULL) {
        // list all class
        while (test_class != NULL) {
            tt_test_list_class(test_class, succ_num, fail_num);
            test_class = test_class->next;
        }
    } else {
        // list matching class
        while (test_class != NULL) {
            if (tt_strncmp(class_name,
                           test_class->name,
                           TT_TEST_CLASS_NAME_LEN) == 0) {
                tt_test_list_class(test_class, succ_num, fail_num);
            }
            test_class = test_class->next;
        }
    }

    return TT_SUCCESS;
}

tt_result_t tt_test_run(IN const tt_char_t *name)
{
    tt_test_class_t *test_class = s_test_class_head;

    if (test_class == NULL) {
        // no class
        return TT_SUCCESS;
    }

    if (name == NULL) {
        // run all class
        while (test_class != NULL) {
            tt_test_run_class(test_class);
            test_class = test_class->next;
        }
        return TT_SUCCESS;
    } else {
        while (test_class != NULL) {
            tt_result_t result;

            if (tt_strcmp(test_class->name, name) == 0) {
                tt_test_run_class(test_class);
                return TT_SUCCESS;
            }

            result = tt_test_run_item_of(test_class, name);
            if (result != TT_E_NOEXIST) {
                test_class->attribute |= __TCLS_TOUCHED;
                return result;
            }
            test_class = test_class->next;
        }
        return TT_E_NOEXIST;
    }
}

void tt_test_error_info(IN tt_test_item_t *item,
                        IN const tt_char_t *function,
                        IN const tt_u32_t line,
                        IN const tt_char_t *error_info)
{
    tt_memset(item->info, 0, sizeof(item->info));
    tt_snprintf(item->info,
                TT_TEST_ITEM_INFO_LEN,
                "error at <%s:%d>: %s",
                function,
                line,
                error_info);
}

void tt_test_info(IN tt_test_item_t *item,
                  IN const tt_char_t *function,
                  IN tt_u32_t line,
                  IN const tt_char_t *format,
                  ...)
{
    va_list args;

    tt_memset(item->info, 0, sizeof(item->info));

    va_start(args, format);
    tt_vsnprintf(item->info, TT_TEST_ITEM_INFO_LEN, format, args);
    va_end(args);
}

void tt_test_class_insert(IN tt_test_class_t *test_class)
{
    if (s_test_class_tail != NULL) {
        s_test_class_tail->next = test_class;
        s_test_class_tail = test_class;
    } else {
        s_test_class_head = test_class;
        s_test_class_tail = test_class;
    }
}

void tt_test_item_insert(IN tt_test_class_t *test_class,
                         IN tt_test_item_t *item)
{
    if (test_class->tail != NULL) {
        test_class->tail->next = item;
        test_class->tail = item;
    } else {
        test_class->head = item;
        test_class->tail = item;
    }
}

tt_result_t tt_test_list_class(IN tt_test_class_t *test_class,
                               IN OUT tt_u32_t *succ_num,
                               IN OUT tt_u32_t *fail_num)
{
    tt_test_item_t *item = test_class->head;

    if (!(test_class->attribute & __TCLS_TOUCHED)) {
        return TT_SUCCESS;
    }

    TT_TEST_INFO("== class: %s\r\n", test_class->name);
#if TT_ENV_OS_IS_IOS
    snprintf(__ios_buf, __IOS_BUF_SIZE, "== class: %s\n\n", test_class->name);
    __ios_display(__ios_buf);
#elif TT_ENV_OS_IS_ANDROID
    __android_display("== class: %s\r\n", test_class->name);
#endif

    while (item != NULL) {
        tt_test_list_item(item, succ_num, fail_num);
        item = item->next;
    }

    return TT_SUCCESS;
}

tt_result_t tt_test_list_item(IN tt_test_item_t *item,
                              IN OUT tt_u32_t *succ_num,
                              IN OUT tt_u32_t *fail_num)
{
    if (item->test_result == TT_E_PROCEED) {
        return TT_SUCCESS;
    }

    TT_TEST_INFO("   |-- case:    %s", item->name);
    TT_TEST_INFO("   |   comment: %s", item->comment);
    TT_TEST_INFO("   |   result:  %s",
                 TT_OK(item->test_result) ? "OK" : "Fail");
    TT_TEST_INFO("   |-- info:    %s\r\n", item->info);

#if TT_ENV_OS_IS_IOS
    snprintf(__ios_buf, __IOS_BUF_SIZE, "   |-- case:    %s\n", item->name);
    __ios_display(__ios_buf);
    snprintf(__ios_buf, __IOS_BUF_SIZE, "   |   comment: %s\n", item->comment);
    __ios_display(__ios_buf);
    snprintf(__ios_buf,
             __IOS_BUF_SIZE,
             "   |   result:  %s\n",
             TT_OK(item->test_result) ? "OK" : "Fail");
    __ios_display(__ios_buf);
    snprintf(__ios_buf, __IOS_BUF_SIZE, "   |-- info:    %s\n\n", item->info);
    __ios_display(__ios_buf);
#elif TT_ENV_OS_IS_ANDROID
    __android_display("   |-- case:    %s", item->name);
    __android_display("   |   comment: %s", item->comment);
    __android_display("   |   result:  %s",
                      TT_OK(item->test_result) ? "OK" : "Fail");
    __android_display("   |-- info:    %s\r\n", item->info);
#endif

    if (TT_OK(item->test_result)) {
        *succ_num += 1;
    } else {
        *fail_num += 1;
    }

    return TT_SUCCESS;
}

tt_result_t tt_test_run_class(IN tt_test_class_t *test_class)
{
    tt_test_item_t *item = test_class->head;

    test_class->attribute |= __TCLS_TOUCHED;

    TT_TEST_INFO("== start running class: %s\r\n", test_class->name);
    while (item != NULL) {
        tt_test_run_item(item);
        item = item->next;
    }
    TT_TEST_INFO("== end running class: %s\r\n\r\n", test_class->name);

    return TT_SUCCESS;
}

tt_result_t tt_test_run_item_of(IN tt_test_class_t *test_class,
                                IN const tt_char_t *name)
{
    tt_test_item_t *item = test_class->head;
    while (item != NULL) {
        if (tt_strcmp(item->name, name) == 0) {
            tt_test_run_item(item);
            return item->test_result;
        }
        item = item->next;
    }
    return TT_E_NOEXIST;
}

tt_result_t tt_test_run_item(IN tt_test_item_t *item)
{
    tt_test_entry_t *entry = &item->entry;

    // before routine
    if (entry->routine_enter != NULL) {
        entry->routine_enter(entry->enter_param);
    }
    // run routine
    if (entry->routine != NULL) {
        item->test_result = entry->routine(entry->param, item);
    }
    // after routine
    if (entry->routine_leave != NULL) {
        entry->routine_leave(entry->leave_param);
    }

    TT_TEST_INFO("   |-- case:    %s", item->name);
    TT_TEST_INFO("   |   comment: %s", item->comment);
    TT_TEST_INFO("   |   result:  %s",
                 TT_OK(item->test_result) ? "OK" : "Fail");
    TT_TEST_INFO("   |-- info:    %s\r\n", item->info);

    return TT_SUCCESS;
}

#if TT_ENV_OS_IS_IOS
void __ios_display(const char *str)
{
    if (g_ios_display != NULL) {
        g_ios_display(str);
    }
}
#endif

tt_export void tt_test_gen_sh_unix()
{
    tt_buf_t buf;
    tt_test_class_t *test_class;

    tt_test_framework_init(0);
    tt_test_unit_init(NULL);

    tt_buf_init(&buf, NULL);

    test_class = s_test_class_head;
    while (test_class != NULL) {
        tt_test_item_t *item = test_class->head;
        while (item != NULL) {
            TT_DO_G(done,
                    tt_buf_putf(&buf,
                                "    - export TT_CASE=%s; ./run_case.sh\n",
                                item->name));
            item = item->next;
        }
        test_class = test_class->next;
    }

    tt_buf_put_u8(&buf, 0);
    printf("%s", TT_BUF_RPOS(&buf));

done:
    tt_buf_destroy(&buf);
}

tt_export void tt_test_gen_sh_win()
{
    tt_buf_t buf;
    tt_test_class_t *test_class;

    tt_test_framework_init(0);
    tt_test_unit_init(NULL);

    tt_buf_init(&buf, NULL);

    test_class = s_test_class_head;
    while (test_class != NULL) {
        tt_test_item_t *item = test_class->head;
        while (item != NULL) {
            TT_DO_G(done,
                    tt_buf_putf(&buf,
                                "    - cmd: set TT_CASE=%s& run_case.bat\n",
                                item->name));
            item = item->next;
        }
        test_class = test_class->next;
    }

    tt_buf_put_u8(&buf, 0);
    printf("%s", TT_BUF_RPOS(&buf));

done:
    tt_buf_destroy(&buf);
}
