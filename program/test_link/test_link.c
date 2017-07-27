#include <stdio.h>
#include <stdlib.h>

#include <tt_platform.h>

tt_result_t test_link(IN void *param)
{
    // xml
    do {
        tt_dns_rr_t drr;
        tt_dns_rr_init(&drr, "test", TT_DNS_A_IN);
        printf("dns api return\n");
    } while (0);

    // ssl
    do {
        tt_ssl_config_t sc;
        tt_ssl_config_create(&sc,
                             TT_SSL_CLIENT,
                             TT_SSL_TRANSPORT_STREAM,
                             TT_SSL_PRESET_DEFAULT);
        printf("tls api return\n");
    } while (0);

    // xml
    do {
        tt_xdoc_t xd;
        tt_xdoc_create(&xd);
        printf("xml api return\n");
    } while (0);

    return TT_SUCCESS;
}

int main(int argc, char *argv[])
{
    tt_task_t t;

    tt_platform_init(NULL);
    tt_thread_create_local(NULL);

    tt_task_create(&t, NULL);
    tt_task_add_fiber(&t, NULL, test_link, NULL, NULL);
    tt_task_run(&t);
    tt_task_wait(&t);

    return 0;
}