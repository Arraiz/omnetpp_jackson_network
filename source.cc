#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class source : public cSimpleModule
{
  public:
    virtual ~source();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;


  private:
    cMessage *msgEvent;

};

// The module class needs to be registered with OMNeT++
Define_Module(source);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
source::~source(){
    cancelAndDelete(msgEvent);
}

void source::initialize()
{

    //generate the traffic inyection a lambda rate
    EV << "MD: Starting Source\n";
    msgEvent = new cMessage("MD: Packet Source");
    scheduleAt(0, msgEvent);


}


void source::handleMessage(cMessage *msg)
{

    EV << "MD: Event received, sending packet\n";
    cancelAndDelete(msgEvent);
    msgEvent = new cMessage("MD: Sending Packet");
    // generate & send packet
    cMessage *pkt = new cMessage;
    //send(pkt, "out");
    // schedule next call
    scheduleAt(simTime()+exponential(1.0), msgEvent);
}








