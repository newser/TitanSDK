/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <event/tt_event_center.h>
#include <io/tt_file_system.h>
#include <io/tt_file_system_aio.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_spinlock.h>
#include <timer/tt_time_reference.h>

#include <tt_cstd_api.h>

#include <stdio.h>
#include <time.h>

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
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_create_open)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dir_basic)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_faio_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_faio_excep)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_faio_perf)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_faio_perf_vs)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_daio_basic)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_dir_excep)

TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_faio_evc)
TT_TEST_ROUTINE_DECLARE(tt_unit_test_fs_daio_evc)
// =========================================

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fs_case)

#ifndef __SIMULATE_FS_AIO_FAIL
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

    TT_TEST_CASE("tt_unit_test_fs_create_open",
                 "testing fs create and open api",
                 tt_unit_test_fs_create_open,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dir_basic",
                 "testing dir api basic",
                 tt_unit_test_dir_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_fs_faio_basic",
                 "testing fs file aio api basic",
                 tt_unit_test_fs_faio_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_fs_faio_evc",
                 "testing fs file aio event poller",
                 tt_unit_test_fs_faio_evc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_fs_daio_basic",
                 "testing fs dir aio basic",
                 tt_unit_test_fs_daio_basic,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_fs_daio_evc",
                 "testing fs dir aio event poller",
                 tt_unit_test_fs_daio_evc,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#endif

#if 0
#define __fa3_static_mem
    TT_TEST_CASE("tt_unit_test_fs_faio_perf", 
                 "testing fs file aio performance", 
                 tt_unit_test_fs_faio_perf, NULL, 
                 NULL, NULL,
                 NULL, NULL),
#endif

#if 1
    TT_TEST_CASE("tt_unit_test_fs_faio_perf",
                 "testing fs file aio performance",
                 tt_unit_test_fs_faio_perf,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_dir_excep",
                 "testing dir aio exceptional cases",
                 tt_unit_test_dir_excep,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),

    TT_TEST_CASE("tt_unit_test_fs_faio_excep",
                 "testing fs file aio exceptional cases",
                 tt_unit_test_fs_faio_excep,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 NULL),
#endif

#ifdef __perf_vs
    TT_TEST_CASE("tt_unit_test_fs_faio_perf_vs",
                 "testing fs file aio perf compare",
                 tt_unit_test_fs_faio_perf_vs,
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
TT_TEST_ROUTINE_DEFINE(name)
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

        TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_create_open)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_file_t tf, tf2;
    tt_result_t ret;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start
    tt_fremove(__SC_TEST_FILE);

    // create
    ret = tt_fcreate(__SC_TEST_FILE,
                     TT_FO_HINT_SEQUENTIAL | TT_FO_HINT_RANDOM | TT_FO_RDWR,
                     NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // open with create should fail as file does not exist
    ret = tt_fopen(&tf, __SC_TEST_FILE, 0, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // create file
    ret = tt_fopen(&tf, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_RDWR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    {
        tt_u8_t buf1[100] = "test1";
        tt_u8_t buf2[100] = {0};
        tt_u64_t d;

        ret =
            tt_fwrite(&tf, buf1, (tt_u32_t)strlen((const char *)buf1) + 1, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, (strlen((const char *)buf1) + 1), "");

        ret = tt_fwrite(&tf, buf1, 0, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, 0, "");

        tt_fseek(&tf, TT_FPOS_BEGIN, 0, NULL);
        ret = tt_fread(&tf, buf2, 90, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, strlen((const char *)buf1) + 1, "");

        ret = tt_fread(&tf, buf2, 0, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, 0, "");

        ret = tt_fread(&tf, buf2, 90, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_END, "");

        // write append
        ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_RDONLY | TT_FO_APPEND, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        // ret = tt_fread(&tf2, buf2, 90, &n);
        // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        // TT_TEST_CHECK_EQUAL(n, strlen(buf1)+1, "");
        tt_fseek(&tf2, TT_FPOS_CUR, 0, &d);

        ret =
            tt_fwrite(&tf2, buf1, (tt_u32_t)strlen((const char *)buf1) + 1, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, strlen((const char *)buf1) + 1, "");

        ret = tt_fwrite(&tf2, buf1, 0, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, 0, "");

        // even fseek begin, write still append data, so fpos should be
        // after 2 strings
        ret = tt_fseek(&tf2, TT_FPOS_CUR, 0, &d);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(d, 2 * (strlen((const char *)buf1) + 1), "");

        d = ~0;
        ret = tt_fseek(&tf2,
                       TT_FPOS_CUR,
                       -2 * (strlen((const char *)buf1) + 1),
                       &d);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(d, 0, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, 2 * (strlen((const char *)buf1) + 1), "");

        ret = tt_fread(&tf2, buf2, 0, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
        TT_TEST_CHECK_EQUAL(n, 0, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_END, "");
        TT_TEST_CHECK_EQUAL(n, 0, "");

        ret = tt_fclose(&tf2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    }
    {
        tt_u8_t buf2[100] = {0};

        ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_TRUNC, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        // truncated
        ret = tt_fread(&tf, buf2, 90, &n);
        TT_TEST_CHECK_EQUAL(ret, TT_END, "");

        ret = tt_fclose(&tf2);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    }

    // create file exclusively, fail as file exist
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_EXCL, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // create and truncate
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_TRUNC, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    // now tf and tf2 could still be used

    // close to delete
    ret = tt_fclose(&tf);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // close to delete
    ret = tt_fclose(&tf2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_basic)
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
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

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
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            n += wn;
        }
        TT_TEST_CHECK_EXP(n == sizeof(buf), "");

        ret = tt_fseek(&tf, TT_FPOS_BEGIN_NTV, 0, NULL);
        TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

        n = 0;
        while (n < sizeof(rbuf)) {
            tt_u32_t rn = 0;
            ret = tt_fread(&tf, rbuf + n, sizeof(rbuf) - n, &rn);
            TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
            n += rn;
        }
        TT_TEST_CHECK_EXP(n == sizeof(rbuf), "");

        TT_TEST_CHECK_EQUAL(tt_strncmp((const char *)buf,
                                       (const char *)rbuf,
                                       sizeof(buf)),
                            0,
                            "");
    }

    // open should fail
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_RDONLY_NTV, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // close to delete
    ret = tt_fclose(&tf);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dir_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dir_t dir, dir2;
    tt_result_t ret = TT_FAIL;

    tt_dir_entry_t de;

    TT_TEST_CASE_ENTER()
// test start

#if TT_ENV_OS_IS_IOS
#define __TEST_DIR "../tmp/test_dir"
#else
//#define __TEST_DIR "./≤‚ ‘ƒø¬º")
#define __TEST_DIR "./test_dir"
#endif

    tt_dremove(__TEST_DIR, TT_DRM_RECURSIVE);

    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dread(&dir, 0, &de);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_TEST_CHECK_EQUAL(de.name[1], '.', "");
        TT_TEST_CHECK_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dread(&dir, 0, &de);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_TEST_CHECK_EQUAL(de.name[1], '.', "");
        TT_TEST_CHECK_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dopen(&dir2, __TEST_DIR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dread(&dir2, 0, &de);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_TEST_CHECK_EQUAL(de.name[1], '.', "");
        TT_TEST_CHECK_EQUAL(de.name[2], '\0', "");
    }

    // nothing else except . and ..
    ret = tt_dread(&dir, 0, &de);
    TT_TEST_CHECK_EQUAL(ret, TT_END, "");

    // could continue read
    ret = tt_dread(&dir2, 0, &de);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    TT_TEST_CHECK_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_TEST_CHECK_EQUAL(de.name[1], '.', "");
        TT_TEST_CHECK_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dread(&dir2, 0, &de);
    TT_TEST_CHECK_EQUAL(ret, TT_END, "");

    ret = tt_dremove(__TEST_DIR, 0);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dclose(&dir);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    ret = tt_dclose(&dir2);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // remove subdirs
    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_dcreate(__TEST_DIR "/s1", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    ret = tt_dcreate(__TEST_DIR "/s1/s2", NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dremove(__TEST_DIR, TT_DRM_RECURSIVE);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_FAIL, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////

static tt_u32_t __fa1_err_line;

static tt_result_t __fa1_ret;
static tt_file_t __fa1_f;
static tt_file_t __fa1_f2;
static tt_file_t *__fa1_fp;

#if TT_ENV_OS_IS_IOS
#define __fa1_fname "../tmp/a1test"
#else
#define __fa1_fname "a1test"
#endif

#define __fa1_fwrite1_num 100
#define __fa1_fwrite2_num 100
static tt_atomic_s32_t __fa1_num;

static tt_spinlock_t write_num_lock;
static tt_s64_t write_num;
static tt_s64_t gen_num;

#define __fa1_read_num 11000
static tt_spinlock_t read_num_lock;
static tt_s64_t read_num;

static int _fa1_mode;

static void __fa1_on_fseek(IN tt_file_t *file,
                           IN tt_u32_t whence,
                           IN tt_s64_t distance,
                           IN tt_faioctx_t *aioctx,
                           IN tt_u64_t position);
static void __fa1_on_fread2(IN tt_file_t *file,
                            IN tt_blob_t *buf_array,
                            IN tt_u32_t buf_num,
                            IN tt_u64_t position,
                            IN tt_faioctx_t *aioctx,
                            IN tt_u32_t read_len);

static void __fa1_on_fwrite2(IN tt_file_t *file,
                             IN tt_blob_t *buf_array,
                             IN tt_u32_t buf_num,
                             IN tt_u64_t position,
                             IN tt_faioctx_t *aioctx,
                             IN tt_u32_t write_len)
{
    static int last_n;
    int n = (int)(tt_ptrdiff_t)aioctx->cb_param;
    tt_u32_t i = 0;
    tt_u32_t tlen = 0;

    for (i = 0; i < buf_num; ++i) {
        tlen += buf_array[i].len;
    }
    // as it's caused by write_full
    if (tlen != write_len) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    write_num += tlen;

    for (i = 0; i < buf_num; ++i)
        tt_mem_free(buf_array[i].addr);

    if (TT_OK(aioctx->result))
        TT_ASSERT(write_len > 0);
    else
        TT_ASSERT(write_len == 0);

    if (!TT_OK(aioctx->result)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (n > 0) {
        if (n != (last_n + 1)) {
            __fa1_ret = TT_FAIL;
            __fa1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
    last_n = n;

    if (n == (__fa1_fwrite1_num - 1)) {
        tt_result_t ret;

        ret = tt_fseek_async(&__fa1_f, TT_FPOS_BEGIN, 0, __fa1_on_fseek, NULL);
        if (ret != TT_SUCCESS) {
            __fa1_ret = TT_FAIL;
            __fa1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        TT_ASSERT(_fa1_mode == 1);
        {
            i = 0;
            while (i <= __fa1_fwrite1_num) {
                tt_blob_t tbuf[8];
                int j, j2 = 1; // rand()%8 + 1;

                for (j = 0; j < j2; ++j) {
                    char *buf = (char *)tt_mem_alloc(100);
                    tbuf[j].addr = (tt_u8_t *)buf;
                    tbuf[j].len = 100;
                }

                if (1 /*i%2*/)
                    ret = tt_fread_async(&__fa1_f,
                                         tbuf,
                                         j2,
                                         TT_FPOS_NULL,
                                         __fa1_on_fread2,
                                         (void *)(tt_ptrdiff_t)i);
                else
                    ret = tt_fread_async(&__fa1_f,
                                         tbuf,
                                         j2,
                                         i * 100,
                                         __fa1_on_fread2,
                                         (void *)(tt_ptrdiff_t)i);

                if (ret != TT_SUCCESS) {
                    __fa1_ret = TT_FAIL;
                    __fa1_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }

                ++i;
            }
        }
    }
}

static void __fa1_on_fclose(IN tt_file_t *file, IN tt_faioctx_t *aioctx)
{
    // all done
    {
        __fa1_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __fa1_on_fseek(IN tt_file_t *file,
                    IN tt_u32_t whence,
                    IN tt_s64_t distance,
                    IN tt_faioctx_t *aioctx,
                    IN tt_u64_t position)
{
    TT_DEBUG("position: %d", position);

    if (!TT_OK(aioctx->result)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __fa1_on_fread(IN tt_file_t *file,
                           IN tt_blob_t *buf_array,
                           IN tt_u32_t buf_num,
                           IN tt_u64_t position,
                           IN tt_faioctx_t *aioctx,
                           IN tt_u32_t read_len);
static void __fa1_on_fread2(IN tt_file_t *file,
                            IN tt_blob_t *buf_array,
                            IN tt_u32_t buf_num,
                            IN tt_u64_t position,
                            IN tt_faioctx_t *aioctx,
                            IN tt_u32_t read_len);

static void __fa1_on_fwrite(IN tt_file_t *file,
                            IN tt_blob_t *buf_array,
                            IN tt_u32_t buf_num,
                            IN tt_u64_t position,
                            IN tt_faioctx_t *aioctx,
                            IN tt_u32_t write_len)
{
    int n = 0;
    tt_u32_t i;

    for (i = 0; i < buf_num; ++i)
        tt_mem_free(buf_array[i].addr);

    if (TT_OK(aioctx->result))
        TT_ASSERT(write_len > 0);
    else
        TT_ASSERT(write_len == 0);

    if (!TT_OK(aioctx->result)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    tt_spinlock_acquire(&write_num_lock);
    write_num += write_len;
    tt_spinlock_release(&write_num_lock);

    if (aioctx->cb_param == 0) {
        if (tt_atomic_s32_inc(&__fa1_num) ==
            __fa1_fwrite1_num * __fa1_fwrite2_num) {
            tt_result_t ret;

            ret = tt_fseek_async(&__fa1_f,
                                 TT_FPOS_BEGIN,
                                 0,
                                 __fa1_on_fseek,
                                 NULL);
            if (ret != TT_SUCCESS) {
                __fa1_ret = TT_FAIL;
                __fa1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            TT_ASSERT(_fa1_mode == 0);
            {
                while (i++ < 22000) {
                    tt_blob_t tbuf[8];
                    int j, j2 = rand() % 8 + 1;

                    for (j = 0; j < j2; ++j) {
                        char *buf = (char *)tt_mem_alloc(200 + 1);
                        tbuf[j].addr = (tt_u8_t *)buf;
                        tbuf[j].len = 201;
                    }

                    if (i % 2)
                        ret = tt_fread_async(&__fa1_f,
                                             tbuf,
                                             j2,
                                             TT_FPOS_NULL,
                                             __fa1_on_fread,
                                             (void *)(tt_ptrdiff_t)i);
                    else
                        ret = tt_fread_async(&__fa1_f,
                                             tbuf,
                                             j2,
                                             TT_FPOS_NULL,
                                             __fa1_on_fread,
                                             (void *)(tt_ptrdiff_t)i);

                    if (ret != TT_SUCCESS) {
                        __fa1_ret = TT_FAIL;
                        __fa1_err_line = __LINE__;
                        tt_evc_exit(TT_LOCAL_EVC);
                        return;
                    }
                }
            }


            ret = tt_fclose_async(&__fa1_f, __fa1_on_fclose, NULL);

            // can not close
            ret = tt_fclose_async(&__fa1_f, __fa1_on_fclose, NULL);
            if (ret == TT_PROCEEDING) {
                tt_evc_exit(TT_LOCAL_EVC);
                __fa1_err_line = __LINE__;
                return;
            }
            // can not seek
            ret = tt_fseek_async(file, TT_FPOS_BEGIN, 0, __fa1_on_fseek, NULL);
            if (ret == TT_PROCEEDING) {
                tt_evc_exit(TT_LOCAL_EVC);
                __fa1_err_line = __LINE__;
                return;
            }
            // can not read
            do {
                tt_blob_t tbuf;
                tbuf.addr = (tt_u8_t *)&ret;
                tbuf.len = sizeof(ret);
                ret = tt_fread_async(&__fa1_f,
                                     &tbuf,
                                     1,
                                     TT_FPOS_NULL,
                                     __fa1_on_fread,
                                     NULL);
                if (ret == TT_PROCEEDING) {
                    tt_evc_exit(TT_LOCAL_EVC);
                    __fa1_err_line = __LINE__;
                    return;
                }
            } while (0);
            // can not write
            do {
                tt_blob_t tbuf;
                tbuf.addr = (tt_u8_t *)&ret;
                tbuf.len = sizeof(ret);
                ret = tt_fwrite_async(&__fa1_f,
                                      &tbuf,
                                      1,
                                      TT_FPOS_NULL,
                                      __fa1_on_fwrite,
                                      NULL);
                if (ret == TT_PROCEEDING) {
                    tt_evc_exit(TT_LOCAL_EVC);
                    __fa1_err_line = __LINE__;
                    return;
                }
            } while (0);

            return;
        }
        return;
    }

    srand((tt_u32_t)time(NULL));

    while (n++ < __fa1_fwrite2_num) {
        tt_blob_t tbuf[8];
        int k, k2 = rand() % 8 + 1;
        tt_result_t ret;

        for (k = 0; k < k2; ++k) {
            char *buf1 = (char *)tt_mem_alloc(101);
            int len, i;

            if (!buf1) {
                __fa1_ret = TT_FAIL;
                __fa1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            len = rand() % 100;
            for (i = 0; i < len; ++i)
                buf1[i] = '0' + (i % 10);
            buf1[len] = 0;

            tbuf[k].addr = (tt_u8_t *)buf1;
            tbuf[k].len = len + 1;

            tt_spinlock_acquire(&write_num_lock);
            gen_num += tbuf[k].len;
            tt_spinlock_release(&write_num_lock);
        }

        ret = tt_fwrite_async(file,
                              tbuf,
                              k2,
                              TT_FPOS_NULL,
                              __fa1_on_fwrite,
                              (void *)0);
        if (ret != TT_SUCCESS) {
            __fa1_ret = TT_FAIL;
            __fa1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

static void __fa1_on_fopen(IN tt_file_t *file,
                           IN const tt_char_t *path,
                           IN tt_u32_t flag,
                           IN tt_file_attr_t *attr,
                           IN tt_faioctx_t *aioctx)
{
    int n = 0;

    if (!TT_OK(aioctx->result)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    srand((tt_u32_t)time(NULL));

    if (_fa1_mode == 0) {
        while (n++ < __fa1_fwrite1_num) {
            tt_blob_t tbuf[8];
            int k, k2 = rand() % 8 + 1;
            tt_result_t ret;

            for (k = 0; k < k2; ++k) {
                char *buf1 = (char *)tt_mem_alloc(101);
                int len, i;

                if (!buf1) {
                    __fa1_ret = TT_FAIL;
                    __fa1_err_line = __LINE__;
                    tt_evc_exit(TT_LOCAL_EVC);
                    return;
                }

                len = rand() % 100;
                for (i = 0; i < len; ++i)
                    buf1[i] = '0' + (i % 10);
                buf1[len] = 0;

                tbuf[k].addr = (tt_u8_t *)buf1;
                tbuf[k].len = len + 1;

                tt_spinlock_acquire(&write_num_lock);
                gen_num += tbuf[k].len;
                tt_spinlock_release(&write_num_lock);
            }

            ret = tt_fwrite_async(file,
                                  tbuf,
                                  k2,
                                  TT_FPOS_NULL,
                                  __fa1_on_fwrite,
                                  (void *)1);
            if (ret != TT_SUCCESS) {
                __fa1_ret = TT_FAIL;
                __fa1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    } else {
        while (n < __fa1_fwrite1_num) {
            int len = 100, i;
            char *buf = (char *)tt_mem_alloc(len);
            tt_result_t ret;
            tt_blob_t tbuf[1];

            if (!buf) {
                __fa1_ret = TT_FAIL;
                __fa1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            for (i = 0; i < len; ++i)
                buf[i] = i;

            tbuf[0].addr = (tt_u8_t *)buf;
            tbuf[0].len = len;

            tt_spinlock_acquire(&write_num_lock);
            gen_num += len;
            tt_spinlock_release(&write_num_lock);

            if (rand() % 2) {
                ret = tt_fwrite_async(file,
                                      tbuf,
                                      1,
                                      TT_FPOS_NULL,
                                      __fa1_on_fwrite2,
                                      (void *)(tt_ptrdiff_t)n);
            } else {
                ret = tt_fwrite_async(file,
                                      tbuf,
                                      1,
                                      n * 100,
                                      __fa1_on_fwrite2,
                                      (void *)(tt_ptrdiff_t)n);
            }
            if (ret != TT_SUCCESS) {
                __fa1_ret = TT_FAIL;
                __fa1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            ++n;
        }
    }
}

static void __fa1_on_fcreate(IN const tt_char_t *path,
                             IN tt_u32_t flag,
                             IN tt_file_attr_t *attr,
                             IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;
    tt_u32_t flag1 = TT_FO_RDWR;

    if (!TT_OK(aioctx->result)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    flag1 |= TT_FO_APPEND;
    ret = tt_fopen_async(&__fa1_f, path, flag1, NULL, __fa1_on_fopen, NULL);
    if (ret != TT_SUCCESS) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __fa1_on_fremove(IN const tt_char_t *path, IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    // we don't care result of fremove

    ret = tt_fcreate_async(path, 0, NULL, __fa1_on_fcreate, NULL);
    if (ret != TT_SUCCESS) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __fa1_on_fread2(IN tt_file_t *file,
                            IN tt_blob_t *buf_array,
                            IN tt_u32_t buf_num,
                            IN tt_u64_t position,
                            IN tt_faioctx_t *aioctx,
                            IN tt_u32_t read_len)
{
    int i = (int)(tt_ptrdiff_t)aioctx->cb_param;
    tt_u32_t j;
    tt_bool_t exit_evc = TT_FALSE;

    if (TT_OK(aioctx->result))
        TT_ASSERT(read_len > 0);
    else
        TT_ASSERT(read_len == 0);

    read_num += read_len;

    if (!TT_OK(aioctx->result) && (i < (__fa1_fwrite1_num))) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        goto out;
    }

    if ((i < (__fa1_fwrite1_num)) && (aioctx->result == TT_END)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        goto out;
    }

    if ((i >= (__fa1_fwrite1_num))) {
        if (aioctx->result != TT_END) {
            __fa1_ret = TT_FAIL;
            __fa1_err_line = __LINE__;
            goto out;
        } else {
            __fa1_ret = TT_SUCCESS;
            exit_evc = TT_TRUE;
            goto out;
        }
    }

    // parsing
    for (j = 0; j < buf_num; ++j) {
        tt_u8_t *buf = buf_array[j].addr;

        if (buf_array[j].len != 100) {
            __fa1_ret = TT_FAIL;
            __fa1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            goto out;
        }

        i = 0;
        while ((tt_u32_t)i < buf_array[j].len) {
            if (i != buf[i]) {
                __fa1_ret = TT_FAIL;
                __fa1_err_line = __LINE__;
                goto out;
            }

            ++i;
        }
    }

out:
    for (j = 0; j < buf_num; ++j) {
        tt_mem_free(buf_array[j].addr);
    }
    if (exit_evc)
        tt_evc_exit(TT_LOCAL_EVC);
    return;
}

static void __fa1_on_fread(IN tt_file_t *file,
                           IN tt_blob_t *buf_array,
                           IN tt_u32_t buf_num,
                           IN tt_u64_t position,
                           IN tt_faioctx_t *aioctx,
                           IN tt_u32_t read_len)
{
    int i = 0;
    tt_u32_t k = 0, n = 0;
    char prev, cur;
    tt_u32_t tlen = 0;

    for (k = 0; k < buf_num; ++k) {
        tlen += buf_array[k].len;
    }

    if (TT_OK(aioctx->result))
        TT_ASSERT(read_len > 0);
    else
        TT_ASSERT(read_len == 0);

    if (!TT_OK(aioctx->result) && (aioctx->result != TT_END)) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    tt_spinlock_acquire(&read_num_lock);
    read_num += read_len;
    tt_spinlock_release(&read_num_lock);

    // if (((tt_u32_t)aioctx->cb_param == (__fa1_read_numTT_FPOS_NULL)))
    //    {tt_sem_release(&__fa1_sem);goto out;}

    if (aioctx->result == TT_END) {
        goto out;
    }

    // parsing
    TT_ASSERT(read_len <= tlen);
    if (read_len == 0)
        goto out;

    n = 0;
    for (k = 0; k < buf_num; ++k) {
        tt_u8_t *buf = buf_array[k].addr;

        i = 0;
        prev = buf[i++];
        ++n;
        while (((tt_u32_t)i < buf_array[k].len) && (n++ < read_len)) {
            cur = buf[i];

            if (prev == 0) {
                if ((cur != 0) && (cur != '0'))
                    __fa1_ret = TT_FAIL;
            } else if (prev < '9') {
                if ((cur != (prev + 1)) && (cur != 0))
                    __fa1_ret = TT_FAIL;
            } else if ((cur != '0') && (cur != 0))
                __fa1_ret = TT_FAIL;

            prev = cur;
            ++i;
        }
    }

out:
    for (k = 0; k < buf_num; ++k) {
        tt_mem_free(buf_array[k].addr);
    }
    return;
}

tt_result_t __fs_faio_basic_on_init(IN struct tt_evcenter_s *evc,
                                    IN void *on_init_param)
{
    tt_result_t ret;

    ret = tt_fremove_async(__fa1_fname, __fa1_on_fremove, NULL);
    if (ret != TT_SUCCESS) {
        __fa1_ret = TT_FAIL;
        __fa1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_faio_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    ret = tt_spinlock_create(&write_num_lock, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    write_num = 0;
    gen_num = 0;

    ret = tt_spinlock_create(&read_num_lock, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    read_num = 0;

    _fa1_mode = 0;
    __fa1_ret = TT_FAIL;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __fs_faio_basic_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__fa1_ret, TT_SUCCESS, "");
    TT_INFO("write_num: %d, read_num: %d", write_num, read_num);
    TT_TEST_CHECK_EQUAL(write_num, read_num, "");
    TT_INFO("gen_num: %d, read_num: %d", gen_num, read_num);
    TT_TEST_CHECK_EQUAL(gen_num, read_num, "");

    // mode 1
    _fa1_mode = 1;
    __fa1_ret = TT_FAIL;

    write_num = 0;
    gen_num = 0;
    read_num = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __fs_faio_basic_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__fa1_ret, TT_SUCCESS, "");
    TT_INFO("write_num: %d, read_num: %d", write_num, read_num);
    TT_TEST_CHECK_EQUAL(write_num, read_num, "");
    TT_INFO("gen_num: %d, read_num: %d", gen_num, read_num);
    TT_TEST_CHECK_EQUAL(gen_num, read_num, "");

    // this time, evc is not exited from on_fclose
    ret = tt_fclose_async(&__fa1_f, NULL, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_IOS
#define __fa2_fname "../tmp/a2test"
#else
#define __fa2_fname "a2test"
#endif

static tt_result_t __fa2_ret;
static tt_u32_t __fa2_err_line;
static tt_file_t __fa2_f;
static tt_u32_t flag2 = TT_FO_RDWR;

#define __fa2_num1 100
#define __fa2_num2 10
#define __fa2_num3 10

static void __fa2_action(IN tt_file_t *file, int level);

static void __fa2_on_fclose(IN tt_file_t *file, IN tt_faioctx_t *aioctx)
{
    if (TT_OK(aioctx->result)) {
        __fa2_ret = TT_SUCCESS;
    }

    // all done
    tt_evc_exit(TT_LOCAL_EVC);
}

static void __fa2_on_fread(IN tt_file_t *file,
                           IN tt_blob_t *buf_array,
                           IN tt_u32_t buf_num,
                           IN tt_u64_t position,
                           IN tt_faioctx_t *aioctx,
                           IN tt_u32_t read_len)
{
    int level = (int)(tt_ptrdiff_t)aioctx->cb_param;
    tt_u32_t k;

    if (aioctx->result == TT_SUCCESS)
        TT_ASSERT(read_len > 0);
    else
        TT_ASSERT(read_len == 0);

    for (k = 0; k < buf_num; ++k) {
        tt_mem_free(buf_array[k].addr);
    }

    __fa2_action(file, level + 1);
}

static void __fa2_on_fwrite(IN tt_file_t *file,
                            IN tt_blob_t *buf_array,
                            IN tt_u32_t buf_num,
                            IN tt_u64_t position,
                            IN tt_faioctx_t *aioctx,
                            IN tt_u32_t write_len)
{
    int level = (int)(tt_ptrdiff_t)aioctx->cb_param;
    tt_u32_t k;

    if (aioctx->result == TT_SUCCESS)
        TT_ASSERT(write_len > 0);
    else
        TT_ASSERT(write_len == 0);

    for (k = 0; k < buf_num; ++k) {
        tt_mem_free(buf_array[k].addr);
    }

    __fa2_action(file, level + 1);
}

static void __fa2_fs_on_fseek(IN tt_file_t *file,
                              IN tt_u32_t whence,
                              IN tt_s64_t distance,
                              IN tt_faioctx_t *aioctx,
                              IN tt_u64_t position)
{
    int level = (int)(tt_ptrdiff_t)aioctx->cb_param;
    __fa2_action(file, level + 1);
}

static void __fa2_action(IN tt_file_t *file, int level)
{
    int i = 0, n;

    if (level == 0)
        n = __fa2_num1;
    else if (level == 1)
        n = __fa2_num2;
    else if (level == 2)
        n = __fa2_num3;
    else
        return;

    while (i++ < n) {
        int k, k2 = rand() % 8 + 1;
        tt_blob_t tbuf[8];

        int r = rand() % 10;
        tt_u32_t _size = (((rand() % 10) /*+ 1*/) * 100);
        // tt_u32_t _size = (level << 8) | i;

        if (r < 8) {
            for (k = 0; k < k2; ++k) {
                tt_u8_t *buf;

                _size = (((rand() % 10) + 1) * 100);
                buf = tt_mem_alloc(_size);
                if (buf == NULL)
                    return;

                tbuf[k].addr = buf;
                tbuf[k].len = _size;
            }
        }

        if (r == 0) {
            tt_fread_async(file,
                           tbuf,
                           k2,
                           TT_FPOS_NULL,
                           __fa2_on_fread,
                           (void *)(tt_ptrdiff_t)level);
        } else if (r == 1) {
            tt_fread_async(file,
                           tbuf,
                           k2,
                           TT_FPOS_NULL,
                           __fa2_on_fread,
                           (void *)(tt_ptrdiff_t)level);
        } else if (r == 2) {
            tt_fread_async(file,
                           tbuf,
                           k2,
                           (rand() % 100) * 100,
                           __fa2_on_fread,
                           (void *)(tt_ptrdiff_t)level);
        } else if (r == 3) {
            tt_fread_async(file,
                           tbuf,
                           k2,
                           (rand() % 100) * 100,
                           __fa2_on_fread,
                           (void *)(tt_ptrdiff_t)level);
        } else if (r == 4) {
            tt_fwrite_async(file,
                            tbuf,
                            k2,
                            TT_FPOS_NULL,
                            __fa2_on_fwrite,
                            (void *)(tt_ptrdiff_t)level);
        } else if (r == 5) {
            tt_fwrite_async(file,
                            tbuf,
                            k2,
                            TT_FPOS_NULL,
                            __fa2_on_fwrite,
                            (void *)(tt_ptrdiff_t)level);
        } else if (r == 6) {
            tt_fwrite_async(file,
                            tbuf,
                            k2,
                            (rand() % 100) * 100,
                            __fa2_on_fwrite,
                            (void *)(tt_ptrdiff_t)level);
        } else if (r == 7) {
            tt_fwrite_async(file,
                            tbuf,
                            k2,
                            (rand() % 100) * 100,
                            __fa2_on_fwrite,
                            (void *)(tt_ptrdiff_t)level);
        } else if (r == 8) {
            int r2 = rand() % 3;
            tt_u32_t whence;
            tt_s64_t distance = (rand() % 10000) - 5000;

            if (r2 == 0)
                whence = TT_FPOS_BEGIN;
            else if (r2 == 1)
                whence = TT_FPOS_CUR;
            else
                whence = TT_FPOS_END;

            tt_fseek_async(file,
                           whence,
                           distance,
                           __fa2_fs_on_fseek,
                           (void *)(tt_ptrdiff_t)level);
        } else if (r == 9) {
            tt_sleep((rand() % 5) * 10);
        }
    }
}

static void __fa2_on_fopen(IN tt_file_t *file,
                           IN const tt_char_t *path,
                           IN tt_u32_t flag,
                           IN tt_file_attr_t *attr,
                           IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;
    // int i;

    srand((tt_u32_t)time(NULL));

    if (!TT_OK(aioctx->result)) {
        TT_WARN("fopen async fail");

        ret = TT_FAIL;
        while (ret != TT_SUCCESS) {
            ret = tt_fopen_async(&__fa2_f,
                                 path,
                                 flag2,
                                 NULL,
                                 __fa2_on_fopen,
                                 NULL);
            if (ret != TT_SUCCESS) {
                TT_WARN("fopen sync fail");
            }
        }
        return;
    }

    __fa2_action(file, 0);

    tt_sleep((rand() % 5 + 10) * 1000);
    ret = TT_FAIL;
    while (ret != TT_SUCCESS) {
        ret = tt_fclose_async(&__fa2_f, __fa2_on_fclose, NULL);
        if (ret != TT_SUCCESS) {
            TT_WARN("fclose sync fail");
        }
    }
}

static void __fa2_on_fcreate(IN const tt_char_t *path,
                             IN tt_u32_t flag,
                             IN tt_file_attr_t *attr,
                             IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    if (!TT_OK(aioctx->result)) {
        TT_WARN("fcreate async fail");

        ret = TT_FAIL;
        while (ret != TT_SUCCESS) {
            ret = tt_fcreate_async(path, 0, NULL, __fa2_on_fcreate, NULL);
            if (ret != TT_SUCCESS) {
                TT_WARN("fcreate sync fail");
            }
        }
        return;
    }

    flag2 |= TT_FO_APPEND;

    ret = TT_FAIL;
    while (ret != TT_SUCCESS) {
        ret = tt_fopen_async(&__fa2_f, path, flag2, NULL, __fa2_on_fopen, NULL);
        if (ret != TT_SUCCESS) {
            TT_WARN("fopen sync fail");
        }
    }
}

static void __fa2_on_fremove(IN const tt_char_t *path, IN tt_faioctx_t *aioctx)
{
    tt_result_t ret = TT_FAIL;

    while (ret != TT_SUCCESS) {
        ret = tt_fcreate_async(path, 0, NULL, __fa2_on_fcreate, NULL);
        if (ret != TT_SUCCESS) {
            TT_WARN("fcreate sync fail");
        }
    }
}

tt_result_t __fa2_evc_on_init(IN struct tt_evcenter_s *evc,
                              IN void *on_init_param)
{
    tt_result_t ret;

    ret = tt_fremove_async(__fa2_fname, __fa2_on_fremove, NULL);
    if (ret != TT_SUCCESS) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_faio_excep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t attr;

    TT_TEST_CASE_ENTER()
    // test start

    write_num = 0;
    read_num = 0;

    __fa2_ret = TT_FAIL;

    tt_evc_attr_default(&attr);
    attr.on_init = __fa2_evc_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__fa2_ret, TT_SUCCESS, "");

    // TT_TEST_CHECK_EQUAL(write_num,read_num,"");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_IOS
#define __fa3_fname_c "../tmp/a3test_c"
#define __fa3_fname_s "../tmp/a3test_s"
#define __fa3_fname_ba "../tmp/a3test_ba"
#else
#define __fa3_fname_c "a3test_c"
#define __fa3_fname_s "a3test_s"
#define __fa3_fname_ba "a3test_ba"
#endif

static tt_result_t __fa3_ret;
static tt_u32_t __fa3_err_line;
static tt_file_t __fa3_f;
static tt_u32_t flag3 = TT_FO_RDWR;
static tt_u32_t __fa3_mode = 0;

#define __fa3_fwrite1_num 10000
#define __fa3_unit 50000

#ifdef __fa3_static_mem
static tt_u8_t __fa3_buf_array[__fa3_fwrite1_num][__fa3_unit];
#endif

static void __fa3_on_fclose(IN tt_file_t *file, IN tt_faioctx_t *aioctx)
{
    if (TT_OK(aioctx->result)) {
        __fa3_ret = TT_SUCCESS;
    }

    // all done
    tt_evc_exit(TT_LOCAL_EVC);
}

static void __fa3_on_fwrite_ba(IN tt_file_t *file,
                               IN tt_blob_t *buf_array,
                               IN tt_u32_t buf_num,
                               IN tt_u64_t position,
                               IN tt_faioctx_t *aioctx,
                               IN tt_u32_t write_len)
{
    tt_u32_t n = (tt_u32_t)(tt_ptrdiff_t)aioctx->cb_param;

    if ((buf_num != 1) || (buf_array[0].len != __fa3_unit)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

#ifndef __fa3_static_mem
    for (n = 0; n < buf_num; ++n)
        tt_mem_free(buf_array[n].addr);
#endif

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

    if (n < __fa3_fwrite1_num - 1) {
        return;
    }

    // all done
    tt_fclose_async(file, __fa3_on_fclose, NULL);
}

static void __fa3_on_fwrite_c(IN tt_file_t *file,
                              IN tt_blob_t *buf_array,
                              IN tt_u32_t buf_num,
                              IN tt_u64_t position,
                              IN tt_faioctx_t *aioctx,
                              IN tt_u32_t write_len)
{
    int n = (int)(tt_ptrdiff_t)aioctx->cb_param;

    if ((buf_num != 1) || (buf_array[0].len != __fa3_unit)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

    tt_mem_free(buf_array[0].addr);

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

    if (n == (__fa3_fwrite1_num - 1)) {
        // all done
        tt_fclose_async(file, __fa3_on_fclose, NULL);
    }
}

static void __fa3_on_fwrite_s(IN tt_file_t *file,
                              IN tt_blob_t *buf_array,
                              IN tt_u32_t buf_num,
                              IN tt_u64_t position,
                              IN tt_faioctx_t *aioctx,
                              IN tt_u32_t write_len)
{
    int n = (int)(tt_ptrdiff_t)aioctx->cb_param;

    if ((buf_num != 1) || (buf_array[0].len != __fa3_unit)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

    tt_mem_free(buf_array[0].addr);

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

    if (n == (__fa3_fwrite1_num - 1)) {
        // all done
        tt_fclose_async(file, __fa3_on_fclose, NULL);
    } else {
        char *buf = (char *)tt_mem_alloc(__fa3_unit);
        tt_result_t ret;
        tt_blob_t tbuf[1];

        if (!buf) {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
        tbuf[0].addr = (tt_u8_t *)buf;
        tbuf[0].len = __fa3_unit;

        ret = tt_fwrite_async(file,
                              tbuf,
                              1,
                              TT_FPOS_NULL,
                              __fa3_on_fwrite_s,
                              (void *)(tt_ptrdiff_t)(n + 1));
        if (ret != TT_SUCCESS) {
            tt_evc_exit(TT_LOCAL_EVC);
            __fa3_err_line = __LINE__;
            return;
        }
    }
}

static void __fa3_on_fopen(IN tt_file_t *file,
                           IN const tt_char_t *path,
                           IN tt_u32_t flag,
                           IN tt_file_attr_t *attr,
                           IN tt_faioctx_t *aioctx)
{
    int n = 0;

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }

    if (__fa3_mode == 1) {
        do {
            char *buf = (char *)tt_mem_alloc(__fa3_unit);
            tt_result_t ret;
            tt_blob_t tbuf[1];

            if (!buf) {
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
            tbuf[0].addr = (tt_u8_t *)buf;
            tbuf[0].len = __fa3_unit;

            ret = tt_fwrite_async(file,
                                  tbuf,
                                  1,
                                  n * __fa3_unit,
                                  __fa3_on_fwrite_c,
                                  (void *)(tt_ptrdiff_t)n);
            if (ret != TT_SUCCESS) {
                tt_evc_exit(TT_LOCAL_EVC);
                __fa3_err_line = __LINE__;
                return;
            }
        } while (++n < __fa3_fwrite1_num);
    } else if (__fa3_mode == 2) {
        static tt_blob_t tbuf[__fa3_fwrite1_num];
        tt_result_t ret;

        do {
#ifdef __fa3_static_mem
            tbuf[n].addr = __fa3_buf_array[n];
            tbuf[n].len = __fa3_unit;
#else
            char *buf = (char *)tt_mem_alloc(__fa3_unit);

            if (!buf) {
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            tbuf[n].addr = (tt_u8_t *)buf;
            tbuf[n].len = __fa3_unit;
#endif

            ret = tt_fwrite_async(file,
                                  &tbuf[n],
                                  1,
                                  TT_FPOS_NULL,
                                  __fa3_on_fwrite_ba,
                                  (void *)(tt_ptrdiff_t)n);
            if (ret != TT_SUCCESS) {
                tt_evc_exit(TT_LOCAL_EVC);
                __fa3_err_line = __LINE__;
                return;
            }
        } while (++n < __fa3_fwrite1_num);
    } else {
        char *buf = (char *)tt_mem_alloc(__fa3_unit);
        tt_result_t ret;
        tt_blob_t tbuf[1];

        if (!buf) {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
        tbuf[0].addr = (tt_u8_t *)buf;
        tbuf[0].len = __fa3_unit;

        ret = tt_fwrite_async(file,
                              tbuf,
                              1,
                              TT_FPOS_NULL,
                              __fa3_on_fwrite_s,
                              (void *)0);
        if (ret != TT_SUCCESS) {
            tt_evc_exit(TT_LOCAL_EVC);
            __fa3_err_line = __LINE__;
            return;
        }
    }
}

static void __fa3_on_fremove(IN const tt_char_t *path, IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    // flag3 |= TT_FO_APPEND;
    ret = tt_fopen_async(&__fa3_f,
                         path,
                         flag3 | TT_FO_CREAT | TT_FO_TRUNC,
                         NULL,
                         __fa3_on_fopen,
                         NULL);
    if (ret != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return;
    }
}

tt_result_t __fa3_on_init(IN struct tt_evcenter_s *evc, IN void *on_init_param)
{
    tt_result_t ret;

    ret = tt_fremove_async((tt_char_t *)on_init_param, __fa3_on_fremove, NULL);
    if (ret != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        __fa3_err_line = __LINE__;
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_faio_perf)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    tt_u32_t t1 = 0, t2 = 0, t3 = 0;
    tt_s64_t start, end;

    TT_TEST_CASE_ENTER()
    // test start

    write_num = 0;
    read_num = 0;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __fa3_on_init;

#if 1
    // mode 0
    __fa3_ret = TT_FAIL;
    __fa3_mode = 0;
    evc_attr.on_init_param = __fa3_fname_c;

    start = tt_time_ref();
    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    end = tt_time_ref();
    t1 = (tt_u32_t)tt_time_ref2ms((end - start));
#endif

#if 1
    // mode 1
    __fa3_ret = TT_FAIL;
    __fa3_mode = 1;
    evc_attr.on_init_param = __fa3_fname_s;

    start = tt_time_ref();
    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    end = tt_time_ref();
    t2 = (tt_u32_t)tt_time_ref2ms((end - start));

    TT_TEST_CHECK_EQUAL(__fa3_ret, TT_SUCCESS, "");
#endif

#if 0
    // mode 2
    __fa3_ret = TT_FAIL;
    __fa3_mode = 2;
    evc_attr.on_init_param = __fa3_fname_ba;

    start = tt_time_ref();
    ret = tt_evc_create(&evc, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    
    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    end = tt_time_ref();
    t3 = (tt_u32_t)tt_time_ref2ms((end - start), NULL);
    
    TT_TEST_CHECK_EQUAL(__fa3_ret, TT_SUCCESS, "");
#endif

    TT_RECORD_INFO("concurrent: %d ms, serialized: %d ms, buf array: %d",
                   t1,
                   t2,
                   t3);
    // TT_TEST_CHECK_EQUAL(write_num,read_num,"");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifdef __perf_vs
static uv_fs_t open_req;
static uv_fs_t write_req;
static int n;

static tt_sem_t __fa_vs_sem;

void on_close(uv_fs_t *req)
{
    uv_fs_req_cleanup(req);

    tt_sem_release_void(&__fa_vs_sem);
}

void on_write(uv_fs_t *req)
{
    uv_buf_t buffer;
    char *buf;

    uv_fs_req_cleanup(req);
    free(req->bufs[0].base);

    if (req->result < 0) {
        printf("write fail: %s", uv_strerror(req->result));
        return;
    }

    if (++n >= __fa3_fwrite1_num) {
        uv_fs_close(uv_default_loop(), req, open_req.result, on_close);
        return;
    }

    buf = (char *)malloc(__fa3_unit);
    if (!buf) {
        printf("oom");
        return;
    }

    buffer.base = buf;
    buffer.len = __fa3_unit;

    uv_fs_write(uv_default_loop(),
                &write_req,
                open_req.result,
                &buffer,
                1,
                TT_FPOS_NULL,
                on_write);
}

void on_open(uv_fs_t *req)
{
    uv_buf_t buffer;
    char *buf;

    // uv_fs_req_cleanup(req);

    buf = (char *)malloc(__fa3_unit);
    if (!buf) {
        printf("oom");
        return;
    }

    buffer.base = buf;
    buffer.len = __fa3_unit;

    if (open_req.result > 0) {
        uv_fs_write(uv_default_loop(),
                    &write_req,
                    open_req.result,
                    &buffer,
                    1,
                    TT_FPOS_NULL,
                    on_write);
    } else {
        printf("open fail: %s", uv_strerror(req->result));
    }
}

void on_unlink(uv_fs_t *req)
{
    uv_fs_req_cleanup(req);

    uv_fs_open(uv_default_loop(),
               &open_req,
               "uvtest1.txt",
               O_CREAT | O_RDWR | O_TRUNC,
               0,
               on_open);
}

tt_result_t __fa_vs_thread(IN void *param, tt_thread_t *thread)
{
    tt_result_t tt_ret;

    tt_ret = tt_sem_create(&__fa_vs_sem, 0, NULL);
    if (!TT_OK(tt_ret))
        return TT_FAIL;

    uv_fs_unlink(uv_default_loop(), &open_req, "uvtest1.txt", on_unlink);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    tt_ret = tt_sem_acquire(&__fa_vs_sem, TT_TIME_INFINITE);
    // all done
    if (!TT_OK(tt_ret))
        return TT_FAIL;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_faio_perf_vs)
{
    tt_result_t ret;
    tt_thread_t thread;

    tt_u32_t t1;
    tt_s64_t start, end;

    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()

    start = tt_time_ref();
    ret = tt_thread_create(&thread,
                           "",
                           __fa_vs_thread,
                           __fa3_fname_s,
                           NULL,
                           NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_thread_wait(&thread);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");
    end = tt_time_ref();
    t1 = (tt_u32_t)tt_time_ref2ms((end - start), NULL);

    TT_RECORD_INFO("time: %d", t1);

    TT_TEST_CASE_LEAVE()
}
#endif

///////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_IOS
#define __da1_path "../tmp/test_d_1"
#else
#define __da1_path "test_d_1"
#endif

static tt_dir_t __da1_dir;
static tt_result_t __da1_ret = TT_FAIL;
static tt_u32_t __da1_err_line;

static tt_dir_entry_t __da1_de[10];
static tt_bool_t __da1_dclose = TT_TRUE;

extern void __da1_on_dcreate(IN const tt_char_t *path,
                             IN tt_dir_attr_t *attr,
                             IN tt_faioctx_t *aioctx);

void __da1_on_dread(IN tt_dir_t *dir,
                    IN tt_u32_t flag,
                    IN tt_dir_entry_t *dentry_array,
                    IN tt_u32_t dentry_num,
                    IN tt_faioctx_t *aioctx)
{
    if ((dentry_num == 1) && !TT_OK(aioctx->result)) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if ((dentry_num == 4) && aioctx->result != TT_END) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (!__da1_dclose) {
        if (aioctx->result == TT_END) {
            tt_evc_exit(TT_LOCAL_EVC);
        }
    }
}

void __da1_on_dremove(IN const tt_char_t *path, IN tt_faioctx_t *aioctx)
{
    static tt_atomic_s32_t removed;
    tt_result_t ret;

    /*if (!TT_OK(aioctx->result)) {
     tt_sem_release(&__da1_sem); return;
    }*/

    if (tt_atomic_s32_inc(&removed) < 3) {
        return;
    }
    // all removed

    if (aioctx->cb_param == (void *)2) {
        ret = tt_dcreate_async(__da1_path, NULL, __da1_on_dcreate, (void *)0);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
        return;
    }

    // remove root dir
    ret = tt_dremove_async(__da1_path, 0, __da1_on_dremove, (void *)2);
    if (ret != TT_SUCCESS) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

void __da1_on_dclose(IN tt_dir_t *dir, IN tt_faioctx_t *aioctx)
{
    if (TT_OK(aioctx->result)) {
        __da1_ret = TT_SUCCESS;
    }

    tt_evc_exit(TT_LOCAL_EVC);
}

extern void __da1_on_dopen(IN tt_dir_t *dir,
                           IN const tt_char_t *path,
                           IN tt_dir_attr_t *attr,
                           IN tt_faioctx_t *aioctx);

void __da1_on_dcreate(IN const tt_char_t *path,
                      IN tt_dir_attr_t *attr,
                      IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    if (!TT_OK(aioctx->result)) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (aioctx->cb_param == (void *)0) {
        // subdir 1
        ret = tt_dcreate_async(__da1_path "/_1",
                               NULL,
                               __da1_on_dcreate,
                               (void *)1);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        // subdir 2
        tt_dcreate_async(__da1_path "/_2", NULL, __da1_on_dcreate, (void *)1);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        // subdir 3
        tt_dcreate_async(__da1_path "/_3", NULL, __da1_on_dcreate, (void *)1);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    } else if (aioctx->cb_param == (void *)1) {
        static tt_atomic_s32_t created;

        if (tt_atomic_s32_inc(&created) < 3) {
            return;
        }
        // 3 subdir are created

        ret = tt_dopen_async(&__da1_dir,
                             __da1_path,
                             NULL,
                             __da1_on_dopen,
                             (void *)1);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

void __da1_on_dopen(IN tt_dir_t *dir,
                    IN const tt_char_t *path,
                    IN tt_dir_attr_t *attr,
                    IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    if (!TT_OK(aioctx->result)) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    } else {
        // read 1
        ret = tt_dread_async(&__da1_dir, 0, NULL, 0, __da1_on_dread, NULL);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        // read 10
        ret = tt_dread_async(&__da1_dir, 0, __da1_de, 10, __da1_on_dread, NULL);
        if (ret != TT_SUCCESS) {
            __da1_err_line = __LINE__;
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }

        // close
        if (__da1_dclose) {
            ret = tt_dclose_async(&__da1_dir, __da1_on_dclose, NULL);
            if (ret != TT_SUCCESS) {
                __da1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }

            // can not close
            ret = tt_dclose_async(&__da1_dir, __da1_on_dclose, NULL);
            if (ret == TT_PROCEEDING) {
                __da1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
            // can not read
            ret = tt_dread_async(&__da1_dir, 0, NULL, 0, __da1_on_dread, NULL);
            if (ret == TT_PROCEEDING) {
                __da1_err_line = __LINE__;
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }
}

tt_result_t __da1_on_init(IN struct tt_evcenter_s *evc, IN void *on_init_param)
{
    tt_result_t ret;

    // remove subdir 1
    ret = tt_dremove_async(__da1_path "/_1", 0, __da1_on_dremove, (void *)1);
    if (ret != TT_SUCCESS) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return TT_FAIL;
    }

    // remove subdir 2
    ret = tt_dremove_async(__da1_path "/_2", 0, __da1_on_dremove, (void *)1);
    if (ret != TT_SUCCESS) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return TT_FAIL;
    }

    // remove subdir 3
    ret = tt_dremove_async(__da1_path "/_3", 0, __da1_on_dremove, (void *)1);
    if (ret != TT_SUCCESS) {
        __da1_err_line = __LINE__;
        tt_evc_exit(TT_LOCAL_EVC);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_daio_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_evcenter_t evc;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __da1_on_init;

    ret = tt_evc_create(&evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__da1_ret, TT_SUCCESS, "");

    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////////

static tt_result_t __dae_ret;

tt_result_t __dae_thread(IN tt_thread_t *thread, IN void *param)
{
    __dae_ret = TT_SUCCESS;
    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_dir_excep)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_thread_t *thread;

    TT_TEST_CASE_ENTER()
    // test start

    __dae_ret = TT_FAIL;
    thread = tt_thread_create("", __dae_thread, NULL, NULL);
    TT_TEST_CHECK_NOT_EQUAL(thread, NULL, "");

    ret = tt_thread_wait(thread);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__dae_ret, TT_SUCCESS, "");

    // TT_TEST_CHECK_EQUAL(write_num,read_num,"");

    // test end
    TT_TEST_CASE_LEAVE()
}

//////////////////////////////////////////////////////////////

#define __fatm_exit_signal 0x1234cccd

#if TT_ENV_OS_IS_IOS
#define __fatm_name "../tmp/__fatm_test"
#else
#define __fatm_name "__fatm_test"
#endif

#define __fatm_char_num 1000

static tt_result_t __fatm_ret;
static tt_file_t __fatm_file;
static tt_evcenter_t __fatm_evc;
static tt_char_t __fatm_char[__fatm_char_num];
static tt_char_t __fatm_char_read[__fatm_char_num];

static void __fatm_on_fclose(IN tt_file_t *file, IN tt_faioctx_t *aioctx)
{
    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    __fatm_ret = TT_SUCCESS;
    tt_evc_exit(TT_LOCAL_EVC);
}

static void __fatm_on_fread(IN tt_file_t *file,
                            IN tt_blob_t *buf_array,
                            IN tt_u32_t buf_num,
                            IN tt_u64_t position,
                            IN tt_faioctx_t *aioctx,
                            IN tt_u32_t read_len)
{
    tt_result_t ret;
    int idx = (int)(tt_ptrdiff_t)aioctx->cb_param;

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (idx < (__fatm_char_num - 1))
        return;

    if (memcmp(__fatm_char, __fatm_char_read, __fatm_char_num) != 0) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    ret = tt_fclose_async(file, __fatm_on_fclose, NULL);
    if (ret != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __fatm_on_fwrite(IN tt_file_t *file,
                             IN tt_blob_t *buf_array,
                             IN tt_u32_t buf_num,
                             IN tt_u64_t position,
                             IN tt_faioctx_t *aioctx,
                             IN tt_u32_t write_len)
{
    tt_result_t ret;
    tt_blob_t tbuf;
    int i, idx = (int)(tt_ptrdiff_t)aioctx->cb_param;

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    if (idx < (__fatm_char_num - 1))
        return;

    for (i = 0; i < __fatm_char_num; ++i) {
        tbuf.addr = (tt_u8_t *)&__fatm_char_read[i];
        tbuf.len = 1;
        ret = tt_fread_async(file,
                             &tbuf,
                             1,
                             i,
                             __fatm_on_fread,
                             (void *)(tt_ptrdiff_t)i);
        if (ret != TT_SUCCESS) {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

static void __fatm_on_fopen(IN tt_file_t *file,
                            IN const tt_char_t *path,
                            IN tt_u32_t flag,
                            IN tt_file_attr_t *attr,
                            IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;
    tt_blob_t tbuf;
    int i;

    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    for (i = 0; i < __fatm_char_num; ++i) {
        __fatm_char[i] = (tt_char_t)i;

        tbuf.addr = (tt_u8_t *)&__fatm_char[i];
        tbuf.len = 1;
        ret = tt_fwrite_async(file,
                              IN & tbuf,
                              1,
                              i,
                              __fatm_on_fwrite,
                              (void *)(tt_ptrdiff_t)i);
        if (ret != TT_SUCCESS) {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

static void __fatm_on_fcreate(IN const tt_char_t *path,
                              IN tt_u32_t flag,
                              IN tt_file_attr_t *attr,
                              IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    if (aioctx->result != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    ret = tt_fopen_async(&__fatm_file,
                         __fatm_name,
                         TT_FO_RDWR,
                         NULL,
                         __fatm_on_fopen,
                         NULL);
    if (ret != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static void __fatm_on_fremove(IN const tt_char_t *path, IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    ret = tt_fcreate_async(__fatm_name, 0, NULL, __fatm_on_fcreate, NULL);
    if (ret != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }
}

static tt_result_t __fatm_on_init(IN struct tt_evcenter_s *evc,
                                  IN void *on_init_param)
{
    tt_thread_ev_t *evn = NULL;
    tt_result_t ret = TT_FAIL;

    ret = tt_fremove_async(__fatm_name, __fatm_on_fremove, NULL);
    if (ret != TT_SUCCESS) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_faio_evc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __fatm_ret = TT_FAIL;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __fatm_on_init;

    ret = tt_evc_create(&__fatm_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&__fatm_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // ret = tt_evc_destroy(&__fatm_evc);
    // TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__fatm_ret, TT_SUCCESS, "");

    // TT_TEST_CHECK_EQUAL(write_num,read_num,"");

    // test end
    TT_TEST_CASE_LEAVE()
}

/////////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_IOS
#define __datm_name "../tmp/__datm_test"
#else
#define __datm_name "__datm_test"
#endif

#define __datm_exit_signal 0xf234cccd

static tt_result_t __datm_ret;
static tt_result_t __datm_err_line;
static tt_dir_t __datm_dir;
static tt_evcenter_t __datm_evc;

#define __datm_de_num 10
static tt_dir_entry_t __datm_de[__datm_de_num];

void __datm_on_dclose(IN tt_dir_t *dir, IN tt_faioctx_t *aioctx)
{
    if (!TT_OK(aioctx->result)) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    __datm_ret = TT_SUCCESS;
    tt_evc_exit(TT_LOCAL_EVC);
}

void __datm_on_dread(IN tt_dir_t *dir,
                     IN tt_u32_t flag,
                     IN tt_dir_entry_t *dentry_array,
                     IN tt_u32_t dentry_num,
                     IN tt_faioctx_t *aioctx)
{
    if ((aioctx->result != TT_SUCCESS) && (aioctx->result != TT_END)) {
        __datm_err_line = __LINE__;
        {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }

    if (aioctx->result == TT_END) {
#if 0
	    tt_result_t ret;
        ret = tt_dclose_async(dir, __datm_on_dclose, NULL);
        if (ret != TT_SUCCESS) {
            __datm_err_line = __LINE__;
            { tt_evc_exit(TT_LOCAL_EVC); return;}
        }
#else
        // to use dclose with NULL on_close
        __datm_ret = TT_SUCCESS;
        tt_evc_exit(TT_LOCAL_EVC);
#endif
    }
}

void __datm_on_dopen(IN tt_dir_t *dir,
                     IN const tt_char_t *path,
                     IN tt_dir_attr_t *attr,
                     IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    if (aioctx->result != TT_SUCCESS) {
        tt_evc_exit(TT_LOCAL_EVC);
        return;
    }

    // read 1
    ret = tt_dread_async(dir, 0, NULL, 1, __datm_on_dread, NULL);
    if (ret != TT_SUCCESS) {
        __datm_err_line = __LINE__;
        {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }

    // read some
    ret =
        tt_dread_async(dir, 0, __datm_de, __datm_de_num, __datm_on_dread, NULL);
    if (ret != TT_SUCCESS) {
        __datm_err_line = __LINE__;
        {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

void __datm_on_dcreate(IN const tt_char_t *path,
                       IN tt_dir_attr_t *attr,
                       IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    if (aioctx->result != TT_SUCCESS) {
        __datm_err_line = __LINE__;
        {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }

    if ((int)(tt_ptrdiff_t)aioctx->cb_param == 0) {
        ret = tt_dcreate_async(__datm_name "/subd1",
                               NULL,
                               __datm_on_dcreate,
                               (void *)(tt_ptrdiff_t)1);
        if (ret != TT_SUCCESS) {
            __datm_err_line = __LINE__;
            {
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }

        ret = tt_dcreate_async(__datm_name "/subd2",
                               NULL,
                               __datm_on_dcreate,
                               (void *)(tt_ptrdiff_t)1);
        if (ret != TT_SUCCESS) {
            __datm_err_line = __LINE__;
            {
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }

        ret = tt_dcreate_async(__datm_name "/subd3",
                               NULL,
                               __datm_on_dcreate,
                               (void *)(tt_ptrdiff_t)1);
        if (ret != TT_SUCCESS) {
            __datm_err_line = __LINE__;
            {
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    } else {
        static int _created = 0;

        if (++_created < 3)
            return;

        ret = tt_dopen_async(&__datm_dir,
                             __datm_name,
                             NULL,
                             __datm_on_dopen,
                             NULL);
        if (ret != TT_SUCCESS) {
            __datm_err_line = __LINE__;
            {
                tt_evc_exit(TT_LOCAL_EVC);
                return;
            }
        }
    }
}

void __datm_on_dremove(IN const tt_char_t *path, IN tt_faioctx_t *aioctx)
{
    tt_result_t ret;

    ret = tt_dcreate_async(__datm_name,
                           NULL,
                           __datm_on_dcreate,
                           (void *)(tt_ptrdiff_t)0);
    if (ret != TT_SUCCESS) {
        __datm_err_line = __LINE__;
        {
            tt_evc_exit(TT_LOCAL_EVC);
            return;
        }
    }
}

tt_result_t __datm_on_init(IN struct tt_evcenter_s *evc, IN void *on_init_param)
{
    tt_result_t ret;

    ret = tt_dremove_async(__datm_name,
                           TT_DRM_RECURSIVE,
                           __datm_on_dremove,
                           NULL);
    if (ret != TT_SUCCESS)
        return TT_FAIL;

    return TT_SUCCESS;
}

TT_TEST_ROUTINE_DEFINE(tt_unit_test_fs_daio_evc)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    tt_result_t ret;
    tt_evc_attr_t evc_attr;

    TT_TEST_CASE_ENTER()
    // test start

    __datm_ret = TT_FAIL;

    tt_evc_attr_default(&evc_attr);
    evc_attr.on_init = __datm_on_init;

    ret = tt_evc_create(&__datm_evc, TT_FALSE, &evc_attr);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    ret = tt_evc_wait(&__datm_evc);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    TT_TEST_CHECK_EQUAL(__datm_ret, TT_SUCCESS, "");

    ret = tt_dclose_async(&__datm_dir, NULL, NULL);
    TT_TEST_CHECK_EQUAL(ret, TT_SUCCESS, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
