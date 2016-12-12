#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include <click/timer.hh>
#include <click/args.hh>
#include "tcprouter.hh"
#include "ipheader.hh"
#include <cstdio>
CLICK_DECLS

TCPRouter::TCPRouter(){}
TCPRouter::~TCPRouter(){}

int TCPRouter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}
uint16_t TCPRouter::GetIpChk(unsigned char *ptr, int size)
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
void TCPRouter::push(int port, Packet * p){
	if(port == 0){
		tcpheader * th;
		th = (tcpheader *)p->data();
		if(pair_map.find(th->des_port) == pair_map.end()){
			click_chatter("TCPRouter Unknown TCP port pair. %d %d\n", th->src_port, th->des_port);
			p->kill();
			return;
		}
		if(pair_map[th->des_port] == th->src_port)
			output(port_map[th->des_port]).push(p);
		else{
			click_chatter("TCPRouter receive wrong packet %d %d.\n", p->length(), port);
			p->kill();
		}
	}
	else{
		if(p->length() == 4 || p->length() == 4 + IPHEADERSIZE){
			if(p->length() == 4 + IPHEADERSIZE){
				ipheader * ip_h = (ipheader *)p->data();
				if(ip_h->Version != 4 || ip_h->IHL != 5 || ip_h->TotalLength != p->length() ||
					ip_h->TTL == 0 || ip_h->CheckSum != GetIpChk((unsigned char*)p->data(), IPHEADERSIZE)){
					p->kill();
					return;
				}
			}
			pair_port *pp;
			if(p->length() == 4 + IPHEADERSIZE)
				pp = (pair_port *)(p->data() + IPHEADERSIZE);
			else
				pp = (pair_port *)p->data();
			port_map[pp->src] = port;
			pair_map[pp->src] = pp->des;
			click_chatter("Add new pair %d %d.\n", pp->src, pp->des);
		}
		else{
			output(0).push(p);
		}
	}
}
CLICK_ENDDECLS
EXPORT_ELEMENT(TCPRouter)