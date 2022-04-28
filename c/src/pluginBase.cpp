#include "pluginBase.hpp"
#include <signal.h>

void enableLogging() {
    stdout = stderr;
    sr_log_stderr(SR_LL_DBG);
    ly_log_level(LY_LLDBG);
    ly_log_options(LY_LOLOG | LY_LOSTORE);
}

#ifdef STANDALONE

volatile bool running = true;

static void signalHandler(int signalCode) {
    printf("\n");
    switch (signalCode) {
        case SIGSEGV:
            printf("Segmentation fault, aborting.\n");
            exit(1);
            break;
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            printf("User abort\n");
            break;
        default:
            break;
    }
    running = false;
}

PluginBase::PluginBase(const char* name,
                       PluginInitFunction pluginInit,
                       PluginCleanupFunction pluginCleanup)
    : name(name),
      pluginInit(pluginInit),
      pluginCleanup(pluginCleanup) {
    enableLogging();
}

int PluginBase::run() {
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGSEGV, signalHandler);

    int rc = SR_ERR_OK;

    // connect to sysrepo
    rc = sr_connect(SR_CONN_DEFAULT, &this->connection);
    if (rc) {
        printf("%s plugin failed to connect: %s.\n",
               this->name,
               sr_strerror(rc));
        goto finish;
    }

    // start a session on the running datastore
    rc = sr_session_start(connection, SR_DS_RUNNING, &this->session);
    if (rc) {
        printf("%s plugin failed to start session: %s.\n",
               this->name,
               sr_strerror(rc));
        goto finish;
    }

    // start the plugin using the session
    rc = this->pluginInit(this->session, NULL);
    if (rc != SR_ERR_OK) {
        printf("%s plugin failed to initialize: %s.\n",
               this->name,
               sr_strerror(rc));
        goto finish;
    }

    // loop until an exit signal is received
    while (running)
        sleep(1);

finish:
    this->pluginCleanup(this->session, NULL);
    sr_disconnect(this->connection);
    return rc;
}

#endif // STANDALONE
