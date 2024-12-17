
#ifndef __PHOENIX_DEMIURGE_H_
#define __PHOENIX_DEMIURGE_H_

#include <omnetpp.h>

#include "phoenix.h"

using namespace omnetpp;
using namespace std;

class Demiurge : public cSimpleModule
{
    private:
        long networkCacheHitCount;
        long networkCacheMissCount;
        long networkInterstInjectedCount;
        long networkInterstRetransmissionCount;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    public:
        list<NodeInfo*> allNodesList;
        list<SameWirelessGroup*> sameWirelessGroupList;

        void incrementNetworkCacheHitCount();
        double getNetworkCacheHitRatio();
        void incrementNetworkCacheMissCount();
        double getNetworkCacheMissRatio();
        void incrementNetworkInterestRetransmissionCount();
        long getNetworkInterestRetransmissionCount();
        void incrementNetworkInterestInjectedCount();
        long getNetworkInterestInjectedCount();

};

#endif
