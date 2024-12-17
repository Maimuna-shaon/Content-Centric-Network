

#ifndef __PHOENIX_WIRELESSTRANSPORT_H_
#define __PHOENIX_WIRELESSTRANSPORT_H_

#include <omnetpp.h>
#include "InternalMessages_m.h"
#include "TransportMessages_m.h"
#include "Numen.h"

#include "inet/mobility/contract/IMobility.h"
#include "Demiurge.h"
#include "phoenix.h"

using namespace omnetpp;
using namespace std;

class Demiurge;
class Numen;

class WirelessTransport : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const {return 3;}
        void refreshDisplay() const;
        virtual void finish();

    private:
        // model parameters
        string wirelessTechnology;
        string operationMode;
        string connectString;
        double wirelessRange;
        double dataRate;
        double packetErrorRate;
        double scanInterval;
        int headerSize;
        bool wirelessRangeRadiusShow;
        string wirelessRangeRadiusColor;

        // globally unique MAC-like address
        string macAddress;

        // node ID
        long nodeID;

        // model info
        Demiurge *demiurgeModel;
        Numen *numenModel;
        cModule *nodeModel;
        inet::IMobility *mobilityModel;

        // variables used when in client mode
        WirelessTransportInfo *lastConnectedAP;

        void buildMACLikeAddress();
        void getDemiurgeModel();
        void getAllOtherModels();
        void registerWirelessTransportWithDemiurge();
        bool inWirelessRange(inet::IMobility *neighbourMobilityModel, inet::IMobility *ownMobilityModel, double radius);
        void processOutgoingMessage(cMessage *msg);
        void processOutgoingOnAPNode(cMessage *msg);
        void processOutgoingOnClientNode(cMessage *msg);
        void processOutgoingOnDirectNode(cMessage *msg);
        void processIncomingMessage(cMessage *msg);
        void processSendingNeighbourList();

        simsignal_t totalWirelessBytesReceivedSignal;
        simsignal_t totalWirelessBytesSentSignal;

};

#define WIRELESSTRANSPORT_TRANSPORT_REG_REM_EVENT_CODE    116
#define WIRELESSTRANSPORT_SCAN_NEIGH_EVENT_CODE           117

#endif
