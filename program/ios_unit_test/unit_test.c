#include <tt_platform.h>

#include "unit_test_process.h"

//#include <locale.h>

struct tt_evcenter_s;

extern tt_result_t __ipc_cli_1(IN void *param);
extern tt_result_t __ipc_cli_oneshot(IN void *param);
extern tt_result_t __ipc_svr_1(IN void *param);

static tt_bool_t tt_ut_ok = TT_FALSE;

tt_result_t __ut_fiber(IN void *param)
{
    const tt_char_t *name = NULL;
    const tt_char_t buf[200] = {0};

    tt_test_framework_init(0);
    tt_test_unit_init(NULL);

#if TT_ENV_OS_IS_WINDOWS
    GetEnvironmentVariableA("TT_CASE", buf, sizeof(buf) - 1);
    name = buf;
#else
    name = getenv("TT_CASE");
#endif
    if (name != NULL) {
        if (tt_strcmp(name, "all") == 0) {
            tt_test_unit_run(NULL);
            tt_test_unit_list(NULL);
            tt_ut_ok = TT_TRUE;
        } else if (TT_OK(tt_test_unit_run(name))) {
            tt_ut_ok = TT_TRUE;
        }
    }
#if 1
    else {
        tt_u32_t i;
        const tt_char_t *names[] = {
//"case_fs_basic",
#if 0
            "TEST_UNIT_LOG",
            "TEST_UNIT_LOG_PATTERN",
#endif

#if 0
            "TEST_UNIT_ATOMIC",
            "TEST_UNIT_THREAD",
            "TEST_UNIT_FIBER",
            "TEST_UNIT_SEM",
            "TEST_UNIT_RWLOCK",
            "TEST_UNIT_SPIN_LOCK",
            "TEST_UNIT_MUTEX",
#endif

#if 0
            "TEST_UNIT_SLAB",
            "TEST_UNIT_MEMPOOL",
#endif

#if 0
            "TEST_UNIT_TIME_REF",
            "TEST_UNIT_DATE",
            "TEST_UNIT_TIMER",
            "TEST_UNIT_FPATH",
#endif

#if 0
            "TEST_UNIT_MISC",
            "TEST_UNIT_CHARSET",
            "TEST_UNIT_CFGNODE",
            "TEST_UNIT_CFGPATH",
            "TEST_UNIT_CFGSHELL",
#endif

#if 0
            "TEST_UNIT_FS",
"TEST_UNIT_IPC",
//"TEST_UNIT_SOCKET",
#endif

#if 0
            "ALG_UT_BASIC_ALG",
            "ALG_UT_HEAP",
            "ALG_UT_VECTOR",
            "ALG_UT_LIST",
            "ALG_UT_HASHMAP",
            "ALG_UT_QUEUE",
            "ALG_UT_RBTREE",
            "ALG_UT_STACK",
            "ALG_UT_RBUF",
            "ALG_UT_BUF",
            "ALG_UT_STRING",
#endif

#if 0
            "DNS_UT_QUERY",
            "DNS_UT_RR",
            "DNS_UT_CACHE",
#endif

#if 0
            "SSL_UT_X509",
            "SSL_UT_IO",
#endif

#if 1
            "HTTP_UT_URI",
            "HTTP_UT_HDR",
            "HTTP_UT_RENDER",
            "HTTP_UT_SCONN",
            "HTTP_UT_SVR",
            "HTTP_UT_INSERV",
#endif
        };

        for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
            tt_test_unit_run(names[i]);
        }
        tt_test_unit_list(NULL);
        tt_ut_ok = TT_TRUE;
    }
#else
    else {
        printf("unit_test <case name> | all");
    }
#endif

    return TT_SUCCESS;
}

int app_ut_main(int argc, char *argv[])
{
    tt_thread_t *ut_thread;
    int i;
    tt_task_t t;

    // setlocale(LC_ALL, "chs");

    for (i = 0; i < argc; ++i) { printf("argv[%d]: %s\n", i, argv[i]); }

    if (argc > 1) {
        if (strcmp(argv[1], "process") == 0) {
            return app_ut_process(argc, argv);
        } else if (strncmp(argv[1], "ipc", 3) == 0) {
            if (argc < 3) {
                printf("app unit test ipc need at least 3 args\n");
                return -1;
            }

            // init platform
            tt_platform_init(NULL);

            tt_task_create(&t, NULL);
            if (strcmp(argv[1], "ipc-1") == 0) {
                tt_task_add_fiber(&t, NULL, __ipc_cli_1, NULL, NULL);
            } else if (strcmp(argv[1], "ipc-2") == 0) {
                tt_task_add_fiber(&t, NULL, __ipc_cli_oneshot, NULL, NULL);
            } else if (strcmp(argv[1], "ipc-svr") == 0) {
                tt_task_add_fiber(&t, NULL, __ipc_svr_1, NULL, NULL);
            }
            tt_task_run(&t);
            tt_task_wait(&t);
            printf("exiting\n");
#if 0
            while (1) {
#else
            while (0) {
#endif
            tt_sleep(10000);
        }
        exit(0);
        return 0;
    } // haniu
    else {
        printf("unknown process arg: %s\n", argv[1]);

        // must return so as to infinitely creating process
        return -1;
    }
}

// init platform
tt_platform_init(NULL);

tt_task_create(&t, NULL);
tt_task_add_fiber(&t, NULL, __ut_fiber, NULL, NULL);
tt_task_run(&t);
tt_task_wait(&t);
printf("exiting\n");
tt_platform_exit();

// disable below for travis CI
exit(0);
return 0;

// tt_page_os_stat_show(0);
// tt_skt_stat_show(0);

#ifdef TT_WINDOWS_CRT_DUMP
_CrtDumpMemoryLeaks();
#endif

#if TT_ENV_OS_IS_WINDOWS
while (1) {
#else
    while (0) {
#endif
    tt_sleep(10000);
}

return 0;
}
