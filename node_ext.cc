#include <string.h>
#include <omnetpp.h>
#include <stdio.h>
#include "paquete_m.h"

using namespace omnetpp;


class node_ext : public cSimpleModule
{
  public:
    virtual ~node_ext();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendCopyOf(paquete *packet);

  private:
    cMessage *msgEvent;

};

// The module class needs to be registered with OMNeT++
Define_Module(node_ext);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
node_ext::~node_ext(){
    cancelAndDelete(msgEvent);
}

void node_ext::initialize()
{

    EV << "MD: Event received, sending packet\n";
    cancelAndDelete(msgEvent);
    msgEvent = new cMessage("MD: Sending Packet from source");

    scheduleAt(simTime()+exponential(1.0), msgEvent);
   // pck *buildedPck = buildPacket();
   // send(buildedPck, "out");


}


void node_ext::handleMessage(cMessage *msg)
{
   cancelAndDelete(msgEvent);
   if(msg->arrivedOn("packet_in")){

       EV << "MD: message arrived to packet_in\n";
       paquete *packet = check_and_cast<paquete*>(msg);
       sendCopyOf(packet);

   }

}
void node_ext::sendCopyOf(paquete *msg)
{
    /*Duplicar el mensaje y mandar una copia*/
    paquete *copy = (paquete*) msg->dup();

    /*Set the retransmission timer to 3 times the sending time (just need to be greater than RTT)*/
       simtime_t FinishTime = gate("out")->getTransmissionChannel()->getTransmissionFinishTime();
       simtime_t nextTime = simTime()+3*(FinishTime-simTime());
       scheduleAt(nextTime,timeout);

    send(copy, "out");
}

