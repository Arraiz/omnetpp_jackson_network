#include <string.h>
#include <omnetpp.h>
#include <stdio.h>
#include "paquete_m.h"

using namespace omnetpp;


//Entrega final 28 enero

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

    virtual void gbn_sender(cMessage *msg);
    virtual void gbn_windowSlide(int seqNum);



  private:
    cMessage *msgEvent;
    cMessage *timerEvent;
    cMessage *pckTxTime;//stop and wait

    cMessage *readyTosend;
    paquete *pck;
    paquete *queuePack;
    cMessage *queueEvent;
    cQueue *queue;
    cQueue *confirmationQueue;

    cMessage *txLineFree;//stop and wait

    cQueue *GbnWindowQueue;
    cQueue *GbnConfirmationQueue;
    cChannel *channel;
    unsigned short estado=0; //
    bool lastAckConfirmed=false;
    double TIMER=1000;
    enum estados
       {   READY_TO_SEND = 0,
           SENDING = 1,
           WAITING_ACK = 2,
       };
    unsigned int N=3;
    unsigned int estadoGBN=0;
    enum estados_GBN{
        GBN_READY=0,
        GBN_WINDOWFULL=1,
        GBN_VENTANA=2

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
    GbnWindowQueue = new cQueue("GBN-Window-queue");
    GbnConfirmationQueue = new cQueue("GBN-confirmation-queue");
    WATCH(estado);
    WATCH(estadoGBN);




}



void node_ext::handleMessage(cMessage *msg){


    if(msg->arrivedOn("packet_in") || msg->arrivedOn("in")){
       pck = check_and_cast<paquete*>(msg);
    }
    EV << getName()<<" "<< ": " << "Enter state machine with: "<<msg->getFullName()<<"\n";
    switch (estadoGBN) {
        case GBN_READY:
            EV << getName()<<" "<< ": " << "GBN_READY: "<<pck->getFullName()<<"\n";


            if (strcmp(msg->getFullName(), "txLineFree")==0){ //gestionamos la cola
                EV << getName()<<" "<< ": " << "LINE FREE: "<<pck->getFullName()<<"\n";
                if(queue->getLength() != 0){
                    EV << getName()<<" "<< ": " << "Sending from queue: "<<pck->getFullName()<<"\n";
                    pck = (paquete *)queue->get(0);
                    sendCopyOf(pck);
                }
               //comprobar ventana
               if(GbnWindowQueue->getLength()>=N){
                   estadoGBN=GBN_WINDOWFULL;
               }


           }else if(pck->getType()== 0){ // paquete de datos

                //enviamos si se puede
                if(queue->getLength()==0){
                    sendCopyOf(pck);
                }else{
                    EV << getName()<<" "<< ": " << "Sending from queue: "<<pck->getFullName()<<"\n";
                    pck = (paquete *)queue->get(0);
                    //pck = (paquete *)queue->pop(); //CUANDO HACES POP EL PAQUETE YA NO ESTA EN LA COLA POR ESO NO FUNCIONA EL CONTAIND
                    //(paquete *)queue->pop();
                    sendCopyOf(pck);
                }
                //comprovar ventana
                if(GbnWindowQueue->getLength()>=N){
                    estadoGBN=GBN_WINDOWFULL;
                }

            }else if (pck->getType() == 1) { //ack

                //si viene ack gestionamos la ventana

                int seqNum = pck->getSeq();

                gbn_windowSlide(seqNum);


            }else if (pck->getType() == 2){ //nack



            }

        break;

        case GBN_WINDOWFULL:
            EV << getName()<<" "<< ": " << "WINDOW_FULL: "<<pck->getFullName()<<"\n";
            //ventana completa, solo podemos encolar
            if (strcmp(msg->getFullName(), "txLineFree")==0){ //gestionamos la cola
                EV << getName()<<" "<< ": " << "LINE FREE, WINDOW FULL: "<<pck->getFullName()<<"\n";

            }else  if(pck->getType()== 0){ // paquete de datos
                EV << getName()<<" "<< ": " << "WINDOW_FULL: QUEUING -->" <<pck->getFullName()<<"\n";
                if(queue->contains(pck)){
                    EV << getName()<< ": " << "Pck already in queue: "<<msg->getFullName()<<"\n";
                    //queue->insert(pck);
                  }else{
                      EV << getName()<< ": " << "Inserting new packet in queue: "<<msg->getFullName()<<"\n";
                      queue->insert(pck);
                  }


           }else if (pck->getType() == 1) { //ack
                  //desplazar ventana y mirar si podemos enviar
               int seqNum = pck->getSeq();
               gbn_windowSlide(seqNum);

                   if(GbnWindowQueue->getLength()<N){
                       //podemos enviar
                       estadoGBN=GBN_READY;
                       //lanzar mensaje para poder enviar desde la cola
                   }
           }else if (pck->getType() == 2){ //nack


           }

            break;

        case GBN_VENTANA:

            // estamos gestionando la ventana, encolamos

            break;
        default:
            break;

    }
    //aliminar los pquetes
    if(channel->isBusy()){
        simtime_t txFinishTime = channel->getTransmissionFinishTime();
        txLineFree=new cMessage("txLineFree");
    }else{
        txLineFree=new cMessage("txLineFree");
        scheduleAt(simTime()+exponential(1.0),txLineFree);
    }





    //s_w_sender(msg);


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


void node_ext::gbn_windowSlide(int seqNum){

    //miramos si el numero de secuencia es menor o igual a los paquetes que tenemos en la ventan
    EV << getName()<<" "<< ": " << "sliding window for: "<< seqNum <<"\n";
    paquete *pck_iter;
    int popCOunt=0;
    for (int i=0;i<GbnWindowQueue->getLength();i++){
       pck_iter= (paquete *)GbnWindowQueue->get(i);
       EV << getName()<<" "<< ": " << "checking: " << pck_iter->getFullName() <<"\n";
       if(pck_iter->getSeq() <= seqNum){
           popCOunt++;
           EV << getName()<<" "<< ": " << "removed from queue: " << pck_iter->getFullName() <<"\n";


       }
    }
    EV << getName()<<" "<< ": " << "pop count: " << popCOunt <<"\n";
    for(int i=1;i<=popCOunt;i++){
        GbnWindowQueue->pop();
    }
    //GbnWindowQueue->pop();
}

void node_ext::gbn_sender(cMessage *msg){
    /*
    paquete *pck;
    EV << getName()<< " Incoming event: "<< msg->getFullName() << "\n";
    if(msg->arrivedOn("packet_in") || msg->arrivedOn("in")){
        pck = check_and_cast<paquete*>(msg);
    }else{
        delete pck;
    }
    EV << getName()<< "event in to FSM: "<< msg->getFullName() << "\n";
    switch (estadoGBN) {

        case GBN_SENDING:
            if(pck->getType()==0){
                EV << getName()<< " GBN_SENDING: "<< msg->getFullName() << "\n";
                if(GbnWindowQueue->getLength()<N){

                    if(queue->getLength()==0){
                        sendCopyOf(pck);
                    }else{
                        //get pack form queue
                        pck = (paquete *)queue->pop();
                        sendCopyOf(pck);

                    }
                }else{
                    estadoGBN=GBN_WAITING_ACK;
                }
             //we got an ACK
            }else if(pck->getType()==1){
                EV << getName()<< " GBN_SENDING: "<< "ACK RECEIVED" << "\n";
                GbnWindowQueue->pop();
                scheduleAt(simTime(),new cMessage("GBN_SENDING")); //timer para repeticion
            //ack event, ready for sending
            }else if(strcmp(msg->getFullName(), "GBN_SENDING")==0){
                EV << getName()<< " GBN_SENDING: "<< "GBN SENDING" << "\n";
                if(queue->getLength()==0){
                    sendCopyOf(pck);
                }else{
                    //get pack form queue

                    pck = (paquete *)queue->pop();
                    sendCopyOf(pck);
                }
            }

            break;
        case GBN_WAITING_ACK:
            EV << getName()<<" "<< " GBN_WAITING_ACK: "<< msg->getFullName() << "\n";
            if(pck->getType()==1){ // only acks allowed
                GbnWindowQueue->pop();
                if(GbnWindowQueue->getLength()<N){
                    scheduleAt(simTime(),new cMessage("GBN_SENDING")); //timer para repeticion
                    //disparar un evento de que puede enviar
                    estadoGBN=GBN_SENDING;
                }
            }
            break;
        default:
            break;
    }

*/
}

void node_ext::s_w_sender(cMessage *msg){

    paquete *pck;
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
    EV << getName()<< ": " << "Entering send copy of with: "<<msg->getFullName()<<"\n";

    if(GbnWindowQueue->getLength()>=N){
        estadoGBN=GBN_WINDOWFULL;
    }else if(channel->isBusy()){
        //aqui podriamos encolar
        if(queue->contains(msg)){
            EV << getName()<< ": " << "Pck already in queue: "<<msg->getFullName()<<"\n";
            //queue->insert(msg);
           }else{
               //new packet, queue
               queue->insert(msg);
           }
       // simtime_t txFinishTime = channel->getTransmissionFinishTime();
       // pckTxTime=new cMessage("txLineFree");
    }
    else{

    estadoGBN=GBN_READY;

    if(queue->contains(msg)){
        queue->remove(msg);
    }


    GbnWindowQueue->insert(msg);
    EV << getName()<< ": " << "SENDING: "<<msg->getFullName()<<"\n";
    //Duplicar el mensaje y mandar una copia
    //confirmationQueue->insert(msg);
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
    //simtime_t txFinishTime = channel->getTransmissionFinishTime();
    //pckTxTime=new cMessage("pckTxTime");
    //scheduleAt(txFinishTime,pckTxTime); //paquete ha llegado
    //timerEvent = new cMessage("timer");
    //scheduleAt(txFinishTime*1.3,timerEvent); //timer para repeticion

    }

}
/*
void node_ext::sendCopyOf(paquete *msg)
{


    estado=SENDING;
    EV << getName()<< ": " << "sendCopy of "<<msg->getFullName()<<"\n";
    //Duplicar el mensaje y mandar una copia
    confirmationQueue->insert(msg);
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
    simtime_t txFinishTime = channel->getTransmissionFinishTime();
    pckTxTime=new cMessage("pckTxTime");
    scheduleAt(txFinishTime,pckTxTime); //paquete ha llegado
    timerEvent = new cMessage("timer");
    scheduleAt(txFinishTime*1.3,timerEvent); //timer para repeticion

}
*/

void node_ext::refreshDisplay() const
    {
        char buf[40];
        //stop wait
        //sprintf(buf, "state: %d\nCola: %d\nconf: %d", estado,queue->getLength(),confirmationQueue->getLength());
        //getDisplayString().setTagArg("t", 0, buf);

        //GBN
        sprintf(buf, "state: %d\nCola: %d\nVentana: %d", estadoGBN,queue->getLength(),N-GbnWindowQueue->getLength());
        getDisplayString().setTagArg("t", 0, buf);
    }



