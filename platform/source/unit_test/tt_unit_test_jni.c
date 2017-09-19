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

#if TT_ENV_OS_IS_ANDROID
#include <jni.h>
#endif

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_ANDROID

tt_buf_t tt_g_jni_buf;

tt_result_t __ut_fiber(IN void *param)
{
    const tt_char_t *name = (const tt_char_t *)param;

    tt_test_framework_init(0);
    tt_test_unit_init(NULL);

#if 0
    tt_test_unit_run(NULL);
    tt_test_unit_list(NULL);
#endif

    if (name != NULL) {
        TT_INFO("case name: %s", name);
        if (tt_strcmp(name, "all") == 0) {
            tt_test_unit_run(NULL);
            // tt_ut_ok = TT_TRUE;
        } else if (TT_OK(tt_test_unit_run(name))) {
            // tt_ut_ok = TT_TRUE;
        }
        tt_test_unit_list(NULL);
    } else {
        TT_INFO("unit_test <case name> | all");
    }

    return TT_SUCCESS;
}

JNIEXPORT jstring JNICALL
Java_com_titansdk_titansdkunittest_TTUnitTestJNI_runUT(JNIEnv *env,
                                                       jobject obj,
                                                       jstring name)
{
    static tt_bool_t initialized = TT_FALSE;
    tt_task_t t;

    if (!initialized) {
        tt_platform_init(NULL);

        tt_thread_create_local(NULL);

        initialized = TT_TRUE;
    }

    tt_buf_init(&tt_g_jni_buf, NULL);

    tt_task_create(&t, NULL);
    tt_task_add_fiber(&t,
                      NULL,
                      __ut_fiber,
                      (void *)(*env)->GetStringUTFChars(env, name, 0),
                      NULL);
    tt_task_run(&t);
    tt_task_wait(&t);
    TT_INFO("exiting");

    tt_buf_put_u8(&tt_g_jni_buf, 0);
    return (*env)->NewStringUTF(env, (const char *)TT_BUF_RPOS(&tt_g_jni_buf));
}

#endif
