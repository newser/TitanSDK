/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <io/tt_file_system.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_spinlock.h>
#include <os/tt_task.h>
#include <time/tt_time_reference.h>

//#define __perf_vs
#ifdef __perf_vs
#include <uv.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h> /* memset */
#include <sys/stat.h>
#endif

/*
 * local definition
 */

// === routine declarations ================
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_open)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_rw)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_multhread)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_consistency)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_dir_basic)

// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fs_case)

#if 1
TT_TEST_CASE("tt_unit_test_fs_basic",
             "testing fs basic",
             tt_unit_test_fs_basic,
             NULL,
             NULL,
             NULL,
             NULL,
             NULL)
,

    TT_TEST_CASE("tt_unit_test_fs_open",
                 "testing fs open close",
                 tt_unit_test_fs_open,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_fs_rw",
                 "testing fs read write",
                 tt_unit_test_fs_rw,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dir_basic",
                 "testing dir basic",
                 tt_unit_test_dir_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_fs_multhread",
                 "testing fs read write in multi thread",
                 tt_unit_test_fs_multhread,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_fs_consistency",
                 "testing fs read write consistency",
                 tt_unit_test_fs_consistency,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

    TT_TEST_CASE_LIST_DEFINE_END(fs_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_FS, 0, fs_case)

/*
 * interface implementation
 */


/*
TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_consistency)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    // test end
    TT_TEST_CASE_LEAVE()
}
*/

#if TT_ENV_OS_IS_WINDOWS
#define __SC_TEST_FILE "测试"
#elif TT_ENV_OS_IS_IOS
#define __SC_TEST_FILE "../tmp/测试"
#else
#define __SC_TEST_FILE "测试"
#endif

        TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;

    TT_TEST_CASE_ENTER()

    tt_fremove(__SC_TEST_FILE);

    // create
    ret = tt_fcreate(__SC_TEST_FILE, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_open)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_file_t tf, tf2;
    tt_result_t ret;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(__SC_TEST_FILE);

    // fail as it does not exist
    ret = tt_fopen(&tf, __SC_TEST_FILE, 0, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // open file
    ret = tt_fopen(&tf,
                   __SC_TEST_FILE,
                   TT_FO_CREAT | TT_FO_READ | TT_FO_WRITE,
                   NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    {
        tt_u8_t buf1[100] = "test1";
        tt_u8_t buf2[100] = {0};
        tt_u64_t d;

        ret =
            tt_fwrite(&tf, buf1, (tt_u32_t)strlen((const char *)buf1) + 1, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, (strlen((const char *)buf1) + 1), "");

        ret = tt_fwrite(&tf, buf1, 0, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, 0, "");

        tt_fseek(&tf, TT_FSEEK_BEGIN, 0, NULL);
        ret = tt_fread(&tf, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, strlen((const char *)buf1) + 1, "");

        ret = tt_fread(&tf, buf2, 0, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, 0, "");

        ret = tt_fread(&tf, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_END, "");

        // write append
        ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_RDWR | TT_FO_APPEND, NULL);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, (tt_u32_t)strlen((const char *)buf1) + 1, "");

        tt_fseek(&tf2, TT_FSEEK_CUR, 0, &d);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(d, (tt_u32_t)strlen((const char *)buf1) + 1, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_END, "");

        ret =
            tt_fwrite(&tf2, buf1, (tt_u32_t)strlen((const char *)buf1) + 1, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, strlen((const char *)buf1) + 1, "");

        ret = tt_fwrite(&tf2, buf1, 0, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, 0, "");

        // even fseek begin, write still append data, so fpos should be
        // after 2 strings
        ret = tt_fseek(&tf2, TT_FSEEK_CUR, 0, &d);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(d, 2 * (strlen((const char *)buf1) + 1), "");

        d = ~0;
        ret = tt_fseek(&tf2,
                       TT_FSEEK_CUR,
                       -2 * (strlen((const char *)buf1) + 1),
                       &d);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(d, 0, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, 2 * (strlen((const char *)buf1) + 1), "");

        ret = tt_fread(&tf2, buf2, 0, &n);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_EQUAL(n, 0, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_END, "");
        TT_UT_EQUAL(n, 0, "");

        tt_fclose(&tf2);
    }
    {
        tt_u8_t buf2[100] = {0};

        ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_TRUNC, NULL);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        // truncated
        ret = tt_fread(&tf, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_END, "");

        tt_fclose(&tf2);
    }

    tt_fclose(&tf);

    // create file exclusively, fail as file exist
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_EXCL, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // create and truncate
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_TRUNC, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    // now tf and tf2 could still be used

    // close to delete
    tt_fclose(&tf2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_rw)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_file_t tf, tf2;
    tt_result_t ret;

    TT_TEST_CASE_ENTER()
    // test start
    // setlocale(LC_ALL, "chs");
    tt_fremove(__SC_TEST_FILE);

    // create
    ret = tt_fopen(&tf,
                   __SC_TEST_FILE,
                   TT_FO_CREAT | TT_FO_EXCL | TT_FO_RDWR,
                   NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    // could write and read before close
    {
        tt_u8_t buf[100] = {0};
        tt_u8_t rbuf[100] = {0};
        tt_u32_t n = 0;
        tt_result_t ret;

        tt_strncpy((tt_char_t *)buf, "12345", (tt_u32_t)sizeof("12345"));

        while (n < sizeof(buf)) {
            tt_u32_t wn = 0;
            ret = tt_fwrite(&tf, buf + n, sizeof(buf) - n, &wn);
            TT_UT_EQUAL(ret, TT_SUCCESS, "");
            n += wn;
        }
        TT_UT_EXP(n == sizeof(buf), "");

        ret = tt_fseek(&tf, TT_FSEEK_BEGIN, 0, NULL);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");

        n = 0;
        while (n < sizeof(rbuf)) {
            tt_u32_t rn = 0;
            ret = tt_fread(&tf, rbuf + n, sizeof(rbuf) - n, &rn);
            TT_UT_EQUAL(ret, TT_SUCCESS, "");
            n += rn;
        }
        TT_UT_EXP(n == sizeof(rbuf), "");

        TT_UT_EQUAL(tt_strncmp((const char *)buf,
                               (const char *)rbuf,
                               sizeof(buf)),
                    0,
                    "");
    }

    // open should fail
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_READ, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // close to delete
    tt_fclose(&tf);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dir_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dir_t dir, dir2;
    tt_result_t ret = TT_FAIL;

    tt_dirent_t de;

    TT_TEST_CASE_ENTER()
// test start

#if TT_ENV_OS_IS_IOS
#define __TEST_DIR "../tmp/test_dir"
#else
//#define __TEST_DIR "./≤‚ ‘ƒø¬º")
#define __TEST_DIR "./test_dir"
#endif

    tt_dremove(__TEST_DIR);

    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dread(&dir, &de);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dread(&dir, &de);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dopen(&dir2, __TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dread(&dir2, &de);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    // nothing else except . and ..
    ret = tt_dread(&dir, &de);
    TT_UT_EQUAL(ret, TT_END, "");

    // could continue read
    ret = tt_dread(&dir2, &de);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dread(&dir2, &de);
    TT_UT_EQUAL(ret, TT_END, "");

    ret = tt_dremove(__TEST_DIR);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    tt_dclose(&dir);

    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_dclose(&dir2);

    // remove subdirs
    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_dcreate(__TEST_DIR "/s1", NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_dcreate(__TEST_DIR "/s1/s2", NULL);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dremove(__TEST_DIR);
    TT_UT_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

#if TT_ENV_OS_IS_IOS
#define __ut_fname "../tmp/a.txt"
#else
#define __ut_fname "a.txt"
#endif

#define __task_num 10
#define __fiber_per_task 10

static tt_u32_t __err_line;
static tt_u32_t __task_end[__task_num];
static tt_bool_t __fiber_end[__task_num][__fiber_per_task];

tt_result_t __fs_fiber(IN void *param)
{
    tt_u32_t t_idx = (tt_u32_t)(tt_uintptr_t)param;
    tt_u32_t f_idx = t_idx & 0xFFFF;
    tt_file_t f;
    tt_u8_t buf[2000];
    tt_u32_t i, n, l;

    t_idx >>= 16;
    // TT_INFO("task[%d], fiber[%d, %p]", t_idx, f_idx, tt_current_fiber());

    if (!TT_OK(tt_fopen(&f, __ut_fname, TT_FO_RDWR, NULL))) {
        // may fail because of "too many opened files"
        //__err_line = __LINE__;
        // tt_task_exit(NULL);

        __fiber_end[t_idx][f_idx] = TT_TRUE;
        if (++__task_end[t_idx] == __fiber_per_task) {
            // TT_INFO("task[%d] exit", t_idx);
            // tt_task_exit(NULL);
        }

        return TT_FAIL;
    }

    i = 0;
    n = 3; // n = tt_rand_u32() % 100;
    l = 0;
    while (i++ < n) {
        tt_u32_t act = tt_rand_u32() % 2;
        tt_u32_t len = 0;
        if (act == 0) {
            tt_result_t ret = TT_FAIL;
            ret = tt_fread(&f, buf, tt_rand_u32() % sizeof(buf), &len);
            if (TT_OK(ret)) {
                l += len;
            }
        } else {
            tt_fwrite(&f, buf, tt_rand_u32() % sizeof(buf), &len);
        }
    }

    tt_fclose(&f);

    if (++__task_end[t_idx] == __fiber_per_task) {
        // TT_INFO("task[%d] exit", t_idx);
        // tt_task_exit(NULL);
    }
    //++__fiber_end[t_idx][f_idx];
    __fiber_end[t_idx][f_idx] = TT_TRUE;
    // TT_INFO("fiber[%d][%d] exit", t_idx, f_idx);
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_multhread)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t task[__task_num];
    tt_result_t ret;
    tt_u32_t i, j;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fcreate(__ut_fname, NULL);

    for (i = 0; i < __task_num; ++i) {
        ret = tt_task_create(&task[i], NULL);
        TT_UT_SUCCESS(ret, "");

        for (j = 0; j < __fiber_per_task; ++j) {
            tt_task_add_fiber(&task[i],
                              NULL,
                              __fs_fiber,
                              (void *)(tt_uintptr_t)((i << 16) | j),
                              NULL);
        }
    }

    __err_line = 0;
    for (i = 0; i < __task_num; ++i) {
        ret = tt_task_run(&task[i]);
        TT_UT_SUCCESS(ret, "");
    }

    for (i = 0; i < __task_num; ++i) {
        tt_task_wait(&task[i]);
    }
    TT_UT_EQUAL(__err_line, 0, "");

    tt_fremove(__ut_fname);

    // test end
    TT_TEST_CASE_LEAVE()
}

#if TT_ENV_OS_IS_IOS
static const tt_char_t *fname[4] = {"../tmp/1.txt",
                                    "../tmp/2.txt",
                                    "../tmp/3.txt",
                                    "../tmp/4.txt"};
#else
static const tt_char_t *fname[4] = {"1.txt", "2.txt", "3.txt", "4.txt"};
#endif

static tt_bool_t __fb_end;

tt_result_t __wr_fiber(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;
    tt_char_t buf[16 * 1024];
    tt_u32_t len, i, size = 0;
    tt_u8_t last = 0;
    tt_file_t f;
    tt_result_t ret;

    ret =
        tt_fopen(&f, fname[idx], TT_FO_CREAT | TT_FO_TRUNC | TT_FO_WRITE, NULL);
    if (!TT_OK(ret)) {
        __err_line = __LINE__;
        goto exit;
    }

    //  write 16M
    while (size < (1 << 24)) {
        tt_u32_t n;

        len = tt_rand_u32() % sizeof(buf);
        for (i = 0; i < len; ++i) {
            buf[i] = '0' + last;
            ++last;
            last %= 10;
        }

        ret = tt_fwrite(&f, (tt_u8_t *)buf, len, &n);
        if (!TT_OK(ret)) {
            __err_line = __LINE__;
            goto exit;
        }

        size += len;
    }

    tt_fclose(&f);

exit:
    if (++__fb_end == 4) {
        // tt_task_exit(NULL);
    }

    return TT_SUCCESS;
}

tt_result_t __rd_fiber(IN void *param)
{
    tt_u32_t idx = (tt_u32_t)(tt_uintptr_t)param;
    tt_char_t buf[16 * 1024];
    tt_u32_t len, i, size = 0;
    tt_u8_t last = 0;
    tt_file_t f;
    tt_result_t ret;

    ret = tt_fopen(&f, fname[idx % 4], TT_FO_RDWR, NULL);
    if (!TT_OK(ret)) {
        __err_line = __LINE__;
        goto exit;
    }

    //  read 16M
    while (1) {
        tt_u32_t n;

        len = tt_rand_u32() % sizeof(buf);
        ret = tt_fread(&f, (tt_u8_t *)buf, len, &n);
        if (TT_OK(ret)) {
            for (i = 0; i < n; ++i) {
                if (buf[i] != (last + '0')) {
                    __err_line = __LINE__;
                    goto exit;
                }

                ++last;
                last %= 10;
            }
        } else if (ret == TT_END) {
            break;
        } else {
            __err_line = __LINE__;
            goto exit;
        }
    }

    tt_fclose(&f);

exit:
    if (++__fb_end == 8) {
        // tt_task_exit(NULL);
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_consistency)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t task;
    tt_result_t ret;
    tt_u32_t j;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_task_create(&task, NULL);
    TT_UT_SUCCESS(ret, "");

    for (j = 0; j < 4; ++j) {
        tt_task_add_fiber(&task,
                          NULL,
                          __wr_fiber,
                          (void *)(tt_uintptr_t)j,
                          NULL);
    }

    __fb_end = 0;
    __err_line = 0;
    ret = tt_task_run(&task);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&task);
    TT_UT_EQUAL(__err_line, 0, "");

    ret = tt_task_create(&task, NULL);
    TT_UT_SUCCESS(ret, "");

    // read, check consistency
    for (j = 0; j < 8; ++j) {
        tt_task_add_fiber(&task,
                          NULL,
                          __rd_fiber,
                          (void *)(tt_uintptr_t)j,
                          NULL);
    }

    __fb_end = 0;
    __err_line = 0;
    ret = tt_task_run(&task);
    TT_UT_SUCCESS(ret, "");

    tt_task_wait(&task);
    TT_UT_EQUAL(__err_line, 0, "");

    // remove
    for (j = 0; j < sizeof(fname) / sizeof(fname[0]); ++j) {
        tt_fremove(fname[j]);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
