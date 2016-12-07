#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "desipfilter.hh"
#include "ipheader.hh"
#include <cstdio>
CLICK_DECLS

DesIPFilter::DesIPFilter(){}
DesIPFilter::~DesIPFilter(){}

int DesIPFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh, "IP", cpkM, cpUnsigned, &dest_ip, cpEnd) < 0) return -1;
	return 0;
}
void DesIPFilter::push(int, Packet * p){
    if(p->length() < 20){
        p->kill();
        return;
    }
    ipheader * ip_h = (ipheader *)p->data();
    if(ip_h->DesIP != dest_ip)
        output(1).push(p);
    else
        output(0).push(p);
}
CLICK_ENDDECLS
EXPORT_ELEMENT(DesIPFilter)
