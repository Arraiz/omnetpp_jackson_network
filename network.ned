//Mikel Diez Garcia (Arraiz)
simple source
{
    @display("i=block/source");
    //gates:
    //    output out;
}

//simple node
//{
//    @display("i=block/routing");
//    gates:
//        input in;
//        output out;
//}



network Network
{
    @display("bgb=206,185");
    submodules:
        source: source {
            @display("p=17.5,79.1");
        }
        //node1: node {
        //    @display("p=91.7,79.1");
        //}
    //connections:
        //source.out --> {  delay = 100ms; } --> node1.in;
}
