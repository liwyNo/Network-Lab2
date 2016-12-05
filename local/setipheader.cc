#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "setipheader.hh"
#include "ipheader.hh"
#include <cstdio>
CLICK_DECLS

SetIPHeader::SetIPHeader(){}
SetIPHeader::~SetIPHeader(){}

int SetIPHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
	protocol = 6, src_ip = 0, dest_ip = 0;
	if (cp_va_kparse(conf, this, errh, 
		"PROTOCOL", 0, cpUnsigned, &protocol,
		"SOURCE_IP", cpkM, cpUnsigned, &src_ip,
		"DEST_IP", cpkM, cpUnsigned, &dest_ip,
		cpEnd) < 0) return -1;

	return 0;
}

uint16_t SetIPHeader::GetIpChk(unsigned char *ptr, int size)
{
	int cksum = 0;
	int index = 0;
	while(index < size)
	{
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

Packet * SetIPHeader::simple_action(Packet *p){
    WritablePacket * q = p->push(IPHEADERSIZE);
    if(!q){
        q = Packet::make(p->length() + IPHEADERSIZE);
    }
    ipheader * ip_header = (ipheader *)(q -> data());
    ip_header->initialize();
    ip_header->Protocol = (uint8_t)protocol;
    ip_header->SrcIP = src_ip;
    ip_header->DesIP = dest_ip;
    ip_header->TotalLength = q->length();
    ip_header->CheckSum = GetIpChk(q->data(), IPHEADERSIZE);
    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SetIPHeader)
