#ifndef CLICK_MYROUTER_HH
#define CLICK_MYROUTER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include "dijkstra.hh"
#include <map>
#include <set>
#define GAPTIME 5
CLICK_DECLS

class MyRouter : public Element {
private:
    std::map<unsigned, std::set<unsigned> > v_map;
    std::map<unsigned, neighbor> nb;
	unsigned netSize;
	Timer _timer;
	unsigned ip;
	unsigned current_loop;
	
public:
	MyRouter();
	~MyRouter();
	
	const char *class_name() const { return "MyRouter";}
	const char *port_count() const { return "1-/="; }
	const char *processing() const { return PUSH; }
	
	int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet*);
	int initialize(ErrorHandler*);
	void run_timer(Timer *);
	void TellNeighbor();
	void TellRouteTable();
	void broadcastall(Packet* p, int );
	void updateNeighborStat();
	void updateRouteTable(int);
	void mergeRouteTable(Packet*);
	uint16_t GetIpChk(unsigned char *ptr, int size);
	void route(Packet *);
	int dijkstra(unsigned);
};

CLICK_ENDDECLS

#endif
