import ned.IdealChannel;
import ned.DatarateChannel;


//Mikel Diez Garcia (Arraiz)
simple source
{
    @display("i=block/source");
    gates:
        output out;
}

simple node
{
    @display("i=block/routing");
    gates:
        input in;
        //output out;
}



network Network
{
    @display("bgb=206,185");
    types:
        channel eth10 extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 50ms;
            per = 0.03;
        }
        channel eth100 extends DatarateChannel
        {
            datarate = 100Mbps;
            delay = 50ms;
            per = 0.03;
        }
    submodules:
        source: source {
            @display("p=27.3,79.1");
        }
        node1: node {
            @display("p=168.7,79.1");
        }
    connections:
        source.out --> node1.in;
}
