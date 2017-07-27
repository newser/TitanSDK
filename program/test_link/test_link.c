#include <stdio.h>
#include <stdlib.h>

#include <tt_platform.h>

tt_result_t __ut_fiber(IN void *param)
{
    return TT_SUCCESS;
}

int main(int argc, char *argv[])
{
    tt_taskt_t *t;

    tt_platform_init(NULL);
    tt_thread_create_local(NULL);

    t = tt_task_create(&t, NULL);
    tt_task_add_fiber(&t, NULL, test_link, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);

    return 0;
}