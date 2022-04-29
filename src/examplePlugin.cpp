#include "pluginBase.hpp"
#include <sysrepo.h>

#define EXAMPLE_DATA_XPATH "/example-module:example-data"

sr_subscription_ctx_t* subscriptionContext = NULL;

#ifdef __cplusplus
extern "C" {
#endif
    int exampleDataCallback(sr_session_ctx_t* session,
                            uint32_t subscriptionID,
                            const char* moduleName,
                            const char* path,
                            const char* requestXPath,
                            uint32_t requestID,
                            struct lyd_node** parent,
                            void* privateData) {
        (void)privateData;
        (void)moduleName;
        (void)path;
        (void)requestXPath;
        (void)subscriptionID;
        (void)requestID;

        // find the libyang context
        const ly_ctx* context =
                sr_acquire_context(sr_session_get_connection(session));

        // create the parent node
        int lyrc = lyd_new_path(
                NULL, context, EXAMPLE_DATA_XPATH, NULL, 0, parent);
        if (lyrc != LY_SUCCESS) {
            goto cleanup;
        }

        lyrc = lyd_new_path(*parent,
                            context,
                            EXAMPLE_DATA_XPATH "/thing",
                            "hello",
                            0,
                            NULL);
        if (lyrc != LY_SUCCESS) {
            goto cleanup;
        }

    cleanup:
        if (lyrc != LY_SUCCESS) {
            printf("Error building data tree: (libyang error: %d).\n", lyrc);
            return SR_ERR_INTERNAL;
        }
        return SR_ERR_OK;
    }

    int sr_plugin_init_cb(sr_session_ctx_t* session, void** privateData) {
        enableLogging();
        int rc =
                sr_oper_get_subscribe(session,
                                      "example-module",
                                      EXAMPLE_DATA_XPATH,
                                      exampleDataCallback,
                                      privateData == NULL ? NULL : *privateData,
                                      SR_SUBSCR_DEFAULT,
                                      &subscriptionContext);
        if (rc != SR_ERR_OK) {
            printf("Plugin initialization failed: %s.\n", sr_strerror(rc));
            return rc;
        }
        printf("Plugin initialized successfully.\n");
        return rc;
    }

    void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* privateData) {
        (void)session;
        (void)privateData;

        sr_unsubscribe(subscriptionContext);
        printf("Cleanup finished.\n");
    }

#ifdef STANDALONE
    int main() {
        PluginBase plugin = PluginBase(
                "example-module", &sr_plugin_init_cb, &sr_plugin_cleanup_cb);
        return plugin.run();
    }
#endif

#ifdef __cplusplus
}
#endif
