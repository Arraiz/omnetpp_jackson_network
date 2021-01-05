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
    virtual void s_w_receriver(cMessage *msg);

  private:
    cMessage *msgEvent;
    cMessage *ackMessage;
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

    s_w_receriver(msg);

}

void node::s_w_receriver(cMessage *msg){

    if(msg->arrivedOn("in")){// mirar si el trafico es n
             EV << getName()<< ": " << "message arrived to in\n";
            paquete *packet = check_and_cast<paquete*>(msg);
             sscanf(packet->getName(), "packet-%d",&packet_number);
             EV << getName() << ":" << "packet number:"<< packet_number <<"\n";
             ackMessage = new cMessage("ackMsg");
             ackMessage->addPar("seqNum").setLongValue(packet_number);

             if(channel->isBusy()){// si el canal esta ocupado sceduleamos
                 simtime_t txFinishTime = channel->getTransmissionFinishTime();
                 scheduleAt(txFinishTime,ackMessage);
             }else{//sino mandamos directamente
                 scheduleAt(simTime(),ackMessage);

             }
         }
      else if(msg->isSelfMessage()){
          EV << getName() << ":" << "Handling self-message:"<< msg->getFullName() <<"\n";
          if(strcmp(msg->getFullName(), "ackMsg")==0){
              sendAck(msg->par("seqNum").longValue());
          }
      }

}

void node::sendAck(int seqNum){

    char ack_name[50];

    if((rand()%100)>0){ //probabilidad de no devolver paquete pqueloss..

        if((rand()%100)<0){ //probabilidad de nak
                //nack
            EV << getName() << ":" << "Sending nack\n";
            sprintf(ack_name, "nack-%d", seqNum);
            paquete *nack = new paquete(ack_name,2);
            nack->setBitLength(2);
            nack->setSeq(seqNum);
            nack->setType(2);
            send(nack,"out");
            }else{
                //ack
            EV << getName() << ":" << "Sending ack\n";
            sprintf(ack_name, "ack-%d", seqNum);
            paquete *ack = new paquete(ack_name,1);
            ack->setBitLength(20);
            ack->setSeq(seqNum);
            ack->setType(1);
            send(ack,"out");
            //enviar paquete a siguiente red
            }

    }else{//pierde pqt
        EV << getName() << ":" << "loosing packet\n";
    }

}





void node::sendCopyOf(paquete *msg)
{
    /*Duplicar el mensaje y mandar una copia*/
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
}


