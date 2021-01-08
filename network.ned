import ned.IdealChannel;
import ned.DatarateChannel;


//Mikel Diez Garcia (Arraiz)

// Fuente simple
simple source
{
    parameters:
        bool is_source = true;
        int lambda = 1;
        @display("i=block/source");
    gates:
        output out;
}


// Nodo exterior, como si fuese un router frontera
simple node_ext
{
    @display("i=block/routing");
    gates:
        input packet_in;
        input in;
        output out;
}

// Nodo del core de la red
simple node
{
    @display("i=block/routing");
    gates:
        input in;
        output out;
        input in2;
        output out2;
}




//Red con dos nodos 
network Two_Nodes
{
    @display("bgb=546.18665,214.57333");
    types:
        channel eth10 extends DatarateChannel
        {
            datarate = 5bps;
        }
        channel eth100 extends DatarateChannel
        {
            datarate = 9600bps;
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
        node3: node {
            @display("p=457,98");
        }
    connections:
        source.out --> node1.packet_in;
        node1.out --> eth100 --> node2.in;
        node2.out --> eth10 --> node1.in;
        node2.out2 --> eth100 --> node3.in;
        node3.out --> eth100 --> node2.in2;

        node3.out2 --> eth100 --> node3.in2;

}








