#ifndef CLICK_SETIPHEADER_HH
#define CLICK_SETIPHEADER_HH

#include <click/element.hh>

CLICK_DECLS

class SetIPHeader : public Element {

private:
	unsigned src_ip, dest_ip, protocol;

public:
	SetIPHeader();
	~SetIPHeader();
	
	const char *class_name() const { return "SetIPHeader";}
	const char *port_count() const { return "1/1"; }
	
	int configure(Vector<String>&, ErrorHandler*);
        Packet * simple_action(Packet *);
        uint16_t GetIpChk(unsigned char *ptr, int size);

};

CLICK_ENDDECLS

#endif
