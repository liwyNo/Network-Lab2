#ifndef CLICK_DESIPFILTER_HH
#define CLICK_DESIPFILTER_HH

#include <click/element.hh>

CLICK_DECLS

class DesIPFilter : public Element {

private:
    unsigned dest_ip;
public:
	DesIPFilter();
	~DesIPFilter();
	
	const char *class_name() const { return "DesIPFilter";}
	const char *port_count() const { return "1/2"; }
	
	int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);
};

CLICK_ENDDECLS

#endif
