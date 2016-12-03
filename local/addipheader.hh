#ifndef CLICK_ADDIPHEADER_HH
#define CLICK_ADDIPHEADER_HH

#include <click/element.hh>

CLICK_DECLS

class AddIPHeader : public Element {

public:
	AddIPHeader();
	~AddIPHeader();
	
	const char *class_name() const { return "AddIPHeader";}
	const char *port_count() const { return "1/1"; }
	
	int configure(Vector<String>&, ErrorHandler*);
    Packet * simple_action(Packet *);

};

CLICK_ENDDECLS

#endif
