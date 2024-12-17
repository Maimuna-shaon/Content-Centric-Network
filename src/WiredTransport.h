

#ifndef __PHOENIX_WIREDTRANSPORT_H_
#define __PHOENIX_WIREDTRANSPORT_H_

#include <omnetpp.h>
#include <queue>
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

class WiredTransport : public cSimpleModule
{
  protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual int numInitStages() const {return 3;}
    virtual void finish();

  private:
      // model parameters
      string wiredTechnology;
      double dataRate;
      double packetErrorRate;
      int headerSize;

      // globally unique MAC-like address
      string macAddress;

      // node ID
      long nodeID;

      // model info
      Demiurge *demiurgeModel;
      Numen *numenModel;
      cModule *nodeModel;
      inet::IMobility *mobilityModel;

      // queues messages arriving from forwarding
      queue<cMessage*> messageQueue;
      cMessage *msgSendCompletedEvent;

      void buildMACLikeAddress();
      void getDemiurgeModel();
      void getAllOtherModels();
      void registerWiredTransportWithDemiurge();
      void processOutgoingMessage(cMessage *msg);
      void processIncomingMessage(cMessage *msg);
      void sendOutgoingMessage(cMessage *msg);

      simsignal_t totalWiredBytesReceivedSignal;
      simsignal_t totalWiredBytesSentSignal;

};

#define WIREDTRANSPORT_TRANSPORT_REG_REM_EVENT_CODE    106
#define WIREDTRANSPORT_MSG_SEND_COMPLETED_EVENT_CODE   107

#endif
