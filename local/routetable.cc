#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "routetable.hh"
#include "ipheader.hh"
#include <cstdio>
CLICK_DECLS

RouteTable::RouteTable(){}
RouteTable::~RouteTable(){}

int RouteTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}
void RouteTable::push(int, Packet * p){
    output(0).push(p);
}
CLICK_ENDDECLS
EXPORT_ELEMENT(RouteTable)
