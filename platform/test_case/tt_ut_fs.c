/*
 * import header files
 */
#include "tt_unit_test_case_config.h"
#include <unit_test/tt_unit_test.h>

#include <algorithm/tt_string_common.h>
#include <io/tt_file_system.h>
#include <io/tt_fpath.h>
#include <memory/tt_memory_alloc.h>
#include <os/tt_process.h>
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
TT_TEST_ROUTINE_DECLARE(case_fs_basic)
TT_TEST_ROUTINE_DECLARE(case_fs_open)
TT_TEST_ROUTINE_DECLARE(case_fs_rw)
TT_TEST_ROUTINE_DECLARE(case_fs_copy)
TT_TEST_ROUTINE_DECLARE(case_fs_multhread)
TT_TEST_ROUTINE_DECLARE(case_fs_consistency)

TT_TEST_ROUTINE_DECLARE(case_dir_basic)

// =========================================

static tt_string_t __wpath;
static tt_string_t __sc_fpath, __sc_fpath2;
static tt_string_t __sc_dpath, __sc_dpath1;

static void __fs_enter(void *enter_param)
{
#if TT_ENV_OS_IS_IOS
#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    tt_char_t *pwd = tt_current_path(TT_FALSE);
    tt_string_create(&__wpath, pwd, NULL);
    tt_free(pwd);

    tt_set_current_path("../tmp");
#else
    tt_char_t *s;

    s = getenv("HOME");
    if (s != NULL) {
        tt_string_init(&__wpath, NULL);
        tt_string_append(&__wpath, s);
        tt_string_append(&__wpath, "/Library/Caches");
        tt_set_current_path(tt_string_cstr(&__wpath));
        tt_string_destroy(&__wpath);
    }

    s = tt_current_path(TT_FALSE);
    tt_string_create(&__wpath, s, NULL);
    tt_free(s);
#endif
#elif TT_ENV_OS_IS_ANDROID
    tt_char_t *pwd = tt_current_path(TT_FALSE);
    tt_string_create(&__wpath, pwd, NULL);
    tt_free(pwd);

    tt_set_current_path("/data/data/com.titansdk.titansdkunittest/");
#endif
}

static void __fs_exit(void *enter_param)
{
#if TT_ENV_OS_IS_IOS || TT_ENV_OS_IS_ANDROID
    tt_set_current_path(tt_string_cstr(&__wpath));
    tt_string_destroy(&__wpath);
#endif
}

static void __fs_enter_consis(void *enter_param);

// === test case list ======================
TT_TEST_CASE_LIST_DEFINE_BEGIN(fs_case)

TT_TEST_CASE("case_fs_basic",
             "testing fs basic",
             case_fs_basic,
             NULL,
             __fs_enter,
             NULL,
             __fs_exit,
             NULL)
,

    TT_TEST_CASE("case_fs_open",
                 "testing fs open close",
                 case_fs_open,
                 NULL,
                 __fs_enter,
                 NULL,
                 __fs_exit,
                 NULL),

    TT_TEST_CASE("case_fs_rw",
                 "testing fs read write",
                 case_fs_rw,
                 NULL,
                 __fs_enter,
                 NULL,
                 __fs_exit,
                 NULL),

    TT_TEST_CASE("case_fs_copy",
                 "testing fs copy",
                 case_fs_copy,
                 NULL,
                 __fs_enter,
                 NULL,
                 __fs_exit,
                 NULL),

    TT_TEST_CASE("case_dir_basic",
                 "testing dir basic",
                 case_dir_basic,
                 NULL,
                 __fs_enter_consis,
                 NULL,
                 __fs_exit,
                 NULL),

    TT_TEST_CASE("case_fs_multhread",
                 "testing fs read write in multi thread",
                 case_fs_multhread,
                 NULL,
                 __fs_enter,
                 NULL,
                 __fs_exit,
                 NULL),

    TT_TEST_CASE("case_fs_consistency",
                 "testing fs read write consistency",
                 case_fs_consistency,
                 NULL,
                 __fs_enter,
                 NULL,
                 __fs_exit,
                 NULL),

    TT_TEST_CASE_LIST_DEFINE_END(fs_case)
    // =========================================

    TT_TEST_UNIT_DEFINE(TEST_UNIT_FS, 0, fs_case)

/*
 * interface implementation
 */


/*
TT_TEST_ROUTINE_DEFINE(case_fs_consistency)
{
    //tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);

    TT_TEST_CASE_ENTER()
    // test start

    // test end
    TT_TEST_CASE_LEAVE()
}
*/

#define APK_PATH "/data/data/com.titansdk.titansdkunittest/"
#define __SC_TEST_FILE "测试"
#define __SC_TEST_FILE2 "测试2"
#define __TEST_D1 "测试目录1"
#define __TEST_D2 "测试目录2"
#define __TEST_F3 "测试文件3"

        TT_TEST_ROUTINE_DEFINE(case_fs_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_file_t f;
    tt_fstat_t fstat;

    TT_TEST_CASE_ENTER()

    tt_fremove(__SC_TEST_FILE);

    ret = tt_fopen(&f, __SC_TEST_FILE, TT_FO_READ, NULL);
    TT_UT_EQUAL(ret, TT_E_NOEXIST, "");
    ret = tt_fstat_path(__SC_TEST_FILE, &fstat);
    TT_UT_EQUAL(ret, TT_E_NOEXIST, "");

    // create
    ret = tt_fcreate(__SC_TEST_FILE, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_fcreate(__SC_TEST_FILE, NULL);
    TT_UT_EQUAL(ret, TT_E_EXIST, "");

    ret = tt_fopen(&f, __SC_TEST_FILE, TT_FO_RDWR, NULL);
    TT_UT_SUCCESS(ret, "");

    {
        tt_date_t d;
        tt_fstat_t fstat;
        tt_date_init(&d, tt_g_local_tmzone);
        tt_date_now(&d);
        ret = tt_fstat(&f, &fstat);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_date_cmp_date(&fstat.created, &d), 0, "");
#if !TT_ENV_OS_IS_WINDOWS
        TT_UT_EQUAL(fstat.created.hour, d.hour, "");
        TT_UT_EQUAL(fstat.created.minute, d.minute, "");
#endif
        TT_UT_EQUAL(tt_date_cmp_date(&fstat.accessed, &d), 0, "");
        TT_UT_EQUAL(fstat.accessed.hour, d.hour, "");
        TT_UT_EQUAL(fstat.accessed.minute, d.minute, "");
        TT_UT_EQUAL(tt_date_cmp_date(&fstat.modified, &d), 0, "");
        TT_UT_EQUAL(fstat.modified.hour, d.hour, "");
        TT_UT_EQUAL(fstat.modified.minute, d.minute, "");
        TT_UT_EQUAL(fstat.size, 0, "");
        TT_UT_EQUAL(fstat.is_dir, TT_FALSE, "");
        TT_UT_EQUAL(fstat.is_file, TT_TRUE, "");
        TT_UT_EQUAL(fstat.is_link, TT_FALSE, "");
        TT_UT_EQUAL(fstat.is_usr_readable, TT_TRUE, "");
        TT_UT_EQUAL(fstat.is_usr_writable, TT_TRUE, "");
    }

#if 0
    ret = tt_ftrylock(&f, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ftrylock(&f, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    tt_funlock(&f);
    tt_funlock(&f);

    ret = tt_ftrylock(&f, TT_TRUE);
    TT_UT_SUCCESS(ret, "");
    tt_funlock(&f);
#endif

    {
        tt_u64_t len;

        ret = tt_ftrunc(&f, 0);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fseek(&f, TT_FSEEK_END, 0, &len);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(len, 0, "");

        ret = tt_ftrunc(&f, 111);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fseek(&f, TT_FSEEK_END, 0, &len);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(len, 111, "");

        ret = tt_ftrunc(&f, 10);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fseek(&f, TT_FSEEK_END, 0, &len);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(len, 10, "");
    }

    tt_fclose(&f);

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_SUCCESS(ret, "");
    tt_fremove(__SC_TEST_FILE2);
    tt_dremove(__SC_TEST_FILE2);

    // rename
    {
        TT_UT_EQUAL(tt_fs_exist(__SC_TEST_FILE), TT_FALSE, "");

        ret = tt_fcreate(__SC_TEST_FILE, NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fs_exist(__SC_TEST_FILE), TT_TRUE, "");

        ret = tt_fs_rename(__SC_TEST_FILE, __SC_TEST_FILE2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fs_exist(__SC_TEST_FILE), TT_FALSE, "");
        TT_UT_EQUAL(tt_fs_exist(__SC_TEST_FILE2), TT_TRUE, "");

        ret = tt_fremove(__SC_TEST_FILE2);
        TT_UT_SUCCESS(ret, "");
    }

    // create file with parent paths
    {
        tt_dremove(__TEST_D1);
        TT_UT_FALSE(tt_fs_exist(__TEST_D1), "");

        ret = tt_fcreate(__TEST_D1
                         "/"__TEST_D2
                         "/"__TEST_F3,
                         NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_TRUE(tt_fs_exist(__TEST_D1 "/"__TEST_D2
                                         "/"__TEST_F3),
                   "");

        tt_dremove(__TEST_D1
                   "/"__TEST_D2
                   "/");
        TT_UT_FALSE(tt_fs_exist(__TEST_D1 "/"__TEST_D2), "");

        ret = tt_fcreate(__TEST_D1
                         "/"__TEST_D2
                         "/"__TEST_F3,
                         NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_TRUE(tt_fs_exist(__TEST_D1 "/"__TEST_D2
                                         "/"__TEST_F3),
                   "");
    }

    // open file with parent paths
    {
        tt_dremove(__TEST_D1);
        TT_UT_FALSE(tt_fs_exist(__TEST_D1), "");

        ret = tt_fopen(&f,
                       __TEST_D1
                       "/"__TEST_D2
                       "/"__TEST_F3,
                       TT_FO_CREAT_DIR,
                       NULL);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_TRUE(tt_fs_exist(__TEST_D1 "/"__TEST_D2
                                         "/"__TEST_F3),
                   "");
        tt_fclose(&f);

        tt_dremove(__TEST_D1
                   "/"__TEST_D2
                   "/");
        TT_UT_TRUE(tt_fs_exist(__TEST_D1), "");
        TT_UT_FALSE(tt_fs_exist(__TEST_D1 "/"__TEST_D2
                                          "/"),
                    "");

        ret = tt_fopen(&f,
                       __TEST_D1
                       "/"__TEST_D2
                       "/"__TEST_F3,
                       TT_FO_CREAT_DIR,
                       NULL);
        TT_UT_EQUAL(ret, TT_SUCCESS, "");
        TT_UT_TRUE(tt_fs_exist(__TEST_D1 "/"__TEST_D2
                                         "/"__TEST_F3),
                   "");
        tt_fclose(&f);
    }

    {
        tt_date_t a, m;
        tt_fstat_t fst;

        ret = tt_fopen(&f, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_WRITE, NULL);
        TT_UT_SUCCESS(ret, "");

        tt_ftrunc(&f, 0);
        tt_fwrite(&f, (tt_u8_t *)"123", 3, NULL);

        ret = tt_futime(&f, NULL, NULL);
        TT_UT_SUCCESS(ret, "");

        tt_date_init(&a, tt_g_local_tmzone);
        tt_date_init(&m, tt_g_local_tmzone);

        tt_date_set(&a, 1980, TT_OCTOBER, 10, 1, 2, 3);
        // 2038 overflow
        tt_date_set(&m, 2010, TT_NOVEMBER, 30, 4, 5, 6);
        ret = tt_futime(&f, &a, &m);
        TT_UT_SUCCESS(ret, "");

        tt_fclose(&f);

        ret = tt_fstat_path(__SC_TEST_FILE, &fst);
        // TT_UT_EQUAL(tt_date_cmp(&a, &fst.accessed), 0, "");
        TT_UT_EQUAL(tt_date_cmp(&m, &fst.modified), 0, "");
    }

// do not test link on android
#if !TT_ENV_OS_IS_ANDROID
    {
        tt_u8_t *data;
        tt_u64_t n;
        tt_char_t name[1024];

        // link
        tt_fremove(__SC_TEST_FILE2);

        ret = tt_fs_link(__SC_TEST_FILE, __SC_TEST_FILE2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_TRUE(tt_fs_exist(__SC_TEST_FILE2), "");
        data = tt_fcontent(__SC_TEST_FILE2, &n);
        TT_UT_EQUAL(n, 3, "");
        TT_UT_MEMEQ(data, "123", 3, "");
        tt_free(data);

        ret = tt_fs_readlink(__SC_TEST_FILE2, name, sizeof(name));
        TT_UT_FAIL(ret, ""); // not a symlink

        ret = tt_fs_realpath(__SC_TEST_FILE2, name, sizeof(name));
        TT_UT_SUCCESS(ret, "");

        // hard link, still exist
        tt_fremove(__SC_TEST_FILE);
        data = tt_fcontent(__SC_TEST_FILE2, &n);
        TT_UT_EQUAL(n, 3, "");
        TT_UT_MEMEQ(data, "123", 3, "");
        tt_free(data);

        ret = tt_fs_link(__SC_TEST_FILE, "123");
        TT_UT_FAIL(ret, "");
    }
#endif

#if !TT_ENV_OS_IS_WINDOWS && !TT_ENV_OS_IS_ANDROID
    {
        tt_u8_t *data;
        tt_u64_t n;
        tt_char_t name[1024];

        // symlink
        ret = tt_fs_symlink(__SC_TEST_FILE2, __SC_TEST_FILE);
        TT_UT_SUCCESS(ret, "");
        data = tt_fcontent(__SC_TEST_FILE, &n);
        TT_UT_EQUAL(n, 3, "");
        TT_UT_MEMEQ(data, "123", 3, "");
        tt_free(data);

        ret = tt_fs_readlink(__SC_TEST_FILE, name, sizeof(name));
        TT_UT_SUCCESS(ret, "");
        TT_UT_STREQ(name, __SC_TEST_FILE2, "");

        ret = tt_fs_realpath(__SC_TEST_FILE2, name, sizeof(name));
        TT_UT_SUCCESS(ret, "");

        // sym link, not exist
        tt_fremove(__SC_TEST_FILE2);
        data = tt_fcontent(__SC_TEST_FILE, &n);
        TT_UT_NULL(data, "");

        // still readable?
        ret = tt_fs_readlink(__SC_TEST_FILE, name, sizeof(name));
        TT_UT_SUCCESS(ret, "");
        TT_UT_STREQ(name, __SC_TEST_FILE2, "");

        ret = tt_fs_realpath(__SC_TEST_FILE2, name, sizeof(name));
        TT_UT_FAIL(ret, "");
    }
#endif

    {
#if TT_ENV_OS_IS_IOS && (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __TT1 "../tmp/tttmp/1.xXXX"
#define __TT2 "../tmp/tttmp/XXXXX"
#define __TTD "../tmp/tttmp"
#else
#define __TT1 "tttmp/1.xXXX"
#define __TT2 "tttmp/XXXXX"
#define __TTD "tttmp"
#endif

        char path[20] = "";

        ret = tt_fcreate_temp(path, NULL);

        tt_strncpy(path, __TT1, sizeof(__TT1));
        ret = tt_fcreate_temp(path, NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_NSTREQ(path, __TT1, sizeof(__TT1) - 4, "");

        tt_strncpy(path, __TT2, sizeof(__TT2));
        ret = tt_fcreate_temp(path, NULL);
        TT_INFO("path: %s", path);
        TT_UT_SUCCESS(ret, "");

        tt_dremove(__TTD);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#if 0
TT_TEST_ROUTINE_DEFINE(case_fs_flock)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_file_t f;
    tt_process_t p;
    tt_char_t *arg[] = {"\"not care\"", "flock", __SC_TEST_FILE, "e", NULL};
    tt_u8_t r;

    TT_TEST_CASE_ENTER()

#if TT_ENV_OS_IS_IOS
    return TT_SUCCESS;
#endif

#if TT_ENV_OS_IS_WINDOWS
    return TT_SUCCESS;
#endif

    tt_fremove(__SC_TEST_FILE);
    tt_fcreate(__SC_TEST_FILE, NULL);

    ret = tt_fopen(&f, __SC_TEST_FILE, TT_FO_RDWR, NULL);
    TT_UT_SUCCESS(ret, "");

    // share lock first
    ret = tt_ftrylock(&f, TT_FALSE);
    TT_UT_SUCCESS(ret, "");
    ret = tt_ftrylock(&f, TT_FALSE);
    TT_UT_SUCCESS(ret, "");

    arg[3] = "ex";
#if 0
        {
			    tt_fpath_t fp;
    tt_fpath_init(&fp, TT_FPATH_AUTO);
    tt_fpath_set(&fp, tt_current_path(TT_TRUE));
    tt_fpath_to_dir(&fp);
    tt_fpath_set_filename(&fp,__SC_TEST_FILE);
    arg[2] = (tt_char_t*)tt_fpath_cstr(&fp);
        }
#endif
    ret = tt_process_create(&p, tt_process_path(NULL), arg, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_process_wait(&p, TT_TRUE, &r);
    TT_UT_SUCCESS(ret, "");
    // can not exclusive lock
    TT_UT_EQUAL(r, 1, "");

    arg[3] = "sh";
    ret = tt_process_create(&p, tt_process_path(NULL), arg, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_process_wait(&p, TT_TRUE, &r);
    TT_UT_SUCCESS(ret, "");
    // can share lock
    TT_UT_EQUAL(r, 0, "");

    tt_funlock(&f);

    // exclusive lock first
    ret = tt_ftrylock(&f, TT_TRUE);
    TT_UT_SUCCESS(ret, "");

    arg[3] = "ex";
    ret = tt_process_create(&p, tt_process_path(NULL), arg, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_process_wait(&p, TT_TRUE, &r);
    TT_UT_SUCCESS(ret, "");
    // can not exclusive lock
    TT_UT_EQUAL(r, 1, "");

    arg[3] = "sh";
    ret = tt_process_create(&p, tt_process_path(NULL), arg, NULL);
    TT_UT_SUCCESS(ret, "");
    ret = tt_process_wait(&p, TT_TRUE, &r);
    TT_UT_SUCCESS(ret, "");
    // can not share lock
    TT_UT_EQUAL(r, 1, "");

    tt_funlock(&f);

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_SUCCESS(ret, "");

    // test end
    TT_TEST_CASE_LEAVE()
}
#endif

TT_TEST_ROUTINE_DEFINE(case_fs_open)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_file_t tf, tf2;
    tt_result_t ret;
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(__SC_TEST_FILE);

    TT_UT_EQUAL(tt_fs_exist(__SC_TEST_FILE), TT_FALSE, "");

    // fail as it does not exist
    ret = tt_fopen(&tf, __SC_TEST_FILE, 0, NULL);
    TT_UT_EQUAL(ret, TT_E_NOEXIST, "");

    // open file
    ret = tt_fopen(&tf,
                   __SC_TEST_FILE,
                   TT_FO_CREAT | TT_FO_READ | TT_FO_WRITE,
                   NULL);
    TT_UT_SUCCESS(ret, "");

    TT_UT_EQUAL(tt_fs_exist(__SC_TEST_FILE), TT_TRUE, "");

    {
        tt_u8_t buf1[100] = "test1";
        tt_u8_t buf2[100] = {0};
        tt_u64_t d;

        ret =
            tt_fwrite(&tf, buf1, (tt_u32_t)strlen((const char *)buf1) + 1, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, (strlen((const char *)buf1) + 1), "");

        {
            tt_date_t d;
            tt_fstat_t fstat;
            tt_date_init(&d, tt_g_local_tmzone);
            tt_date_now(&d);
            ret = tt_fstat(&tf, &fstat);
            TT_UT_SUCCESS(ret, "");
            TT_UT_EQUAL(fstat.size, n, "");
            TT_UT_EQUAL(fstat.is_dir, TT_FALSE, "");
            TT_UT_EQUAL(fstat.is_file, TT_TRUE, "");
            TT_UT_EQUAL(fstat.is_link, TT_FALSE, "");
            TT_UT_EQUAL(fstat.is_usr_readable, TT_TRUE, "");
            TT_UT_EQUAL(fstat.is_usr_writable, TT_TRUE, "");
        }

        ret = tt_fwrite(&tf, buf1, 0, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, 0, "");

        tt_fseek(&tf, TT_FSEEK_BEGIN, 0, NULL);
        ret = tt_fread(&tf, buf2, 90, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, strlen((const char *)buf1) + 1, "");

        ret = tt_fread(&tf, buf2, 0, &n);
        TT_UT_FAIL(ret, "");

        ret = tt_fread(&tf, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_E_END, "");

        // write append
        ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_RDWR | TT_FO_APPEND, NULL);
        TT_UT_SUCCESS(ret, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, (tt_u32_t)strlen((const char *)buf1) + 1, "");

        tt_fseek(&tf2, TT_FSEEK_CUR, 0, &d);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(d, (tt_u32_t)strlen((const char *)buf1) + 1, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_E_END, "");

        ret =
            tt_fwrite(&tf2, buf1, (tt_u32_t)strlen((const char *)buf1) + 1, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, strlen((const char *)buf1) + 1, "");

        ret = tt_fwrite(&tf2, buf1, 0, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, 0, "");

        // even fseek begin, write still append data, so fpos should be
        // after 2 strings
        ret = tt_fseek(&tf2, TT_FSEEK_CUR, 0, &d);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(d, 2 * (strlen((const char *)buf1) + 1), "");

        d = ~0;
        ret = tt_fseek(&tf2,
                       TT_FSEEK_CUR,
                       -2 * (tt_s32_t)(strlen((const char *)buf1) + 1),
                       &d);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(d, 0, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, 2 * (strlen((const char *)buf1) + 1), "");

        ret = tt_fread(&tf2, buf2, 0, &n);
        TT_UT_FAIL(ret, "");

        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_E_END, "");

        ret = tt_fseek(&tf2, TT_FSEEK_END, -20000, NULL);
        TT_UT_FAIL(ret, "");
        ret = tt_fread(&tf2, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_E_END, "");

        tt_fclose(&tf2);
    }
    {
        tt_u8_t buf2[100] = {0};

        ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_TRUNC, NULL);
        TT_UT_SUCCESS(ret, "");

        // truncated
        ret = tt_fread(&tf, buf2, 90, &n);
        TT_UT_EQUAL(ret, TT_E_END, "");

        tt_fclose(&tf2);
    }

    tt_fclose(&tf);

    // create file exclusively, fail as file exist
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_EXCL, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // create and truncate
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_TRUNC, NULL);
    TT_UT_SUCCESS(ret, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_SUCCESS(ret, "");
    // now tf and tf2 could still be used

    // close to delete
    tt_fclose(&tf2);

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_fs_rw)
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
    TT_UT_SUCCESS(ret, "");

    // remove
    ret = tt_fremove(__SC_TEST_FILE);
    TT_UT_SUCCESS(ret, "");

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
            TT_UT_SUCCESS(ret, "");
            n += wn;

            if (tt_rand_u32() & 1) {
                ret = tt_fsync(&tf);
            } else {
                ret = tt_fdatasync(&tf);
            }
            TT_UT_SUCCESS(ret, "");
        }
        TT_UT_EXP(n == sizeof(buf), "");

        ret = tt_fseek(&tf, TT_FSEEK_BEGIN, 0, NULL);
        TT_UT_SUCCESS(ret, "");

        n = 0;
        while (n < sizeof(rbuf)) {
            tt_u32_t rn = 0;
            ret = tt_fread(&tf, rbuf + n, sizeof(rbuf) - n, &rn);
            TT_UT_SUCCESS(ret, "");
            n += rn;
        }
        TT_UT_EXP(n == sizeof(rbuf), "");

        TT_UT_EQUAL(tt_strncmp((const char *)buf,
                               (const char *)rbuf,
                               sizeof(buf)),
                    0,
                    "");
    }

    // close to delete
    tt_fclose(&tf);

    // open should fail
    ret = tt_fopen(&tf2, __SC_TEST_FILE, TT_FO_READ, NULL);
    TT_UT_EQUAL(ret, TT_E_NOEXIST, "");

    // test end
    TT_TEST_CASE_LEAVE()
}

TT_TEST_ROUTINE_DEFINE(case_dir_basic)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_dir_t dir, dir2;
    tt_result_t ret = TT_FAIL;

    tt_dirent_t de;

    TT_TEST_CASE_ENTER()
// test start

#if TT_ENV_OS_IS_IOS && (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)

#define __TEST_DIR "../tmp/test_dir"
#define __TEST_DIR2 "../tmp/test_dir2"

#elif TT_ENV_OS_IS_ANDROID

#define __TEST_DIR APK_PATH "test_dir"
#define __TEST_DIR2 APK_PATH "test_dir2"

#else

#define __TEST_DIR "./test_dir"
#define __TEST_DIR2 "./test_dir2"

#endif

#define __TEST_SUBDIR "一个子目录3"

    tt_dremove(__TEST_DIR);
    tt_dremove(__TEST_DIR2);

    TT_UT_EQUAL(tt_fs_exist(__TEST_DIR), TT_FALSE, "");

    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(tt_fs_exist(__TEST_DIR), TT_TRUE, "");
    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_E_EXIST, "");
    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_dread(&dir, &de);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dread(&dir, &de);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dopen(&dir2, __TEST_DIR, NULL);
    TT_UT_SUCCESS(ret, "");

    ret = tt_dread(&dir2, &de);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    // nothing else except . and ..
    ret = tt_dread(&dir, &de);
    TT_UT_EQUAL(ret, TT_E_END, "");

    // could continue read
    ret = tt_dread(&dir2, &de);
    TT_UT_SUCCESS(ret, "");
    TT_UT_EQUAL(de.name[0], '.', "");
    if (de.name[1] != '\0') {
        TT_UT_EQUAL(de.name[1], '.', "");
        TT_UT_EQUAL(de.name[2], '\0', "");
    }

    ret = tt_dread(&dir2, &de);
    TT_UT_EQUAL(ret, TT_E_END, "");

    ret = tt_dremove(__TEST_DIR);
    TT_UT_SUCCESS(ret, "");

    tt_dclose(&dir);

    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    tt_dclose(&dir2);

    // remove subdirs
    ret = tt_dcreate(__TEST_DIR, NULL);
    TT_UT_SUCCESS(ret, "");
    {
        tt_char_t s[256];
        tt_u32_t len = (tt_u32_t)tt_strlen(__TEST_DIR);
        tt_memcpy(s, __TEST_DIR, len);
        tt_memcpy(s + len, "/s1", sizeof("/s1"));
        ret = tt_dcreate(s, NULL);
        TT_UT_SUCCESS(ret, "");
    }
    {
        tt_char_t s[256];
        tt_u32_t len = (tt_u32_t)tt_strlen(__TEST_DIR);
        tt_memcpy(s, __TEST_DIR, len);
        tt_memcpy(s + len, "/s1/s2", sizeof("/s1/s2"));
        ret = tt_dcreate(s, NULL);
        TT_UT_SUCCESS(ret, "");
    }

    ret = tt_dremove(__TEST_DIR);
    TT_UT_SUCCESS(ret, "");

    ret = tt_dopen(&dir, __TEST_DIR, NULL);
    TT_UT_EQUAL(ret, TT_FAIL, "");

    // rename
    {
        TT_UT_EQUAL(tt_fs_exist(__TEST_DIR), TT_FALSE, "");

        ret = tt_dcreate(__TEST_DIR, NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fs_exist(__TEST_DIR), TT_TRUE, "");

        ret = tt_fs_rename(__TEST_DIR, __TEST_DIR2);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(tt_fs_exist(__TEST_DIR), TT_FALSE, "");
        TT_UT_EQUAL(tt_fs_exist(__TEST_DIR2), TT_TRUE, "");

#if !TT_ENV_OS_IS_WINDOWS
        ret = tt_fs_symlink(__TEST_DIR2, __TEST_DIR);
        TT_UT_SUCCESS(ret, "");
#endif

        ret = tt_dremove(__TEST_DIR2);
        TT_UT_SUCCESS(ret, "");
    }

    {
        tt_dremove(__TEST_DIR);
        TT_UT_FALSE(tt_fs_exist(__TEST_DIR), "");

        ret = tt_dcreate(__TEST_DIR "/" __TEST_SUBDIR "/" __TEST_SUBDIR
                                    "/" __TEST_SUBDIR,
                         NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_TRUE(tt_fs_exist(__TEST_DIR "/" __TEST_SUBDIR "/" __TEST_SUBDIR
                                          "/" __TEST_SUBDIR),
                   "");

        tt_dremove(__TEST_DIR "/" __TEST_SUBDIR "/" __TEST_SUBDIR);
        TT_UT_TRUE(tt_fs_exist(__TEST_DIR), "");
        TT_UT_TRUE(tt_fs_exist(__TEST_DIR "/" __TEST_SUBDIR), "");
        TT_UT_FALSE(tt_fs_exist(__TEST_DIR "/" __TEST_SUBDIR "/" __TEST_SUBDIR),
                    "");

        ret = tt_dcreate(__TEST_DIR "/" __TEST_SUBDIR "/" __TEST_SUBDIR
                                    "/" __TEST_SUBDIR "/",
                         NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_TRUE(tt_fs_exist(__TEST_DIR "/" __TEST_SUBDIR "/" __TEST_SUBDIR
                                          "/" __TEST_SUBDIR),
                   "");
    }

    {
#if TT_ENV_OS_IS_IOS && (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)

#define __DTT1 "../tmp/dtttmp/1.xXXX"
#define __DTT2 "../tmp/dtttmp/XXXXX"
#define __DTTD "../tmp/dtttmp"

#elif TT_ENV_OS_IS_ANDROID

#define __DTT1 APK_PATH "dtttmp/1.xXXX"
#define __DTT2 APK_PATH "dtttmp/XXXXX"
#define __DTTD APK_PATH "dtttmp"

#else

#define __DTT1 "dtttmp/1.xXXX"
#define __DTT2 "dtttmp/XXXXX"
#define __DTTD "dtttmp"

#endif

        char path[128] = "";

        ret = tt_dcreate_temp(path, NULL);

        tt_strncpy(path, __DTT1, sizeof(__DTT1));
        ret = tt_dcreate_temp(path, NULL);
        TT_UT_SUCCESS(ret, "");
        TT_UT_NSTREQ(path, __DTT1, sizeof(__DTT1) - 4, "");

        tt_strncpy(path, __DTT2, sizeof(__DTT2));
        ret = tt_dcreate_temp(path, NULL);
        TT_INFO("path: %s", path);
        TT_UT_SUCCESS(ret, "");

        tt_dremove(__DTTD);
    }

    // test end
    TT_TEST_CASE_LEAVE()
}

#if TT_ENV_OS_IS_IOS && (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
#define __ut_fname "../tmp/a.txt"
#elif TT_ENV_OS_IS_ANDROID
#define __ut_fname APK_PATH "a.txt"
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

TT_TEST_ROUTINE_DEFINE(case_fs_multhread)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t task[__task_num];
    tt_result_t ret;
    tt_u32_t i, j;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(__ut_fname);
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
#if (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
static const tt_char_t *fname[4] = {"../tmp/1.txt",
                                    "../tmp/2.txt",
                                    "../tmp/3.txt",
                                    "../tmp/4.txt"};
#else
static const tt_char_t *fname[4] = {0};
#endif
#elif TT_ENV_OS_IS_ANDROID
static const tt_char_t *fname[4] = {APK_PATH "1.txt",
                                    APK_PATH "2.txt",
                                    APK_PATH "3.txt",
                                    APK_PATH "4.txt"};
#else
static const tt_char_t *fname[4] = {"1.txt", "2.txt", "3.txt", "4.txt"};
#endif

static tt_bool_t __fb_end;

void __fs_enter_consis(void *enter_param)
{
#if TT_ENV_OS_IS_IOS && !(TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)
    static tt_bool_t done = TT_FALSE;
    tt_char_t *s;

    if (done) {
        return;
    }

    __fs_enter(NULL);

    s = getenv("HOME");
    if (s != NULL) {
        tt_string_t str;
        tt_char_t *path;
        tt_u32_t len;

        tt_string_init(&str, NULL);
        tt_string_append(&str, s);
        tt_string_append(&str, "/Library/Caches");
        len = tt_string_len(&str);

        path = tt_malloc(len + sizeof("/1.txt"));
        if (path != NULL) {
            tt_memcpy(path, tt_string_cstr(&str), len);
            tt_memcpy(path + len, "/1.txt", sizeof("/1.txt"));
            fname[0] = path;
        }

        path = tt_malloc(len + sizeof("/2.txt"));
        if (path != NULL) {
            tt_memcpy(path, tt_string_cstr(&str), len);
            tt_memcpy(path + len, "/2.txt", sizeof("/2.txt"));
            fname[1] = path;
        }

        path = tt_malloc(len + sizeof("/3.txt"));
        if (path != NULL) {
            tt_memcpy(path, tt_string_cstr(&str), len);
            tt_memcpy(path + len, "/3.txt", sizeof("/3.txt"));
            fname[2] = path;
        }

        path = tt_malloc(len + sizeof("/4.txt"));
        if (path != NULL) {
            tt_memcpy(path, tt_string_cstr(&str), len);
            tt_memcpy(path + len, "/4.txt", sizeof("/4.txt"));
            fname[3] = path;
        }

        tt_string_destroy(&str);
        done = TT_TRUE;
    }

#endif
}

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

        len = tt_rand_u32() % sizeof(buf) + 1;
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
        } else if (ret == TT_E_END) {
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

TT_TEST_ROUTINE_DEFINE(case_fs_consistency)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_task_t task;
    tt_result_t ret;
    tt_u32_t j;

    TT_TEST_CASE_ENTER()
// test start

#ifdef __UT_LITE__
    return TT_SUCCESS;
#endif

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

TT_TEST_ROUTINE_DEFINE(case_fs_copy)
{
    // tt_u32_t param = TT_TEST_ROUTINE_PARAM(tt_u32_t);
    tt_result_t ret;
    tt_file_t f;
    tt_char_t buf[] = "12345", buf2[10];
    tt_u32_t n;

    TT_TEST_CASE_ENTER()
    // test start

    tt_fremove(__SC_TEST_FILE);
    tt_fremove(__SC_TEST_FILE2);

    // src not exist
    {
        ret = tt_fcopy(__SC_TEST_FILE2, __SC_TEST_FILE, 0);
        TT_UT_FAIL(ret, "");
    }

    // copied
    {
        ret = tt_fopen(&f, __SC_TEST_FILE, TT_FO_CREAT | TT_FO_WRITE, NULL);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fwrite(&f, (tt_u8_t *)buf, (tt_u32_t)sizeof(buf), NULL);
        TT_UT_SUCCESS(ret, "");
        tt_fclose(&f);

        ret = tt_fcopy(__SC_TEST_FILE2, __SC_TEST_FILE, 0);
        TT_UT_SUCCESS(ret, "");

        ret = tt_fopen(&f, __SC_TEST_FILE2, TT_FO_READ, NULL);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fread(&f, (tt_u8_t *)buf2, sizeof(buf2), &n);
        TT_UT_SUCCESS(ret, "");
        TT_UT_EQUAL(n, sizeof(buf), "");
        tt_fclose(&f);
    }

    // copy excl
    {
        ret = tt_fcopy(__SC_TEST_FILE2, __SC_TEST_FILE, TT_FCOPY_EXCL);
        TT_UT_FAIL(ret, "");
        tt_fremove(__SC_TEST_FILE2);
        ret = tt_fcopy(__SC_TEST_FILE2, __SC_TEST_FILE, TT_FCOPY_EXCL);
        TT_UT_SUCCESS(ret, "");
    }
    tt_fremove(__SC_TEST_FILE2);

    // copy to existing dir
    {
        tt_dcreate(__SC_TEST_FILE2, NULL);
        ret = tt_fcopy(__SC_TEST_FILE2, __SC_TEST_FILE, 0);
        TT_UT_FAIL(ret, "");
    }

    // test copying dir

    tt_fremove(__SC_TEST_FILE);
    tt_fremove(__SC_TEST_FILE2);
    tt_dremove(__SC_TEST_FILE);
    tt_dremove(__SC_TEST_FILE2);

    // src not exist
    {
        ret = tt_dcopy(__SC_TEST_FILE2, __SC_TEST_FILE, 0);
        TT_UT_FAIL(ret, "");
    }

#if 0 // undefined behavior
    // src is a file
    {
        ret = tt_fcreate(__SC_TEST_FILE, NULL);
        TT_UT_SUCCESS(ret, "");

        ret = tt_dcopy(__SC_TEST_FILE2, __SC_TEST_FILE, 0);
        TT_UT_FAIL(ret, "");
    }
#endif

    {
#if TT_ENV_OS_IS_IOS && (TT_ENV_OS_FEATURE & TT_ENV_OS_FEATURE_IOS_SIMULATOR)

#define __CD_D1 "../tmp/d1"
#define __CD_D1S "../tmp/d1/"
#define __CD_D1_D2 "../tmp/d1/d2"
#define __CD_D1_F1 "../tmp/d1/f1"
#define __CD_D1_D2_F2 "../tmp/d1/d2/f2"

#define __COPIED_D1 "../tmp/copied_d1"
#define __COPIED_D1S "../tmp/copied_d1/"
#define __COPIED_D1_D2 "../tmp/copied_d1/d2"
#define __COPIED_D1_F1 "../tmp/copied_d1/f1"
#define __COPIED_D1_D2_F2 "../tmp/copied_d1/d2/f2"

#else

#define __CD_D1 "d1"
#define __CD_D1S "d1/"
#define __CD_D1_D2 "d1/d2"
#define __CD_D1_F1 "d1/f1"
#define __CD_D1_D2_F2 "d1/d2/f2"

#define __COPIED_D1 "copied_d1"
#define __COPIED_D1S "copied_d1/"
#define __COPIED_D1_D2 "copied_d1/d2"
#define __COPIED_D1_F1 "copied_d1/f1"
#define __COPIED_D1_D2_F2 "copied_d1/d2/f2"

#endif

        tt_dremove(__CD_D1);
        tt_dremove(__COPIED_D1);

        ret = tt_dcreate(__CD_D1_D2, NULL);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fcreate(__CD_D1_F1, NULL);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fcreate(__CD_D1_D2_F2, NULL);
        TT_UT_SUCCESS(ret, "");

        ret = tt_dcopy(__COPIED_D1, __CD_D1, 0);
        TT_UT_SUCCESS(ret, "");

        TT_UT_TRUE(tt_fs_exist(__COPIED_D1), "");
        TT_UT_TRUE(tt_fs_exist(__COPIED_D1_F1), "");
        TT_UT_TRUE(tt_fs_exist(__COPIED_D1_D2), "");
        TT_UT_TRUE(tt_fs_exist(__COPIED_D1_D2_F2), "");

        // endwith slash
        tt_dremove(__CD_D1);
        tt_dremove(__COPIED_D1);

        ret = tt_dcreate(__CD_D1_D2, NULL);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fcreate(__CD_D1_F1, NULL);
        TT_UT_SUCCESS(ret, "");
        ret = tt_fcreate(__CD_D1_D2_F2, NULL);
        TT_UT_SUCCESS(ret, "");

        ret = tt_dcopy(__COPIED_D1S, __CD_D1S, 0);
        TT_UT_SUCCESS(ret, "");

        TT_UT_TRUE(tt_fs_exist(__COPIED_D1), "");
        TT_UT_TRUE(tt_fs_exist(__COPIED_D1_F1), "");
        TT_UT_TRUE(tt_fs_exist(__COPIED_D1_D2), "");
        TT_UT_TRUE(tt_fs_exist(__COPIED_D1_D2_F2), "");

#if 0
        ret = tt_dcopy("copied_d1", "d1", TT_DCOPY_EXCL);
        TT_UT_FAIL(ret, "");
#endif
    }

    // test end
    TT_TEST_CASE_LEAVE()
}
