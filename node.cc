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
    //virtual void sendCopyOf(paquete *packet);
    virtual void sendCopyOf(paquete *msg, const char* output, cChannel* outChannel);
    virtual int sendAck(int seqNum,const char* input,const char *output);
    virtual int s_w_receriver(cMessage *msg,const char* input,const char *output);

    virtual void refreshDisplay() const override;
   // virtual void sendCopyOf(paquete *packet);
    virtual void s_w_sender(cMessage *msg,const char* source ,const char *input,const char *output, cChannel *outChannel);

    virtual void gbn_receiver(cMessage *msg);

  private:
    cMessage *msgEvent;
    cMessage *ackMessage;
    cQueue *queue;
    cChannel *channel;
    cChannel *channel_in1;
    cChannel *channel_out2;
    cChannel *channel_in2;
    cChannel *channel_out3;
    cChannel *channel_in3;
    cChannel *channel_out4;
    cChannel *channel_in4;
    int packet_number=0;


    //cMessage *msgEvent;
    cMessage *timerEvent;
    cMessage *pckTxTime;//stop and wait

    cMessage *readyTosend;
    paquete *pck;
    paquete *queuePack;
    cMessage *queueEvent;
    //cQueue *queue;
    cQueue *confirmationQueue;

    cMessage *txLineFree;//stop and wait

    cQueue *GbnWindowQueue;
    cQueue *GbnConfirmationQueue;
    //cChannel *channel;
    unsigned short estado=0; //
    char in[5];
    char out[5];
    int confirmed;
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
};

// The module class needs to be registered with OMNeT++
Define_Module(node);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
node::~node(){
    cancelAndDelete(msgEvent);
}

void node::initialize()
{


    if (strcmp("node2", getName()) == 0) {
        channel = gate("out1")->getTransmissionChannel();
        channel_out2 = gate("out2")->getTransmissionChannel();
        channel_out3 = gate("out3")->getTransmissionChannel();
        channel_out4 = gate("out4")->getTransmissionChannel();

        //channel_in1 = gate("in1")->getTransmissionChannel();
    }
    if (strcmp("node5", getName()) == 0) {
        channel = gate("out1")->getTransmissionChannel();
        channel_out2 = gate("out2")->getTransmissionChannel();
        channel_out3 = gate("out3")->getTransmissionChannel();
    }
    if (strcmp("node3", getName()) == 0) {
        channel = gate("out1")->getTransmissionChannel();
        //channel_in1 = gate("in1")->getTransmissionChannel();
    }
    if (strcmp("node4", getName()) == 0) {
        channel = gate("out1")->getTransmissionChannel();
        //channel_in1 = gate("in1")->getTransmissionChannel();
    }


    //channel = gate("out1")->getTransmissionChannel();
    //channel_in1 = gate("in1")->getTransmissionChannel();
    //channel_out2 = gate("out2")->getTransmissionChannel();
    //channel_in2 = gate("in2")->getTransmissionChannel();
    queue = new cQueue("node_ext_queue");


   // channel = gate("out")->getTransmissionChannel();
   //   queue = new cQueue("node_ext_queue");
      confirmationQueue = new cQueue("node_ext_queue_ack_queue");
      GbnWindowQueue = new cQueue("GBN-Window-queue");
      GbnConfirmationQueue = new cQueue("GBN-confirmation-queue");
      WATCH(estado);
      WATCH(estadoGBN);


}


void node::handleMessage(cMessage *msg){


    paquete *msg_cp = (paquete*) msg->dup();
    if (strcmp("node2", getName()) == 0) {

        if(msg->arrivedOn("in1")){
              EV << getName()<< ": " << "gate:" <<msg->getArrivalGateId()<<" \n";
              s_w_receriver(msg,"in1","out1");

          }

          if(msg->arrivedOn("in4")){
              EV << getName()<< ": " << "gate:" <<msg->getArrivalGateId()<<" \n";
              s_w_receriver(msg,"in4","out4");
          }


        if(strcmp(msg->getFullName(), "ackMsg")==0){
                     s_w_receriver(msg,msg->par("input"),msg->par("output"));
                 }


          if((rand()%100)<50){
             s_w_sender(msg_cp, "packet_in", "in2", "out2", channel_out2);
           }else{
               s_w_sender(msg_cp, "packet_in", "in3", "out3", channel_out3);
           }





          }


    if(strcmp("node5", getName()) == 0){

        //paquete *msg_cp = (paquete*) msg->dup();
        //s_w_receriver(msg,"in1","out1");


        if(msg->arrivedOn("in1")){
              EV << getName()<< ": " << "gate:" <<msg->getArrivalGateId()<<" \n";
              s_w_receriver(msg,"in1","out1");

          }


        if((rand()%100)<50){
            s_w_sender(msg_cp, "packet_in", "in2", "out2", channel_out2);
          }else{
              s_w_sender(msg_cp, "packet_in", "in3", "out3", channel_out3);
          }




        if(strcmp(msg->getFullName(), "ackMsg")==0){
             s_w_receriver(msg,msg->par("input"),msg->par("output"));
         }

    }



    if (strcmp("node3", getName()) == 0) {

              s_w_receriver(msg,"in1","out1");
          }

    if (strcmp("node4", getName()) == 0) {

        if(msg->arrivedOn("in1")){
            EV << getName()<< ": " << "gate:" <<msg->getArrivalGateId()<<" \n";
            s_w_receriver(msg,"in1","out2");

        }
        if(msg->arrivedOn("in2")){
            EV << getName()<< ": " << "gate:" <<msg->getArrivalGateId()<<" \n";
            s_w_receriver(msg,"in2","out1");
        }
        if(strcmp(msg->getFullName(), "ackMsg")==0){
            s_w_receriver(msg,msg->par("input"),msg->par("output"));
        }



      }


}

void node::gbn_receiver(cMessage *msg){
    if(msg->arrivedOn("in1")){// mirar si el trafico es n
                EV << getName()<< ": " << "message arrived to in\n";
               paquete *packet = check_and_cast<paquete*>(msg);
                sscanf(packet->getName(), "packet-%d",&packet_number);
                EV << getName() << ":" << "packet number:"<< packet_number <<"\n";
                ackMessage = new cMessage("ackMsg");
                ackMessage->addPar("seqNum").setLongValue(packet_number);

                if(channel->isBusy()){// si el canal esta ocupado sceduleamos
                   //drop
                    //simtime_t txFinishTime = channel->getTransmissionFinishTime();
                    //scheduleAt(txFinishTime+exponential(1),ackMessage);
                }else{//sino mandamos directamente
                    scheduleAt(simTime(),ackMessage);

                }
            }
         else if(msg->isSelfMessage()){
             EV << getName() << ":" << "Handling self-message:"<< msg->getFullName() <<"\n";
             if(strcmp(msg->getFullName(), "ackMsg")==0){
                 //sendAck(msg->par("seqNum").longValue(),input,output);
             }
         }

}

int node::s_w_receriver(cMessage *msg,const char* input,const char *output){

    if(msg->arrivedOn(input)){// mirar si el trafico es n
             EV << getName()<< ": " << "message arrived to in\n";
            paquete *packet = check_and_cast<paquete*>(msg);
             sscanf(packet->getName(), "packet-%d",&packet_number);
             EV << getName() << ":" << "packet number:"<< packet_number <<"\n";
             ackMessage = new cMessage("ackMsg");
             ackMessage->addPar("seqNum").setLongValue(packet_number);
             ackMessage->addPar("input").setStringValue(input);
             ackMessage->addPar("output").setStringValue(output);
             if(channel->isBusy()){// si el canal esta ocupado sceduleamos
                 simtime_t txFinishTime = channel->getTransmissionFinishTime();
                 scheduleAt(txFinishTime+exponential(1),ackMessage);
             }else{//sino mandamos directamente
                 scheduleAt(simTime()+exponential(1),ackMessage);

             }
         }
      else if(msg->isSelfMessage()){
          EV << getName() << ":" << "Handling self-message:"<< msg->getFullName() <<"\n";
          if(strcmp(msg->getFullName(), "ackMsg")==0){
              int confirmed;
              confirmed = sendAck(msg->par("seqNum").longValue(),input,output);
              if(confirmed==1){
              return 1;
              }
              else{
               return 0;
              }
          }
      }
    return 1;
}

int node::sendAck(int seqNum, const char* input,const char *output){

    char ack_name[50];


    if((rand()%100)>0){ //probabilidad de no devolver paquete pqueloss..

        if((rand()%100)<0){ //probabilidad de nak
                        //nack
                    EV << getName() << ":" << "Sending nack\n";
                    sprintf(ack_name, "nack-%d", seqNum);
                    paquete *nack = new paquete(ack_name,2);
                    nack->setBitLength(1);
                    nack->setSeq(seqNum);
                    nack->setType(2);
                    int inputNumber;
                    int outputNumber;
                    sscanf (input,"in%d",&inputNumber);
                    sscanf (output,"out%d",&outputNumber);
                    nack->setInput(inputNumber);
                    nack->setOutput(outputNumber);
                    send(nack,output);

                    return 0;
                }else{
                    //ack
                EV << getName() << ":" << "Sending ack\n";
                sprintf(ack_name, "ack-%d", seqNum);
                paquete *ack = new paquete(ack_name,1);
                ack->setBitLength(1);
                ack->setSeq(seqNum);
                ack->setType(1);
                int inputNumber;
                int outputNumber;
               sscanf (input,"in%d",&inputNumber);
               sscanf (output,"out%d",&outputNumber);
               ack->setInput(inputNumber);
               ack->setOutput(outputNumber);
                send(ack,output);
                return 1;
                //enviar paquete a siguiente red
            }

    }else{//pierde pqt
        EV << getName() << ":" << "loosing packet\n";
        return 0;
    }

}




/*
void node::sendCopyOf(paquete *msg)
{
    //Duplicar el mensaje y mandar una copia
    paquete *copy = (paquete*) msg->dup();
    send(copy, "out");
}
*/

void node::s_w_sender(cMessage *msg,const char* source ,const char *input,const char *output, cChannel *outChannel){

    paquete *pck;
    if(msg->arrivedOn(input)){
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
            sendCopyOf(pck,output,outChannel);
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
                       sendCopyOf(pck,output,outChannel);
                   }
               }else{

                   //si justo viene un packete
                   if(pck->getType()==0){
                   EV << getName()<< "sending queue packet\n";
                   queue->insert(pck);
                   pck = (paquete *)queue->pop();
                   sendCopyOf(pck,output,outChannel);
                   }else{
                       pck = (paquete *)queue->pop();
                       sendCopyOf(pck,output,outChannel);
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
            if(confirmationQueue->length()>0){
                confirmationQueue->pop();
            }
            estado=READY_TO_SEND;
            //delete pck;
            readyTosend=new cMessage("readyToSend");
            scheduleAt(simTime(),readyTosend);
            //enviar que estamos ready to send

        }else { //es un nack
            estado=SENDING;
            pck = (paquete*)confirmationQueue->pop();
            //aqui no se pasa a ready to send, ya que es prioritario transmitir el paquete anterior
            sendCopyOf(pck,output,outChannel);
        }

    }
}

void node::sendCopyOf(paquete *msg, const char* output, cChannel* outChannel)
{


    estado=SENDING;
    EV << getName()<< ": " << "sendCopy of "<<msg->getFullName()<<"\n";
    //Duplicar el mensaje y mandar una copia
    confirmationQueue->insert(msg);
    paquete *copy = (paquete*) msg->dup();
    send(copy, output);
    simtime_t txFinishTime = outChannel->getTransmissionFinishTime();
    pckTxTime=new cMessage("pckTxTime");
    scheduleAt(txFinishTime,pckTxTime); //paquete ha llegado
    timerEvent = new cMessage("timer");
    //scheduleAt(txFinishTime*10,timerEvent); //timer para repeticion

}
/*
void node::sendCopyOf(paquete *msg)
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



void node::refreshDisplay() const
    {
        char buf[40];
        //stop wait
        sprintf(buf, "state: %d\nCola: %d\nconf: %d", estado,queue->getLength(),confirmationQueue->getLength());
        getDisplayString().setTagArg("t", 0, buf);

        //GBN
        //sprintf(buf, "state: %d\nCola: %d\nVentana: %d", estadoGBN,queue->getLength(),N-GbnWindowQueue->getLength());
        //getDisplayString().setTagArg("t", 0, buf);
    }



