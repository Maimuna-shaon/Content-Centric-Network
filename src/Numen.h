
#ifndef __PHOENIX_NUMEN_H_
#define __PHOENIX_NUMEN_H_

#include <omnetpp.h>

using namespace omnetpp;

class Numen : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
