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
    virtual void sendAck(int seqNum);

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
    if(msg->arrivedOn("in")){// mirar si el trafico es n


           EV << getName()<< ": " << "message arrived to in\n";
           EV << getName()<< ": "<< "checking transmision time\n";
           //simtime_t txFinishTime = channel->getTransmissionFinishTime();
          // EV << getName() << ":" << "ready at time:"<< txFinishTime.getScale() <<"time\n";
           paquete *packet = check_and_cast<paquete*>(msg);
           sscanf(packet->getName(), "packet-%d",&packet_number);
           EV << getName() << ":" << "packet number:"<< packet_number <<"\n";
           //sendAck(packet->getSeq());
           simtime_t txFinishTime = channel->getTransmissionFinishTime();
           sendAck(packet->getSeq());
           //scheduleAt(txFinishTime,new cMessage("ack"));

       }
    else if(msg->isSelfMessage()){
        paquete *packet = check_and_cast<paquete*>(msg);
        sendAck(packet->getSeq());
    }


}

void node::sendAck(int seqNum){
    char ack_name[50];
    sprintf(ack_name, "ack-%d", seqNum);
    paquete *ack = new paquete(ack_name,0);
    ack->setBitLength(1);
    ack->setSeq(seqNum);
    send(ack,"out");

}

void node::sendCopyOf(paquete *msg)
{
    /*Duplicar el mensaje y mandar una copia*/
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
}


