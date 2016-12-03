#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "addipheader.hh"
#include "ipheader.hh"
#include <cstdio>
CLICK_DECLS

AddIPHeader::AddIPHeader(){}
AddIPHeader::~AddIPHeader(){}

int AddIPHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}

Packet * AddIPHeader::simple_action(Packet *p){
    WritablePacket * q = p->push(IPHEADERSIZE);
    if(!q){
        q = Packet::make(p->length() + IPHEADERSIZE);
    }
    ipheader * ip_header = (ipheader *)(q -> data());
    ip_header->initialize();
    ip_header->TotalLength = q->length();
    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddIPHeader)
