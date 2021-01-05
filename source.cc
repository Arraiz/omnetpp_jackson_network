#include <string.h>
#include <omnetpp.h>
#include "paquete_m.h"
using namespace omnetpp;


class source : public cSimpleModule
{
  public:
    virtual ~source();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual paquete *buildPacket();

  private:
    paquete *newPacket;
    int packet_counter=0;

};

// The module class needs to be registered with OMNeT++
Define_Module(source);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
source::~source(){
    cancelAndDelete(newPacket);
}

void source::initialize()
{

    if(par("is_source")){//parametro definido en el NED
        //Inicualizamos la fuente, contruimos el primer paquete y scheduleamos su envio
        EV << getName()<<":"<<" Started\n";
        newPacket = buildPacket();
        scheduleAt(simTime()+exponential(1.0),newPacket);
    }

}


void source::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){
        // Enviamos el paquete previamente scheduleado contruimos otro y lo volvemos a schedulear
        EV << getName()<<":"<<" sending packet\n";
        send(newPacket, "out");
        newPacket = buildPacket();
        scheduleAt(simTime()+exponential(1.0),newPacket);
    }


}

paquete *source::buildPacket(){
    char packet_name[50];
    packet_counter++;
    sprintf(packet_name, "packet-%d", packet_counter);
    EV << getName()<<":"<<"building packet: "<<packet_name<<"\n";
    paquete *buildedPck = new paquete(packet_name,0);
    buildedPck->setSeq(packet_counter);
    buildedPck -> setBitLength(1024);
    buildedPck->setType(0);
    return buildedPck;
}

void source::finish(){
    cancelAndDelete(newPacket);
}







