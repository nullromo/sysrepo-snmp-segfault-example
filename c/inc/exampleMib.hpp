#ifndef EXAMPLE_MIB_HPP
#define EXAMPLE_MIB_HPP

#include <agent_pp/sim_mib.h>
#include <sysrepo.h>

inline int getJSON(sr_session_ctx_t* session,
                   std::string xpath,
                   std::string& json) {
    int rc = SR_ERR_OK;
    int lyrc = LY_SUCCESS;
    sr_data_t* data = NULL;
    char* jsonString = NULL;

    // get the data
    rc = sr_get_data(session, xpath.c_str(), 0, 0, SR_OPER_DEFAULT, &data);
    if (rc != SR_ERR_OK) {
        goto cleanup;
    }
    if (data == NULL) {
        rc = SR_ERR_NOT_FOUND;
        goto cleanup;
    }

    // format the results as a string
    lyrc = lyd_print_mem(
            &jsonString, data->tree, LYD_JSON, LYD_PRINT_WITHSIBLINGS);
    if (lyrc != LY_SUCCESS) {
        rc = SR_ERR_INTERNAL;
        goto cleanup;
    }

    // put the result into the output parameter
    if (jsonString != NULL) {
        json = std::string(jsonString);
        free(jsonString);
    }

cleanup:
    if (data != NULL) {
        sr_release_data(data);
    }
    if (rc != SR_ERR_OK) {
        printf("%s\n", sr_strerror(rc));
    }
    return rc;
}

inline int fullLifecycleDataGet() {
    int rc = SR_ERR_OK;

    // create connection
    sr_conn_ctx_t* newConnection = NULL;
    rc = sr_connect(SR_CONN_DEFAULT, &newConnection);
    if (rc != SR_ERR_OK) {
        printf("Error with new connection: %s (%d)\n", sr_strerror(rc), rc);
    }
    printf("Made new connection.\n");

    // create session
    sr_session_ctx_t* newSession = NULL;
    rc = sr_session_start(newConnection, SR_DS_OPERATIONAL, &newSession);
    if (rc != SR_ERR_OK) {
        printf("Error with new session: %s (%d)\n", sr_strerror(rc), rc);
    }
    printf("Made new session.\n");

    // do test
    std::string rawJSON;
    rc = getJSON(newSession, "/example-module:example-data", rawJSON);
    if (rc != SR_ERR_OK) {
        printf("Error with getting data: %s (%d)\n", sr_strerror(rc), rc);
    }
    std::cout << "Got result: " << rawJSON << "\n";

    // close session
    rc = sr_session_stop(newSession);
    if (rc != SR_ERR_OK) {
        printf("Error with closing session: %s (%d)\n", sr_strerror(rc), rc);
    }
    printf("Closed session.\n");

    // close connection
    rc = sr_disconnect(newConnection);
    if (rc != SR_ERR_OK) {
        printf("Error with closing connection: %s (%d)\n", sr_strerror(rc), rc);
    }
    printf("Closed connection.\n");

    return rc;
}

class ExampleLeaf : public Agentpp::SimMibLeaf {
  public:
    ExampleLeaf(const Agentpp::Oidx& id)
        : SimMibLeaf(id,
                     Agentpp::READWRITE,
                     new Agentpp::SnmpInt32(0),
                     VMODE_DEFAULT) {
    }

    void get_request(Agentpp::Request* req, int ind) {
        int rc = fullLifecycleDataGet();
        if (rc != SR_ERR_OK) {
            printf("Could not get one-off data (%d).\n", rc);
        }

        this->set_value(0);
        SimMibLeaf::get_request(req, ind);
    }
};

class ExampleMib : public Agentpp::MibGroup {
  public:
    ExampleMib() : MibGroup("1.3.6.1.4.1.1.1", "ExampleMib") {
        add(new ExampleLeaf("1.3.6.1.4.1.1.1"));
    }
};

#endif // EXAMPLE_MIB_HPP
