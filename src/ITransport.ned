package phoenix;


moduleinterface ITransport
{
    parameters:
        @display("i=block/transport;t=Transport,l");

    gates:
        inout forwarderInOut;
        inout physicalInOut;
        input radioIn @directIn;
}
