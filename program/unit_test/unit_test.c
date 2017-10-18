#include <tt_platform.h>

#include <tt_cstd_api.h>

#include "app_unit_test_process.h"

//#include <locale.h>

extern tt_result_t __ipc_cli_1(IN void *param);
extern tt_result_t __ipc_cli_oneshot(IN void *param);
extern tt_result_t __ipc_svr_1(IN void *param);
extern tt_result_t __ipc_cli_pev(IN void *param);

extern tt_result_t tt_cli_demo_run();

tt_buf_t console_output;

tt_result_t __console_ev_handler(IN void *console_param,
                                 IN tt_cons_ev_t ev,
                                 IN tt_cons_ev_data_t *ev_data)
{
#if 0
    if (ev == TT_CONS_EV_KEY) {
        tt_cli_t *cli = (tt_cli_t *)console_param;
        tt_cons_ev_data_t output_data;

        if (ev_data->key.key[0] == TT_CONS_EXTKEY_CTRLC) {
            tt_u8_t cli_key = TT_CLI_EXTKEY_CTRLC;
            tt_cli_input(cli, &cli_key, 1, &console_output);
        } else if (ev_data->key.key[0] == TT_CONS_EXTKEY_UP) {
            tt_u8_t cli_key = TT_CLI_EXTKEY_UP;
            tt_cli_input(cli, &cli_key, 1, &console_output);
        } else if (ev_data->key.key[0] == TT_CONS_EXTKEY_DOWN) {
            tt_u8_t cli_key = TT_CLI_EXTKEY_DOWN;
            tt_cli_input(cli, &cli_key, 1, &console_output);
        } else if (ev_data->key.key[0] == TT_CONS_EXTKEY_RIGHT) {
            tt_u8_t cli_key = TT_CLI_EXTKEY_RIGHT;
            tt_cli_input(cli, &cli_key, 1, &console_output);
        } else if (ev_data->key.key[0] == TT_CONS_EXTKEY_LEFT) {
            tt_u8_t cli_key = TT_CLI_EXTKEY_LEFT;
            tt_cli_input(cli, &cli_key, 1, &console_output);
        } else {
            tt_cli_input(cli,
                         ev_data->key.key,
                         ev_data->key.key_num,
                         &console_output);
        }

        output_data.key.key = TT_BUF_RPOS(&console_output);
        output_data.key.key_num = TT_BUF_RLEN(&console_output);
        tt_console_send(TT_CONS_EV_KEY, &output_data);
        tt_buf_reset_rwp(&console_output);
    }
#else
    tt_console_send(ev, ev_data);
    if (ev_data->key.key[0] == TT_CONS_EXTKEY_CTRLD) {
        TT_DEBUG("exit console 1");
        TT_DEBUG("exit console 2");
        TT_DEBUG("exit console 3");
        return TT_E_END;
    }
#endif

    return TT_SUCCESS;
}

static tt_bool_t tt_ut_ok = TT_FALSE;

tt_result_t __ut_fiber(IN void *param)
{
    const tt_char_t *name = NULL;
    tt_char_t buf[200] = {0};

    tt_test_framework_init(0);
    tt_test_unit_init(NULL);

#if TT_ENV_OS_IS_WINDOWS
    GetEnvironmentVariableA("TT_CASE", buf, sizeof(buf) - 1);
    name = buf;
    printf("%s|\n", name);
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
        const tt_char_t *names[] = {"case_zsrc_writefile_off"};
        tt_u32_t i;

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

static tt_s32_t tt_g_flock_ret;

tt_result_t __flock_fiber(IN void *param)
{
    char **argv = (char **)param;
    tt_file_t f;
    tt_result_t r;

    printf("testing flock 2\n");

    if (!TT_OK(tt_fopen(&f, argv[2], TT_FO_RDWR, NULL))) {
        tt_g_flock_ret = -1;
        return TT_FAIL;
    }
    printf("flock %d\n", TT_BOOL(strcmp(argv[3], "ex") == 0));
    r = tt_ftrylock(&f, TT_BOOL(strcmp(argv[3], "ex") == 0));
    if (TT_OK(r)) {
        tt_funlock(&f);
        tt_g_flock_ret = 0;
    } else if (r == TT_E_TIMEOUT) {
        tt_g_flock_ret = 1;
    } else {
        tt_g_flock_ret = -1;
    }
    return TT_SUCCESS;
}

int main(int argc, char *argv[])
{
    tt_task_t t;

// setlocale(LC_ALL, "chs");

#if 0
    for (i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
#endif

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
            } else if (strcmp(argv[1], "ipc-pev") == 0) {
                tt_task_add_fiber(&t, NULL, __ipc_cli_pev, NULL, NULL);
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
            return 0;
        }
        return 0;
    } else if (strncmp(argv[1], "sh_unix", 7) == 0) {
        extern void tt_test_gen_sh_unix();
        tt_test_gen_sh_unix();
        return 0;
    } else if (strncmp(argv[1], "sh_win", 10) == 0) {
        extern void tt_test_gen_sh_win();
        tt_test_gen_sh_win();
        return 0;
    } else if (strcmp(argv[1], "flock") == 0) {
        tt_task_t t;

        printf("testing flock 1\n");

        if (argc < 4) {
            return -1;
        }

        tt_platform_init(NULL);
        tt_task_create(&t, NULL);
        tt_task_add_fiber(&t, NULL, __flock_fiber, argv, NULL);
        tt_task_run(&t);
        tt_task_wait(&t);
        printf("exiting\n");
        return tt_g_flock_ret;
    } else {
        printf("unknown process arg: %s\n", argv[1]);

        // must return so as to infinitely creating process
        return -1;
    }
}

// init platform
tt_platform_init(NULL);

tt_thread_create_local(NULL);

// run
#define AUT_MODE 0

#if AUT_MODE == 0
tt_task_create(&t, NULL);
tt_task_add_fiber(&t, NULL, __ut_fiber, NULL, NULL);
tt_task_run(&t);
tt_task_wait(&t);
printf("exiting\n");
return TT_COND(tt_ut_ok, 0, -1);
#if TT_ENV_OS_IS_WINDOWS
while (1) {
#else
while (0) {
#endif
    tt_sleep(10000);
}

return 0;
#elif AUT_MODE == 1
    {
        tt_console_run(__console_ev_handler, NULL, TT_TRUE);
    }
#elif AUT_MODE == 2
{
    tt_shell_t sh;

    tt_console_sh_create(&sh, TT_CLI_MODE_DEFAUTL, NULL);

    tt_console_sh_run(&sh, TT_TRUE);
}
#else
tt_cli_demo_run();
#endif

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
