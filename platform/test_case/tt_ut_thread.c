/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <tt_platform.h>
#include <unit_test/tt_unit_test.h>

#include <log/tt_log.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_dll.h>
#include <os/tt_process.h>

// portlayer header files
#include <tt_cstd_api.h>

#include <stdlib.h>
#include <time.h>

/*
 * local definition
 */
// == macro

#define __LOCK_THREAD_LIST()                                                   \
    while (!TT_OK(tt_atomic_s32_cas(&tt_s_thread_list_lock, 0, 1)))
#define __UNLOCK_THREAD_LIST()                                                 \
    TT_ASSERT_ALWAYS(TT_OK(tt_atomic_s32_cas(&tt_s_thread_list_lock, 1, 0)))

// == extern declaration

tt_atomic_s32_t tt_s_thread_list_lock;
tt_list_t tt_s_thread_list;

// == global variant

// == interface declaration
tt_result_t test_thread(IN void *param);

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_thread_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_thread_pressure)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_thread_sleep)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_thread_rand)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_thread_size2alloc)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_dll)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_process_basic)

// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(thread_case)

TT_TEST_CASE("tt_unit_test_thread_basic",
             "basic thread pressure test",
             tt_unit_test_thread_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,
#if 0
    TT_TEST_CASE("tt_unit_test_thread_pressure",
                 "thread pressure test",
                 tt_unit_test_thread_pressure,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_thread_sleep",
                 "thread sleep api",
                 tt_unit_test_thread_sleep,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_thread_rand",
                 "thread random number generator",
                 tt_unit_test_thread_rand,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dll",
                 "dll operations",
                 tt_unit_test_dll,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_process_basic",
                 "process basic operations",
                 tt_unit_test_process_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE_LIST_DEFINE_END(thread_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_THREAD, 0, thread_case)

    /*
     * interface implementation
     */

    /*
    TT_TEST_ROUTINE_DEFINE(name)
    {
        //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

        TT_TEST_CASE_ENTER()
        // test start

        // test end
        TT_TEST_CASE_LEAVE()
    }
    */

    TT_TEST_ROUTINE_DEFINE(tt_unit_test_thread_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_thread_t *thread;
    tt_bool_t numa_enabled;

    TT_TEST_CASE_ENTER()
    // test start

    thread = tt_thread_create(test_thread, NULL, NULL);
    tt_thread_wait(thread);

    numa_enabled = tt_platform_numa_enabled();
    // can not determine expected value on all platforms

    TT_RECORD_INFO("%s, numa_enabled: %d", "", numa_enabled);

    // test end
    TT_TEST_CASE_LEAVE()
}

tt_u32_t t_num;

#if 1
TT_TEST_ROUTINE_DEFINE(tt_unit_test_thread_pressure)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t thread_num = 0;
    tt_u32_t i = 0;
    tt_thread_t *thread1[3];
    tt_thread_t *thread[50];
    tt_result_t result = TT_FAIL;
    tt_thread_attr_t attr;

    TT_TEST_CASE_ENTER()
// test start

#if (TT_UNIT_TEST_CASE_FAST & TT_UNIT_TEST_THREAD_FAST)
    TT_TEST_CASE_LEAVE()
#endif

    tt_thread_attr_default(&attr);
    attr.detached = TT_TRUE;

    t_num = 3;

    // first do some simple test
    thread1[0] = tt_thread_create(test_thread, NULL, &attr);
    TT_TEST_CHECK_EQUAL(thread1[0], (void *)1, "");

    // null name is acceptable
    thread1[1] = tt_thread_create(test_thread, NULL, &attr);
    TT_TEST_CHECK_EQUAL(thread1[0], (void *)1, "");

    thread1[2] = tt_thread_create(test_thread, NULL, &attr);
    TT_TEST_CHECK_EQUAL(thread1[0], (void *)1, "");

    thread_num = 0;
    while (thread_num < 50) {
        // create a thread
        thread[thread_num] = tt_thread_create(test_thread,
                                              (void *)(tt_ptrdiff_t)thread_num,
                                              NULL);

        tt_sleep((rand() % 10) * 10);

        ++thread_num;
    }

    for (i = 0; i < sizeof(thread) / sizeof(thread[0]); ++i) {
        result = tt_thread_wait(thread[i]);
        TT_TEST_CHECK_EQUAL(result, TT_SUCCESS, "");
    }

    // detached
    thread_num = 0;
    while (thread_num < 50) {
        // create a thread
        thread[thread_num] = tt_thread_create(test_thread, NULL, &attr);

        tt_sleep((rand() % 10) * 10);

        ++thread_num;
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#endif

tt_result_t test_thread(IN void *param)
{
    tt_u32_t n = (tt_u32_t)(tt_ptrdiff_t)param;

    srand((tt_u32_t)time(NULL));

    tt_sleep((rand() % 10) * 100);

    {
        tt_u32_t s = rand() % (1 << 20);
        void *p = tt_malloc(s);
        tt_free(p);
    }

    if (n & 1) {
        // use tt_thread_exit to exit
        tt_thread_exit();
    }

    return 0;
}

tt_result_t test_thread_sleep(IN void *param)
{
    tt_sleep(10);

    return 0;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_thread_sleep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_thread_t *thread;
    tt_s64_t s, e;
    TT_TEST_CASE_ENTER()
    // test start


    thread = tt_thread_create(test_thread_sleep, NULL, NULL);
    s = tt_time_ref();
    tt_thread_wait(thread);
    e = tt_time_ref();

    TT_RECORD_INFO("slept: %d", tt_time_ref2ms(e - s));

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_thread_rand)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u64_t n1, n2, n3, n4, n5;

    TT_TEST_CASE_ENTER()
    // test start

    n1 = tt_rand_u64() % 100;
    n2 = tt_rand_u64() % 100;
    n3 = tt_rand_u64() % 100;
    n4 = tt_rand_u64() % 100;
    n5 = tt_rand_u64() % 100;

    TT_RECORD_INFO("%u - %u - %u - %u - %u", n1, n2, n3, n4, n5);

    // test end
    TT_TEST_CASE_LEAVE()
}

#if 0
TT_TEST_ROUTINE_DEFINE(tt_unit_test_thread_size2alloc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_u32_t last_size = 0, size = 0;
    tt_u32_t i;
    tt_tcache_t *tc;
    tt_u8_t *p;
    tt_u32_t r, allocated;

    TT_TEST_CASE_ENTER()
    // test start

    // generate tcache
    p = tt_malloc(1);
    tt_free(p);
    tc = tt_current_thread()->tcache;
    TT_TEST_CHECK_NOT_EQUAL(tc, NULL, "");

    last_size = 0;
    size = 0;
    for (i = 0; size < tc->max_slice_size; /*i < tc->binlist_num;*/
         ++i) {
        size = tt_bin_idx2size(i);

        r = tt_rand_u32() % (size - last_size) + 1;
        r += last_size;
        // r in (last_size,size]

        allocated = tt_tcache_size2alloc(tc, r);
        TT_TEST_CHECK_EQUAL(allocated, size, "");
#if 0
        TT_INFO("---- size: %d, allocated: %d [%f%%]",
                r, allocated, 100*((float)allocated-(float)r)/allocated);
#endif
        last_size = size;
    }
    // now size > tc->max_slice_size
    size += tt_rand_u32() & 0xFFFF;
    allocated = tt_tcache_size2alloc(tc, size);
    TT_U32_ALIGN_INC_PAGE(size);
    TT_TEST_CHECK_EQUAL(allocated, size, "");

#ifdef TT_MEMORY_TAG_ENABLE
    last_size = 0;
    size = 0;
    for (i = 0; size < tc->max_slice_size; /*i < tc->binlist_num;*/
         ++i) {
        tt_u32_t r, allocated, no_tag_size;

        size = tt_bin_idx2size(i);
        if ((size - last_size) <= ((tt_u32_t)sizeof(tt_tcache_bin_tag_t) +
                                   (1 << tt_g_cpu_align_order))) {
            continue;
        }
        no_tag_size =
            (size - last_size) - ((tt_u32_t)sizeof(tt_tcache_bin_tag_t) +
                                  (1 << tt_g_cpu_align_order));

        r = tt_rand_u32() % no_tag_size + 1;
        r += last_size;

        allocated = tt_msize(r);
        TT_TEST_CHECK_EQUAL(allocated + (tt_u32_t)sizeof(tt_tcache_bin_tag_t) +
                                (1 << tt_g_cpu_align_order),
                            size,
                            "");
#if 0
        TT_INFO("---- size(tag): %d, allocated: %d [%f%%]",
                r, allocated, 100*((float)allocated-(float)r)/allocated);
#endif
        last_size = size;
    }
    // now size > tc->max_slice_size
    size += tt_rand_u32() & 0xFFFF;
    allocated = tt_msize(size);
    size += (tt_u32_t)sizeof(tt_tcache_bin_tag_t) + (1 << tt_g_cpu_align_order);
    TT_U32_ALIGN_INC_PAGE(size);
    TT_TEST_CHECK_EQUAL(allocated + (tt_u32_t)sizeof(tt_tcache_bin_tag_t) +
                            (1 << tt_g_cpu_align_order),
                        size,
                        "");
#endif

    // test end
    TT_TEST_CASE_LEAVE()
}
#endif

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dll)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dll_t c_dll;
    tt_result_t ret;
    void *(*__c_memcpy)(void *dst, const void *src, size_t n);
    tt_u8_t buf1[10], buf2[10];

    TT_TEST_CASE_ENTER()
    // test start

    // relative path
    ret =
#if TT_ENV_OS_IS_MACOS
        tt_dll_create(&c_dll, "libc.dylib", NULL);
#elif TT_ENV_OS_IS_IOS
        tt_dll_create(&c_dll, "libc.dylib", NULL);
#elif TT_ENV_OS_IS_LINUX
        tt_dll_create(&c_dll, "libc.so.6", NULL);
#elif TT_ENV_OS_IS_WINDOWS
        tt_dll_create(&c_dll, "ntdll.dll", NULL);
#endif
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __c_memcpy = tt_dll_symbol(&c_dll, "memcpy");
    TT_TEST_CHECK_NOT_EQUAL(__c_memcpy, NULL, "");

    tt_memset(buf1, 0xd0, sizeof(buf1));
    __c_memcpy(buf2, buf1, sizeof(buf1));
    TT_TEST_CHECK_EQUAL(tt_memcmp(buf1, buf2, sizeof(buf1)), 0, "");

    __c_memcpy = tt_dll_symbol(&c_dll, "memcpy111");
    TT_TEST_CHECK_EQUAL(__c_memcpy, NULL, "");

    tt_dll_destroy(&c_dll);

    // absolute path
    ret =
#if TT_ENV_OS_IS_MACOS
        tt_dll_create(&c_dll, "/usr/lib/libc.dylib", NULL);
#elif TT_ENV_OS_IS_IOS
        tt_dll_create(&c_dll, "/usr/lib/libc.dylib", NULL);
#elif TT_ENV_OS_IS_LINUX
        tt_dll_create(&c_dll, "/lib/x86_64-linux-gnu/libc.so.6", NULL);
#elif TT_ENV_OS_IS_WINDOWS
        tt_dll_create(&c_dll, "C:\\Windows\\System32\\ntdll.dll", NULL);
#endif
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    __c_memcpy = tt_dll_symbol(&c_dll, "memcpy");
    TT_TEST_CHECK_NOT_EQUAL(__c_memcpy, NULL, "");

    tt_memset(buf1, 0xd0, sizeof(buf1));
    __c_memcpy(buf2, buf1, sizeof(buf1));
    TT_TEST_CHECK_EQUAL(tt_memcmp(buf1, buf2, sizeof(buf1)), 0, "");

    __c_memcpy = tt_dll_symbol(&c_dll, "memcpy111");
    TT_TEST_CHECK_EQUAL(__c_memcpy, NULL, "");

    tt_dll_destroy(&c_dll);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_process_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_process_t proc;
    tt_result_t ret;
    tt_u8_t ec = 0;

    tt_char_t *argv[20] = {0};
    tt_char_t *path;

    TT_TEST_CASE_ENTER()

    path = tt_process_path(NULL);
    TT_TEST_CHECK_NOT_EQUAL(path, NULL, "");

#if !TT_ENV_OS_IS_IOS

#if TT_ENV_OS_IS_WINDOWS
#define __app_file path
#elif TT_ENV_OS_IS_IOS
    extern const char *get_app_path();
//#define __app_file get_app_path()
#define __app_file path
#define __app_file_sc get_app_path()
#else
//#define __app_file "./app_unit_test"
#define __app_file path
#define __app_file_sc "./测试"
#endif

    // create a child process
    argv[0] = "tsk_unit_test";
    argv[1] = "process";
    argv[2] = "proc1";
    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_process_wait(&proc, TT_TRUE, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // create a child process which does not exist
    argv[0] = "tsk_unit_test";
    argv[1] = "process";
    argv[2] = "proc1";
    ret = tt_process_create(&proc, "./app_unit_test_not_exist", argv, NULL);
    // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    // ret = tt_process_wait(&proc, TT_TRUE, NULL);
    // TT_TEST_CHECK_NOT_EQUAL(ret, TT_SUCCESS, "");

    // create a child process and test nonblock wait
    argv[0] = "tsk_unit_test";
    argv[1] = "process";
    argv[2] = "proc_to";
    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    do {
        ret = tt_process_wait(&proc, TT_FALSE, NULL);
        if (TT_OK(ret)) {
            break;
        } else {
            TT_TEST_CHECK_EQUAL(ret, TT_TIME_OUT, "");
            TT_INFO("waiting for child process");
            tt_sleep(500);
        }
    } while (1);

    // create a child process and test receiving exit code
    argv[0] = "tsk_unit_test";
    argv[1] = "process";
    argv[2] = "proc_exit";
    argv[3] = "213";
    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_process_wait(&proc, TT_TRUE, &ec);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(ec, 213, "");

    // create a child process with more args
    argv[0] = "tsk_unit_test";
    argv[1] = "process";
    argv[2] = "proc_args";
    argv[3] = "3";
    argv[4] = "4";
    argv[5] = "5";
    argv[6] = "6";
    argv[7] = "7";
    argv[8] = "8";
    argv[9] = "9";
    argv[10] = "a";
    argv[11] = "b";
    argv[12] = "c";
    argv[13] = "d";
    argv[14] = "e";
    argv[15] = "f";
    ret = tt_process_create(&proc, __app_file, argv, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_process_wait(&proc, TT_TRUE, &ec);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(ec, 105, "");

#if 0
    // create a child process with name in non-english
    argv[0] = "tsk_unit_test";
    argv[1] = "process";
    argv[2] = "proc1";
    argv[3] = NULL;
    ret = tt_process_create(&proc, __app_file_sc, argv, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_process_wait(&proc, TT_TRUE, &ec);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
#endif

#endif

    tt_free(path);

    // test end
    TT_TEST_CASE_LEAVE()
}
