#ifndef CLICK_FILEAPP_HH
#define CLICK_FILEAPP_HH

#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class FileApp : public Element
{
private:
	String sendfile, recvfile;
	Timer timer1;
public:
	FileApp();
	~FileApp();

	const char *class_name() const { return "FileApp"; }
	const char *port_count() const { return "-1/-1"; }

	int configure(Vector<String>&, ErrorHandler*);
	void push(int port, Packet *);
	void send();
	void run_timer(Timer * timer);
};

CLICK_ENDDECLS

#endif
