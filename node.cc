#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class node : public cSimpleModule
{
  public:
    virtual ~node();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;


  private:
    cMessage *msgEvent;

};

// The module class needs to be registered with OMNeT++
Define_Module(node);

/*Destructor... Used to remove "undisposed object:...basic.source.pck-x"*/
node::~node(){
    cancelAndDelete(msgEvent);
}

void node::initialize()
{




}


void node::handleMessage(cMessage *msg)
{
    EV << "MD: Message Arrived: to node1" ;

}



