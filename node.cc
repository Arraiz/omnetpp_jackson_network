#include <string.h>
#include <omnetpp.h>
#include <stdio.h>
#include "paquete_m.h"


using namespace omnetpp;


class node : public cSimpleModule
{
  public:
    virtual ~node();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendCopyOf(paquete *packet);
    virtual void sendAck();

  private:
    cMessage *msgEvent;
    cQueue *queue;
    cChannel *channel;
    int packet_number=0;

};

// The module class needs to be registered with OMNeT++
Define_Module(node);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
node::~node(){
    cancelAndDelete(msgEvent);
}

void node::initialize()
{

    channel = gate("out")->getTransmissionChannel();
    queue = new cQueue("node_ext_queue");


}


void node::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){ //tranfico inyectado

           EV << getName()<< ": " << "message arrived to in\n";
           EV << getName()<< ": "<< "checking transmision time\n";
           //simtime_t txFinishTime = channel->getTransmissionFinishTime();
          // EV << getName() << ":" << "ready at time:"<< txFinishTime.getScale() <<"time\n";
           paquete *packet = check_and_cast<paquete*>(msg);
           sscanf(packet->getName(), "packet-%d",&packet_number);
           EV << getName() << ":" << "packet number:"<< packet_number <<"\n";
           sendAck();


       }


}

void node::sendAck(){
    paquete *ack = new paquete("ACK",0);
    ack->setBitLength(1);
    send(ack,"out");

}

void node::sendCopyOf(paquete *msg)
{
    /*Duplicar el mensaje y mandar una copia*/
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
}


