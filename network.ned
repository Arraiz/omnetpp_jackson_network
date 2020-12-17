import ned.IdealChannel;
import ned.DatarateChannel;


//Mikel Diez Garcia (Arraiz)
simple source
{
    parameters:
        bool is_source=true;
       	int lambda=1; 
    @display("i=block/source");
    gates:
        output out;
}

simple node_ext
{
    @display("i=block/routing");
    gates:
        input packet_in;
        input in;
        output out;
}
simple node
{
    @display("i=block/routing");
    gates:
        input in;
        output out;
}


network Two_node
{
    @display("bgb=546.18665,214.57333");
    types:
        channel eth10 extends DatarateChannel
        {
            datarate = 9600bps;
            per = 0.5;
        }
        channel eth100 extends DatarateChannel
        {
            datarate = 9600bps;
            per = 0.5;
        }
    submodules:
        source: source {
            @display("p=22.4,98.7");
        }
        node1: node_ext {
            @display("p=156.94,98.42");
        }
        node2: node {
            @display("p=311.22,98.42");
        }
    connections:
        source.out --> node1.packet_in;
        node1.out --> eth100 --> node2.in;
        node2.out --> eth100 --> node1.in;

}

network Network
{
    @display("bgb=546.18665,214.57333");
    types:
        channel eth10 extends DatarateChannel
        {
            datarate = 9600bps;
            per = 0.5;
        }
        channel eth100 extends DatarateChannel
        {
            datarate = 9600bps;
            per = 0.5;
        }
    submodules:
        source: source {
            @display("p=22.4,98.7");
        }
        node1: node_ext {
            @display("p=156.94,98.42");
        }
        node2: node {
            @display("p=311.22,98.42");
        }
    connections:
        source.out --> node1.packet_in;
        node1.out --> eth100 --> node2.in;
        node2.out --> eth100 --> node1.in;

}






