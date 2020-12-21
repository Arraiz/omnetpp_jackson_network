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
    cQueue *queue;
    cChannel *channel;


};

// The module class needs to be registered with OMNeT++
Define_Module(node_ext);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
node_ext::~node_ext(){
    cancelAndDelete(msgEvent);
}

void node_ext::initialize()
{

    channel = gate("out")->getTransmissionChannel();
    queue = new cQueue("node_ext_queue");


}


void node_ext::handleMessage(cMessage *msg)
{
   if(msg->arrivedOn("packet_in")){ //tranfico inyectado

       EV << getName()<< ": " << "message arrived to packet_in\n";
       EV << getName()<< ": "<< "checking transmision time\n";
       simtime_t txFinishTime = channel->getTransmissionFinishTime();
       EV << getName() << ":" << "ready at time:"<< txFinishTime.getScale() <<"time\n";
       paquete *packet = check_and_cast<paquete*>(msg);
       sendCopyOf(packet);

   }

}
void node_ext::sendCopyOf(paquete *msg)
{
    /*Duplicar el mensaje y mandar una copia*/
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
}

