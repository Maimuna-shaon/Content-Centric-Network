
package phoenix;

moduleinterface IForwarder
{
    parameters:
        @display("i=block/fork;t=Forwarding,l");

    gates:
        inout appInOut[];
        inout transportInOut[];
}
