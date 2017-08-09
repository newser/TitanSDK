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

/**
@file tt_test_framework.h
@brief test framework

test framework
*/

#ifndef __TT_TEST_FRAMEWORK__
#define __TT_TEST_FRAMEWORK__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <algorithm/tt_buffer_format.h>
#include <log/tt_log.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_TEST_FRAMEWORK_MODULE_NAME "TestFramework"

#define TT_TEST_ITEM_NAME_LEN (32)
#define TT_TEST_ITEM_COMMENT_LEN (64)
#define TT_TEST_ITEM_INFO_LEN (128)

#define TT_TEST_CLASS_NAME_LEN (32)

#define TT_TEST_DETAIL TT_DEBUG
#define TT_TEST_INFO TT_INFO
#define TT_TEST_ERROR TT_ERROR

#if TT_ENV_OS_IS_ANDROID
#define __android_display(...)                                                 \
    do {                                                                       \
        extern tt_buf_t tt_g_jni_buf;                                          \
        tt_buf_putf(&tt_g_jni_buf, __VA_ARGS__);                               \
        tt_buf_put_u8(&tt_g_jni_buf, '\n');                                    \
    } while (0)
#endif

/*
 * name:
 *   TT_TEST_ERROR
 * description:
 *   record error info during test item running
 * param:
 *   - str:
 *       error info
 * return:
 * note:
 *   such macro can ONLY be used within the test routine
 *   and the maximum length of the error info is now 128bytes
 */
#define TT_RECORD_ERROR(str)                                                   \
    tt_test_error_info((tt_test_item_t *)item, __FUNCTION__, __LINE__, str);
#if 0
#define TT_RECORD_INFO(format, ...)                                            \
    tt_test_info((tt_test_item_t *)item,                                       \
                 __FUNCTION__,                                                 \
                 __LINE__,                                                     \
                 format " <%s:%d>",                                            \
                 __VA_ARGS__,                                                  \
                 __FUNCTION__,                                                 \
                 __LINE__);
#else
#define TT_RECORD_INFO(format, ...)                                            \
    tt_test_info((tt_test_item_t *)item,                                       \
                 __FUNCTION__,                                                 \
                 __LINE__,                                                     \
                 format,                                                       \
                 __VA_ARGS__);
#endif

#define TT_TEST_ROUTINE_DECLARE(routine_name)                                  \
    tt_result_t routine_name(void *routine_param, void *test_item);
#define TT_TEST_ROUTINE_DEFINE(routine_name)                                   \
    tt_result_t routine_name(void *routine_param, void *test_item)
#define TT_TEST_ROUTINE_PARAM(type) ((type)routine_param)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_result_t (*tt_test_routine_t)(void *param, void *item);

typedef void (*tt_test_routine_enter_t)(void *enter_param);
typedef void (*tt_test_routine_leave_t)(void *leave_param);

typedef struct tt_test_entry_struct
{
    tt_test_routine_t routine;
    void *param;

    tt_test_routine_enter_t routine_enter;
    void *enter_param;
    tt_test_routine_leave_t routine_leave;
    void *leave_param;
} tt_test_entry_t;

typedef struct tt_test_item_struct
{
    tt_test_entry_t entry;
    tt_result_t test_result;

    tt_char_t name[TT_TEST_ITEM_NAME_LEN + 1];
    tt_char_t comment[TT_TEST_ITEM_COMMENT_LEN + 1];
    tt_char_t info[TT_TEST_ITEM_INFO_LEN + 1];

    struct tt_test_item_struct *prev;
    struct tt_test_item_struct *next;
} tt_test_item_t;

typedef struct tt_test_class_struct
{
    // self info
    tt_char_t name[TT_TEST_CLASS_NAME_LEN + 1];
    tt_u32_t attribute;

    // items list
    tt_test_item_t *head;
    tt_test_item_t *tail;

    // other classes
    struct tt_test_class_struct *next;
} tt_test_class_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/*
 * name:
 *   tt_test_framework_init
 * description:
 *   init test framework
 * param:
 *   - attribute:
 *       reserved
 *   - param2:
 *       @@
 * return:
 *   TT_SUCCESS if initialized succeeded,
 *   otherwise:
 *
 * note:
 */
tt_export tt_result_t tt_test_framework_init(IN tt_ptr_t attribute);

/*
 * name:
 *   tt_test_class_create
 * description:
 *   create a test class, generally a class represent all test
 *   cases for a submodule such like tt_trace, tt_mem_cache, etc.
 * param:
 *   - name:
 *       the test class name
 *   - attribute:
 *       reserved
 *   - test_class:
 *       return the pointer to the created test class struct
 * return:
 *   TT_SUCCESS if the struct is created succesfully,
 *   otherwise:
 *
 * note:
 *   the corresponding delete interface is not provided, for it
 *   seems no need to delete a test class, this submodule is just
 *   for test purpose
 */
tt_export tt_result_t tt_test_class_create(IN const tt_char_t *name,
                                           IN tt_u32_t attribute,
                                           OUT tt_test_class_t **test_class);

/*
 * name:
 *   tt_test_item_add
 * description:
 *   add a test case to the specified test class, an item always
 *   represent a test case
 * param:
 *   - test_class:
 *       the test class which the case belongs to
 *   - entry:
 *       the routine that the case would run
 *   - name:
 *       name of the case
 *   - comment:
 *       extra introduction of the case
 * return:
 *   TT_SUCCESS if the case is added to the class succesfully,
 *   otherwise:
 *
 * note:
 *   the corresponding delete interface is not provided, for it
 *   seems no need to delete a test item, this submodule is just
 *   for test purpose
 */
tt_export tt_result_t tt_test_item_add(IN tt_test_class_t *test_class,
                                       IN tt_test_entry_t *entry,
                                       IN const tt_char_t *name,
                                       IN const tt_char_t *comment);

/**
@fn tt_result_t tt_test_list(IN const tt_char_t *class_name,
                             IN OUT tt_u32_t *succ_num,
                             IN OUT tt_u32_t *fail_num)
list specified test classes as well as their cases

@param [in] class_name specify which test class should be listed if NULL,
            all test classes would be listed
@param [inout] succ_num return number of successful cases
@param [inout] fail_num return number of failed cases

@note
- all the test items of the test class specified would be listed including
  all information of a test item.
- the initial result of an item is TT_FAIL if not run yet
*/
tt_export tt_result_t tt_test_list(IN const tt_char_t *class_name,
                                   IN OUT tt_u32_t *succ_num,
                                   IN OUT tt_u32_t *fail_num);

/*
 * name:
 *   tt_test_run
 * description:
 *   run specified test classes as well as their cases
 * param:
 *   - class_name:
 *       specify which test class should be run
 *       if NULL, all test classes would be run
 * return:
 * note:
 *   the return value do not indicate if all cases are passed
 */
tt_export tt_result_t tt_test_run(IN const tt_char_t *class_name);

/*
 * name:
 *   tt_test_error_info
 * description:
 *   record error info to a test item
 * param:
 *   - item:
 *       the running item
 *   - function:
 *       at which function the error occurs
 *   - line:
 *       at which line the error occurs
 *   - error_info:
 *       the error info
 * return:
 * note:
 *   it's NOT recommended to use such api, try to use
 *   the error info may be truncated if too long
 */

tt_export void tt_test_error_info(IN tt_test_item_t *item,
                                  IN const tt_char_t *function,
                                  IN const tt_u32_t line,
                                  IN const tt_char_t *error_info);

// only final info is recorded, formal info are overwritten
tt_export void tt_test_info(IN tt_test_item_t *item,
                            IN const tt_char_t *function,
                            IN tt_u32_t line,
                            IN const tt_char_t *format,
                            ...);

#endif /* __TT_TEST_FRAMEWORK__ */
