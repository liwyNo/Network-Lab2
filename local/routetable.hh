#ifndef CLICK_ROUTETABLE_HH
#define CLICK_ROUTETABLE_HH

#include <click/element.hh>

CLICK_DECLS

class RouteTable : public Element {

public:
	RouteTable();
	~RouteTable();
	
	const char *class_name() const { return "RouteTable";}
	const char *port_count() const { return "1/1"; }
	
	int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);
};

CLICK_ENDDECLS

#endif
