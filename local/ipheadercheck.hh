#ifndef CLICK_IPHEADERCHECK_HH
#define CLICK_IPHEADERCHECK_HH

#include <click/element.hh>

CLICK_DECLS

class IPHeaderCheck : public Element {

public:
	IPHeaderCheck();
	~IPHeaderCheck();
	
	const char *class_name() const { return "IPHeaderCheck";}
	const char *port_count() const { return "1/1"; }
	
	int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);
    uint16_t GetIpChk(unsigned char *ptr, int size);
};

CLICK_ENDDECLS

#endif
