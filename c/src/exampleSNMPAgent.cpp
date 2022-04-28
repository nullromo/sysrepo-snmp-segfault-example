#include "exampleMib.hpp"
#include <agent_pp/v3_mib.h>
#include <agent_pp/vacm.h>

#define EMPTY_CONTEXT ""
#define PORT          4701

bool running = true;
Agentpp::Mib mib;
Agentpp::Snmpx* snmp;

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
            printf("Unknown signal\n");
            break;
    }
    running = false;
}

void initSignals() {
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGSEGV, signalHandler);
}

void snmpBoilerplate(Agentpp::RequestList* requestList) {
    mib.set_request_list(requestList);
    int status;
    snmp = new Agentpp::Snmpx(status, PORT);
    if (status != SNMP_CLASS_SUCCESS) {
        printf("Error: %s\n", snmp->error_msg(status));
        exit(status);
    }
    requestList->set_snmp(snmp);
    unsigned int snmpEngineBoots = 0;
    Agentpp::OctetStr engineId(Agentpp::SnmpEngineID::create_engine_id(PORT));
    int stat;
    Agentpp::v3MP* v3mp = new Agentpp::v3MP(engineId, snmpEngineBoots, stat);
    requestList->set_v3mp(v3mp);
    snmp->set_mpv3(v3mp);
    Agentpp::Vacm* vacm = new Agentpp::Vacm(mib);
    requestList->set_vacm(vacm);
    vacm->addNewContext(EMPTY_CONTEXT);
    mib.add(new Agentpp::V3SnmpEngine());
    mib.init();
    vacm->addNewGroup(SNMP_SECURITY_MODEL_V2,
                      "public",
                      "v1v2group",
                      storageType_volatile);
    vacm->addNewAccessEntry("v1v2group",
                            EMPTY_CONTEXT,
                            SNMP_SECURITY_MODEL_V2,
                            SNMP_SECURITY_LEVEL_NOAUTH_NOPRIV,
                            match_exact,
                            "v1v2group-ReadView",
                            "",
                            "v1v2group-NotifyView",
                            storageType_nonVolatile);
    vacm->addNewView("v1v2group-ReadView",
                     "1.3.6.1.4.1.1.1",
                     EMPTY_CONTEXT,
                     view_included,
                     storageType_nonVolatile);
}

int main() {
    int rc = fullLifecycleDataGet();
    if (rc != SR_ERR_OK) {
        printf("Could not get one-off data (%d).\n", rc);
        exit(rc);
    }
    initSignals();
    mib.add(new ExampleMib());
    mib.init();
    Agentpp::RequestList* requestList = new Agentpp::RequestList(&mib);
    snmpBoilerplate(requestList);
    printf("Running main loop...\n");
    while (running) {
        Agentpp::Request* request = requestList->receive(2);
        if (request) {
            mib.process_request(request);
        }
    }
    return 0;
}
