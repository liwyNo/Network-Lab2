#ifndef CLICK_FILEAPP_HH
#define CLICK_FILEAPP_HH

#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class FileApp : public Element
{
private:
	char *buf;
public:
	FileApp();
	~FileApp();

	const char *class_name() const { return "FileApp"; }
	const char *port_count() const { return "2/2"; }

	int configure(Vector<String>&, ErrorHandler*);
	void push(int port, Packet *);
};

CLICK_ENDDECLS

#endif
