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
        input in2;
        output out2;
}

// Nodo del core de la red
simple node
{
    @display("i=block/routing");
    gates:
        input in1;
        output out1;
        input in2;
        output out2;
        input in3;
        output out3;
        input in4;
        output out4;
}




//Red con dos nodos 

network Two_Nodes
{
    @display("bgb=546.18665,214.57333");
    types:
        channel eth10 extends DatarateChannel
        {
            datarate = 10bps;
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
        //source.out --> node1.packet_in;
        //node1.out --> eth100 --> node2.in;
        //node2.out --> eth10 --> node1.in;
        //node2.out2 --> eth100 --> node3.in;
        //node3.out --> eth100 --> node2.in2;
        //node3.out2 --> eth100 --> node3.in2;

}

//Final net
network FinalNet
{
    @display("bgb=772,337");
    types:
        channel eth10f extends DatarateChannel
        {
            datarate = 10000bps;
        }
        channel eth100f extends DatarateChannel
        {
            datarate =  10000bps;
        }
    submodules:
        source: source {
            @display("p=59,182");
        }
        node_ext1: node_ext {
            @display("p=129,182");
        }
        node2: node {
            @display("p=238,39");
        }
        node5: node {
            @display("p=238,297");
        }
        node3: node {
            @display("p=561,39");
        }
        node4: node {
            @display("p=568,297");
        }
    connections allowunconnected:
        source.out --> node_ext1.packet_in;

        node_ext1.out --> eth100f --> node2.in1;
        node_ext1.out2 --> eth100f --> node5.in1;

        node2.out1 --> eth100f --> node_ext1.in;
        node2.out2 --> eth100f --> node3.in1;
        node2.out3 --> eth100f --> node4.in1;
        node2.out4 --> eth100f --> node5.in3;

        node5.out1 --> eth100f --> node_ext1.in2;
        node5.out2 --> eth100f --> node4.in2;
        node5.out3 --> eth100f --> node2.in4;

        node4.out1 --> eth100f --> node5.in2;
        node4.out2 --> eth100f --> node2.in3;
        //node2.out1 --> eth100f --> node_ext1.in;
        //node5.out1 --> eth100f --> node_ext1.in2;


        //node2.out2 --> eth100 --> node3.in;
        node3.out1 --> eth100f --> node2.in2;
        //node3.out2 --> eth100 --> node3.in2;

}








