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
    virtual void refreshDisplay() const override;
    virtual void sendCopyOf(paquete *packet);
    virtual void s_w_sender(cMessage *msg);


  private:
    cMessage *msgEvent;
    cMessage *timerEvent;
    cMessage *pckTxTime;
    cMessage *readyTosend;
    paquete *pck;
    paquete *queuePack;
    cMessage *queueEvent;
    cQueue *queue;
    cQueue *confirmationQueue;
    cChannel *channel;
    unsigned short estado=0; //
    bool lastAckConfirmed=false;
    double TIMER=1000;
    enum estados
       {   READY_TO_SEND = 0,
           SENDING = 1,
           WAITING_ACK = 2,
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
    confirmationQueue = new cQueue("node_ext_queue_ack_queue");
    WATCH(estado);




}



void node_ext::handleMessage(cMessage *msg){

        s_w_sender(msg);


}
/*
void node_ext::handleMessage(cMessage *msg)
{
    paquete *pck = check_and_cast<paquete*>(msg);
    if(msg->isSelfMessage()){

        EV << getName()<< ": " << "selfmsg type: "<<  msg->getFullName() <<"\n";
        if(estado==SENDING){
            estado=WAITING_ACK;
        }
        else if(estado==WAITING_ACK){

            if(!channel->isBusy()){
                sendFromQueue();
            }


        }

    }

   if(msg->arrivedOn("packet_in")||msg->getFullName()=="queueMsg"){ //mirar si el trafico es para inyectar en la red
       paquete *pck = check_and_cast<paquete*>(msg);
       EV << getName()<< "state machine: packet type: " << msg->getFullName() <<"\n";
       switch(estado) {
         case READY_TO_SEND:
         {
           EV << getName()<< ": " << "message arrived to packet_in, state: ready to send Sending\n";
           sendCopyOf(pck);
           break;
         }
         case SENDING:{
           //encola y schedulea
           EV << getName()<< ": " << "message arrived to packet_in, state: sending, Queuing\n";
           queue->insert(pck);
           simtime_t txFinishTime = channel->getTransmissionFinishTime();
           scheduleAt(txFinishTime,new cMessage("Queueing"));
           EV << getName()<< ": " << "queue size: " << queue->getLength()  <<"\n";
           // code block
           break;
         }
         case WAITING_ACK:{
            //encola
           EV << getName()<< ": " << "message arrived to packet_in sate: waiting ACK , Queuing\n";
             //encola y schedulea
           queue->insert(pck);
           simtime_t txFinishTime = channel->getTransmissionFinishTime();
           if(txFinishTime <= getSimulation()->getSimTime()){
               scheduleAt(getSimulation()->getSimTime(),new cMessage("queueMsg"));
           }else{
               scheduleAt(txFinishTime,new cMessage("queueMsg"));
           }
            break;
         }

       }

   }else if(msg->arrivedOn("in")){//es un ack

       paquete *pck = check_and_cast<paquete*>(msg);
       EV << getName()<< ": " << "ACK rec: msg type"<<pck->getType()<< "\n";
       if(pck->getType()==1){
           //ack
           EV << getName()<< ": " << "ack received, poping from ackQueue\n";
           confirmationQueue->pop();
           estado=READY_TO_SEND;


       }else if(pck->getType()==2){
           //nack
           EV << getName()<< ": " << "nack received,resending\n";
           paquete *nackPack = (paquete*)confirmationQueue->pop();
           sendCopyOf(nackPack);

       }

   }

}

*/



void node_ext::s_w_sender(cMessage *msg){


    if(msg->arrivedOn("packet_in") || msg->arrivedOn("in")){
     pck = check_and_cast<paquete*>(msg);
    }



    if(msg->isSelfMessage() && strcmp(msg->getFullName(), "readyToSend") != 0 ){ //ackTxTime, temporizador
        EV << getName()<< ": " << "self-msg received: "<<msg->getFullName()<<" \n";
        if(strcmp(msg->getFullName(), "pckTxTime") == 0){ //tiempo de transmision de paquete
            EV << getName()<< ": " << "waiting ack \n";
            estado=WAITING_ACK;
        }else if(strcmp(msg->getFullName(), "timer") == 0){
            cancelEvent(pckTxTime);
            estado=SENDING;
            EV << getName()<< ": " << "timer triggered \n";
            pck = (paquete*)confirmationQueue->pop();
            sendCopyOf(pck);
        }

    }else if(pck->getType() == 0  || strcmp(msg->getFullName(), "readyToSend") == 0 ){ // es un packete normal
        EV << getName()<< "event in to FSM: "<< msg->getFullName() << "\n";

        switch (estado) {
            case READY_TO_SEND:
               EV << getName()<< "state-machine: READY TO SEND\n";
               //first send from queue
               if(queue->getLength() == 0 && pck != NULL  ){
                   if( pck->getType() == 0){
                       EV << getName()<< "forwarding regular packet\n";
                       sendCopyOf(pck);
                   }
               }else{

                   //si justo viene un packete
                   if(pck->getType()==0){
                   EV << getName()<< "sending queue packet\n";
                   queue->insert(pck);
                   pck = (paquete *)queue->pop();
                   sendCopyOf(pck);
                   }else{
                       pck = (paquete *)queue->pop();
                       sendCopyOf(pck);
                   }
               }
              break;
            case SENDING:
                EV << getName()<< "state-machine: SENDING\n";
                if(pck->getType()==0){
                    queue->insert(pck);
                }

                break;

            case WAITING_ACK:
                EV << getName()<< "state-machine: WAITING ACK\n";
                if(pck->getType()==0){
                    queue->insert(pck);
                }
                break;
            default:
                break;
        }
    }else{ // es un ACK o NACK
        cancelEvent(timerEvent);
        if(pck->getType() == 1){
            EV << getName()<< ": " << "ack received "<<msg->getFullName()<<"\n";
            confirmationQueue->pop();
            estado=READY_TO_SEND;
            //delete pck;
            readyTosend=new cMessage("readyToSend");
            scheduleAt(simTime(),readyTosend);
            //enviar que estamos ready to send

        }else { //es un nack
            estado=SENDING;
            pck = (paquete*)confirmationQueue->pop();
            //aqui no se pasa a ready to send, ya que es prioritario transmitir el paquete anterior
            sendCopyOf(pck);
        }

    }
}

void node_ext::sendCopyOf(paquete *msg)
{


    estado=SENDING;
    EV << getName()<< ": " << "sendCopy of "<<msg->getFullName()<<"\n";
    /*Duplicar el mensaje y mandar una copia*/
    confirmationQueue->insert(msg);
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
    simtime_t txFinishTime = channel->getTransmissionFinishTime();
    pckTxTime=new cMessage("pckTxTime");
    scheduleAt(txFinishTime,pckTxTime); //paquete ha llegado
    timerEvent = new cMessage("timer");
    scheduleAt(txFinishTime*1.3,timerEvent); //timer para repeticion




}


void node_ext::refreshDisplay() const
    {
        char buf[40];
        sprintf(buf, "state: %d\nCola: %d\nconf: %d", estado,queue->getLength(),confirmationQueue->getLength());
        getDisplayString().setTagArg("t", 0, buf);
    }



