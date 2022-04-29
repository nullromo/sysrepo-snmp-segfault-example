#ifndef PLUGIN_BASE_HPP
#define PLUGIN_BASE_HPP

#include <sysrepo.h>
#include <sysrepo/netconf_acm.h>
#include <sysrepo_types.h>

typedef int (*PluginInitFunction)(sr_session_ctx_t* session,
                                  void** privateData);

typedef void (*PluginCleanupFunction)(sr_session_ctx_t* session,
                                      void* privateData);

void enableLogging();

class PluginBase {
  public:
    PluginBase(const char* name,
               PluginInitFunction pluginInit,
               PluginCleanupFunction pluginCleanup);

    int run(void);

  private:
    const char* name = "";
    PluginInitFunction pluginInit = NULL;
    PluginCleanupFunction pluginCleanup = NULL;
    sr_conn_ctx_t* connection = NULL;
    sr_session_ctx_t* session = NULL;

    // only used for nacm if the plugin is a standalone plugin
    sr_subscription_ctx_t* nacmSubscription = NULL;
    
};

#endif // PLUGIN_BASE_HPP
