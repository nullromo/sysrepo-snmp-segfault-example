#ifndef EXAMPLE_MIB_HPP
#define EXAMPLE_MIB_HPP

#include <agent_pp/sim_mib.h>
#include <sysrepo.h>
#include "getData.hpp"


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
