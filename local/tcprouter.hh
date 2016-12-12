#ifndef CLICK_TCPROUTER_HH
#define CLICK_TCPROUTER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include <map>
#include "tcpheader.hh"
CLICK_DECLS

class TCPRouter : public Element {
private:
	std::map<unsigned, unsigned> port_map;
	std::map<unsigned, unsigned> pair_map;
	
public:
	TCPRouter();
	~TCPRouter();
	
	const char *class_name() const { return "TCPRouter";}
	const char *port_count() const { return "1-/="; }
	const char *processing() const { return PUSH; }
	uint16_t GetIpChk(unsigned char *ptr, int size);
	
	int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);
};
struct pair_port{
	unsigned short src;
	unsigned short des;
};
CLICK_ENDDECLS
#endif
