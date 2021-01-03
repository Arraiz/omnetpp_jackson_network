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
    virtual void sendFromQueue();

  private:
    cMessage *msgEvent;
    paquete *queuePack;
    cMessage *queueEvent;
    cQueue *queue;
    cChannel *channel;
    unsigned short estado=0; //
    enum estados
       {   ready_to_send = 0,
           sending = 1,
           waiting_ack = 2,
       };
    /*
     * 0: ready_to_send
     * 1: sending (busy)
     * 2: waiting for ACK (busy)
     * */


};

// The module class needs to be registered with OMNeT++
Define_Module(node_ext);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
node_ext::~node_ext(){
        cancelAndDelete(msgEvent);
        queue->~cQueue();
        delete msgEvent;
}

void node_ext::initialize()
{

    channel = gate("out")->getTransmissionChannel();
    queue = new cQueue("node_ext_queue");
    WATCH(estado);




}


void node_ext::handleMessage(cMessage *msg)
{

  /*  if(msg->isSelfMessage() && channel->isBusy() ){
        EV << getName()<< ": " << "Queue event arrived: sending queue paquet\n";
        sendFromQueue();
        EV << getName()<< ": " << "queue size: " << queue->getLength()  <<"\n";
    }
*/

    if(msg->isSelfMessage()){

        //*en teoria* hemos terminado de enviar el mensaje y estamos esperando ACK
        estado=waiting_ack;

    }

   if(msg->arrivedOn("packet_in")){ //mirar si el trafico es para inyectar en la red
       paquete *pck = check_and_cast<paquete*>(msg);
       switch(estado) {



         case ready_to_send:
         {
           estado=sending;
           EV << getName()<< ": " << "message arrived to packet_in, state: ready to send Sending\n";
           sendCopyOf(pck);
           break;
         }
         case sending:{
           //encola y schedulea
           EV << getName()<< ": " << "message arrived to packet_in, state: sending, Queuing\n";

           queue->insert(pck);
           simtime_t txFinishTime = channel->getTransmissionFinishTime(); //esto es un poco magia negra...
           scheduleAt(txFinishTime,new cMessage("Queue event"));
           EV << getName()<< ": " << "queue size: " << queue->getLength()  <<"\n";
           // code block
           break;
         }
         case waiting_ack:{
            //encola
             EV << getName()<< ": " << "message arrived to packet_in sate: waiting ACK , Queuing\n";
             queue->insert(pck);
             EV << getName()<< ": " << "queue size: " << queue->getLength()  <<"\n";
            break;
         }

       }

   }else if(msg->arrivedOn("in")){//es un ack
       EV << getName()<< ": " << "ACK rec\n";
       estado=ready_to_send;

   }

}


void node_ext::sendFromQueue()
{
    /*Duplicar el mensaje y mandar una copia*/

    queuePack = (paquete *)queue -> pop();
    sendCopyOf(queuePack);
}


void node_ext::sendCopyOf(paquete *msg)
{
    /*Duplicar el mensaje y mandar una copia*/
    estado=sending;
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
    simtime_t txFinishTime = channel->getTransmissionFinishTime(); //esto es un poco magia negra...
    scheduleAt(txFinishTime,new cMessage("state2"));

}


