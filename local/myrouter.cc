#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include <click/timer.hh>
#include "myrouter.hh"
#include "dijkstra.hh"
#include "ipheader.hh"
#include <cstdio>
#include <queue>
#include <algorithm>
CLICK_DECLS

MyRouter::MyRouter():_timer(this), current_loop(0){}
MyRouter::~MyRouter(){}

int MyRouter::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh, "IP", cpkM, cpUnsigned, &ip, cpEnd) < 0) return -1;
	return 0;
}
int MyRouter::initialize(ErrorHandler*){
	_timer.initialize(this);
	_timer.schedule_after_sec(GAPTIME);
	v_map[ip] = std::set<unsigned>();
	netSize = 0;
	TellNeighbor();
    return 0;
}
uint16_t MyRouter::GetIpChk(unsigned char *ptr, int size)
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
void MyRouter::push(int port, Packet * p){
    //Packet is too small
	if(p->length() < sizeof(ripheader)){
		p->kill();
        return;
	}
	//IP Header
	if(p->length() >= 20){
		ipheader * ip_h = (ipheader *)p->data();
		if(ip_h->Version == 4 && ip_h->IHL == 5 && ip_h->TotalLength == p->length() &&
		   ip_h->TTL != 0){
			//packet is a IP packet
			if(ip_h->CheckSum != GetIpChk((unsigned char*)p->data(), IPHEADERSIZE)){
                click_chatter("%d recived a invalid IP packet. %04x %04x \n", ip, ip_h->CheckSum, GetIpChk((unsigned char*)p->data(), IPHEADERSIZE));
				p->kill();
				return;
			}
			if(port == 0){
				//from IP layer to lower layer
				route(p);
				return;
			}
			//from lower layer to upper layer
			if(ip_h->DesIP == ip){
				//packet to me!!!!
                p = p->uniqueify();
				p->pull(sizeof(ipheader));
				output(0).push(p);	
				return;
			}
			else{
				//forward this packet
				route(p);
				return;
			}
		}
	}
	ripheader * rip = (ripheader*)p->data();
	//rip header
	if(rip->version != 1 || rip->magic != RIPMAGIC || rip->size != p->length() || rip->size != rip->entry_num * sizeof(entry) + sizeof(ripheader)){
		//invaild rip packet;
		p->kill();
		return;
	}
	if(rip->command == HELLOMSG){
		unsigned nbip = rip->src_ip;
		if(nb.find(nbip) == nb.end()){
			neighbor tmp;
			tmp.ip = nbip;
			tmp.stat = NEWSTAT;
			tmp.port = port;
			nb[nbip] = tmp;
			updateRouteTable(nbip);
		}
		else{
			nb[nbip].stat = NEWSTAT;
			nb[nbip].port = port;
		}
	}
	if(rip->command == ROUTEMSG){
		mergeRouteTable(p);
	}
	//other is invaild
	p->kill();
}
void MyRouter::TellNeighbor(){
	ripheader hellomsg;
	hellomsg.command = HELLOMSG;
	hellomsg.version = 1;
	hellomsg.magic = RIPMAGIC;
	hellomsg.size = sizeof(ripheader);
	hellomsg.entry_num = 0;
	hellomsg.src_ip = ip;
	
	WritablePacket * p = Packet::make(&hellomsg, sizeof(ripheader));
	broadcastall((Packet*)p, 0);
	p->kill();
}
void MyRouter::run_timer(Timer * timer){
	if(current_loop == 0)
		TellRouteTable();
	current_loop = (current_loop + 1) % 6;
	TellNeighbor();
	updateNeighborStat();
	timer->reschedule_after_sec(GAPTIME);
}
void MyRouter::TellRouteTable(){
	ripheader routemsg;
	routemsg.command = ROUTEMSG;
	routemsg.version = 1;
	routemsg.magic = RIPMAGIC;
	routemsg.size = sizeof(ripheader) + netSize * sizeof(entry);
	routemsg.entry_num = netSize;
	routemsg.src_ip = ip;
	
	WritablePacket * p = Packet::make(&routemsg, sizeof(ripheader));
	p = p->put(netSize * sizeof(entry));
	entry * e_p = (entry*)(((char*)p->data()) + sizeof(ripheader));
	unsigned count = 0;
	for(std::map<unsigned, std::set<unsigned> >::iterator x = v_map.begin(); x != v_map.end(); ++x){
		for(std::set<unsigned>::iterator y = (*x).second.begin(); y != (*x).second.end(); ++y){
			if((*x).first < *y){
				e_p[count++] = entry((*x).first, (*y));
			}
		}
	}
	if(count != netSize){
		click_chatter("count is not eq netSize");
		p->kill();
		return;
	}
	broadcastall((Packet*)p, 0);
	p->kill();
}
void MyRouter::broadcastall(Packet * p, int ignore_port = -1){
	int nout = noutputs();
	for(int i = 1; i < nout; ++i){
		if(i != ignore_port)
			output(i).push(p->clone());
	}
}
void MyRouter::updateNeighborStat(){
/* 	bool changeFlag = false;
	for(auto x : nb){
		switch(x.second.stat){
			case NEWSTAT:
				x.second.stat = SUSPSTAT;
				break;
			case SUSPSTAT:
				x.second.stat = FAILSTAT;
				changeFlag = true;
				break;
		}
	}
	if(changeFlag)
		updateRouteTable(); */
}
void MyRouter::updateRouteTable(int new_nb){
/* 	v.clear();
	for(auto x : nb){
		if(x.second.stat != FAILSTAT)
			v.emplace(ip, x.first);
	}
	for(auto x : v_map){
		int src = x.first;
		if(nb[src].)
		Vector<unsigned> & tmp_map = x.second;
		for(auto y : tmp_map)
			v.emplace(src, y);
	}
	std::sort(v.begin(), v.end());
	std::unique(v.begin(), v.end());
	TellRouteTable(); */
	v_map[ip].insert(new_nb);
	v_map[new_nb] = std::set<unsigned>();
	v_map[new_nb].insert(ip);
	dijk_map.clear();
	netSize++;
	TellRouteTable();
}
void MyRouter::mergeRouteTable(Packet* p){
	ripheader * rip = (ripheader*)p->data();
	int entry_num = rip->entry_num;
	entry * e_p = (entry*)(((char*)p->data()) + sizeof(ripheader));
	for(int i = 0; i < entry_num; ++i){
		int src = e_p[i].src;
		int des = e_p[i].des;
		if(v_map.find(src) == v_map.end())
			v_map[src] = std::set<unsigned>();
		if(v_map.find(des) == v_map.end())
			v_map[des] = std::set<unsigned>();
		v_map[src].insert(des);
		v_map[des].insert(src);
	}
	unsigned newSize = 0;
	for(std::map<unsigned, std::set<unsigned> >::iterator x = v_map.begin(); x != v_map.end(); ++x)
		newSize += (*x).second.size();
	newSize /= 2;
	if(newSize != netSize){
		netSize = newSize;
		dijk_map.clear();
		TellRouteTable();
	}
	netSize = newSize;
}
void MyRouter::route(Packet * p){
	ipheader * ip_h = (ipheader*)p->data();
	unsigned des = ip_h->DesIP;
	int _port;
	if(dijk_map.find(des) == dijk_map.end())
		dijk_map[des] = dijkstra(des);
	_port = dijk_map[des];
	if(_port == -1){
		//p->kill();
        return;
    }
	output(_port).push(p);
}
int MyRouter::dijkstra(unsigned des){
	//actually I wirte a BFS
	std::queue<unsigned> _nb;
	std::queue<unsigned> _point;
	std::map<unsigned, bool> vis;
	for(std::map<unsigned, std::set<unsigned> >::iterator x = v_map.begin(); x != v_map.end(); ++x)
		vis[(*x).first] = false;
	for(std::set<unsigned>::iterator x = v_map[ip].begin(); x != v_map[ip].end(); ++x){
		if(*x == des)
			return nb[des].port;
		vis[*x] = true;
		for(std::set<unsigned>::iterator y = v_map[*x].begin(); y != v_map[*x].end(); ++y){
			_nb.push(*x);
			_point.push(*y);
		}
	}
	while(!_point.empty()){
		unsigned _nbv = _nb.front();
		unsigned _pointv = _point.front();
		_nb.pop();
		_point.pop();
		if(vis[_pointv])
			continue;
		if(_pointv == des)
			return nb[_nbv].port;
		vis[_pointv] = true;
		for(std::set<unsigned>::iterator y = v_map[_pointv].begin(); y != v_map[_pointv].end(); ++y){
			_nb.push(_nbv);
			_point.push(*y);
		}
	}
	click_chatter("Error can't route to des %d.\n", des);
	return -1;
}
CLICK_ENDDECLS
EXPORT_ELEMENT(MyRouter)
