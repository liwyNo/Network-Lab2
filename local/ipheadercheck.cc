#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "ipheadercheck.hh"
#include "ipheader.hh"
#include <cstdio>
CLICK_DECLS

IPHeaderCheck::IPHeaderCheck(){}
IPHeaderCheck::~IPHeaderCheck(){}

int IPHeaderCheck::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}
uint16_t IPHeaderCheck::GetIpChk(unsigned char *ptr, int size)
{
	int cksum = 0;
	int index = 0;
	while(index < size)
	{
        if(index == 10){
            index += 2;
            continue;
        }
		cksum += *(ptr + index + 1);
		cksum += *(ptr + index) << 8;
		index += 2;
	}
	while(cksum > 0xffff)
	{
		cksum = (cksum >> 16) + (cksum & 0xffff);
	}
	cksum = ~cksum;
	return (uint16_t)cksum;
}
void IPHeaderCheck::push(int, Packet * p){
    if(p->length() < 20){
        p->kill();
        return;
    }
    ipheader * ip_h = (ipheader *)p->data();
    if(ip_h->Version != 4 || ip_h->IHL != 5 || ip_h->TotalLength != p->length() ||
       ip_h->TTL == 0 || ip_h->CheckSum != GetIpChk((unsigned char*)p->data(), IPHEADERSIZE)){
        p->kill();
        return;
    }
    output(0).push(p);
}
CLICK_ENDDECLS
EXPORT_ELEMENT(IPHeaderCheck)
