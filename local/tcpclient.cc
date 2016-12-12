#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "tcpclient.hh"
#include "tcpheader.hh"
#include <cstdio>
#include <cstring>
#include <stdlib.h>
CLICK_DECLS
//#define click_chatter(x, ...)
struct pair_port{
	unsigned short src;
	unsigned short des;
};
TcpClient::TcpClient() : _timer1(this), _timer2(this)
{
	state = CLOSED;
	sendcnt = 0;
	buf = NULL;
	bufoffset = 0;
	bc = false;
	window = MSS;
	datasentup = 0;
}

TcpClient::~TcpClient()
{
	if(buf != NULL)
		delete []buf;
}
int TcpClient::initialize(ErrorHandler*){
	_timer1.initialize(this);
	_timer2.initialize(this);
}
int TcpClient::configure(Vector<String> &conf, ErrorHandler *errh)
{
	if(cp_va_kparse(conf, this, errh, 
		"INS", cpkM, cpUnsigned, &ins,
		"SRC_PORT", cpkM, cpUnsigned, &src_port,
		"DES_PORT", cpkM, cpUnsigned, &des_port,
		"BROADCAST",0, cpBool, &bc,
		cpEnd) < 0)
		return -1;
	if(bc){
		pair_port pp;
		pp.src = src_port;
		pp.des = des_port;
		WritablePacket * p = Packet::make((char*)&pp, 4);
		output(1).push(p);
	}
	return 0;
}

uint16_t TcpClient::gettcpchk(const unsigned char *ptr, int size)
{
	int cksum = 0;
	int index = 0;
	while(index < size - (int)sizeof(short))
	{
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

void TcpClient::settcpheader(tcpheader* header, unsigned s, unsigned a, uint8_t o, uint8_t f, unsigned w = 0)
{
	header -> SeqNum = s;
	header -> AckNum = a;
	header -> Offset = o;
	header -> Flag = f;
	header -> Checksum = 0;
	header -> Winsize = w;
	header -> src_port = src_port;
	header -> des_port = des_port;
	header -> Checksum = gettcpchk(cur -> data(), TCPHEADERSIZE);
}

void TcpClient::senddata()
{
	int len = filelen - bufoffset;
	int length = len < MSS ? len : MSS;
	length = length < window ? length : window;
	cur = Packet::make(buf + bufoffset, length);
	cur = cur -> push(TCPHEADERSIZE);
	tcpheader *tcp_header = (tcpheader *)(cur -> data());
	settcpheader(tcp_header, ins, 0, bufoffset, PSH); 
	click_chatter("Sent DATA!\n");
	bufoffset += length;
	sendcnt = 0;
	if(length == len)
	{
		delete []buf;
		buf = NULL;
	}
	 _timer1.schedule_after_sec(PACKET_TIMEOUT);
	output(1).push(cur->clone());
	/*if(len < MSS)
	{
		cur = Packet::make(buf + bufoffset, len);
		cur = cur -> push(TCPHEADERSIZE);
		tcpheader *tcp_header = (tcpheader *)(cur -> data());
		//tcp_header -> initialize();
		settcpheader(tcp_header, ins, 0, bufoffset, PSH); 
		click_chatter("Sent DATA!\n");
		bufoffset += len;
		delete []buf;
		buf = NULL;
		sendcnt = 0;
		 _timer1.schedule_after_sec(PACKET_TIMEOUT);
		output(1).push(cur->clone());
	}
	else
	{
		cur = Packet::make(buf + bufoffset, MSS);
		cur = cur -> push(TCPHEADERSIZE);
		tcpheader *tcp_header = (tcpheader *)(cur -> data());
		//tcp_header -> initialize();
		settcpheader(tcp_header, ins, 0, bufoffset, PSH); 
		click_chatter("Sent DATA!\n");
		bufoffset += MSS;
		sendcnt = 0;
		 _timer1.schedule_after_sec(PACKET_TIMEOUT);
		output(1).push(cur->clone());
	}*/
}
void TcpClient::push(int port, Packet *packet)
{
	if(port == 0) // raw data
	{
		int len = packet -> length();
		filelen = len;
		buf = new char[len];
		memcpy(buf, (const char*)packet -> data(), len);
		if(state == CLOSED) 
		{
			cur = Packet::make(TCPHEADERSIZE);
			tcpheader *tcp_header = (tcpheader *)(cur -> data());
			//tcp_header -> initialize();
			settcpheader(tcp_header, ins, 0, 0, SYN); // SYN
			click_chatter("Sent SYN!\n");
			state = SYN_SENT;
			sendcnt = 0;
			_timer1.schedule_after_sec(PACKET_TIMEOUT);
			output(1).push(cur->clone());
		}
	}
	else
	{
		click_chatter("Received Packet! state: %d, ins: %d\n", state, ins);
		tcpheader *rec_header = (tcpheader *)(packet -> data());
		click_chatter("seq: %d ack: %d offset: %d flag: %d checksum: %d get: %d\n", rec_header -> SeqNum, rec_header -> AckNum, rec_header -> Offset, rec_header -> Flag, rec_header -> Checksum, gettcpchk(packet -> data(), TCPHEADERSIZE));
		if(gettcpchk(packet -> data(), TCPHEADERSIZE) != rec_header -> Checksum)
		{
			packet -> kill();
			click_chatter("Wrong checksum!");
			return;
		}
		if(state == CLOSED)
		{
			if(rec_header -> Flag == SYN)
			{
				click_chatter("Received SYN!\n");
				cur = Packet::make(TCPHEADERSIZE);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				//cur_header -> initialize();
				settcpheader(cur_header, ins, rec_header -> SeqNum + 1, 0, SYN_ACK); // SYN_ACK
				click_chatter("Sent SYN_ACK!\n");
				state = SYN_RECEIVED;
				sendcnt = 0;
				_timer1.schedule_after_sec(PACKET_TIMEOUT);
				output(1).push(cur->clone());
			}
			else
			{
				click_chatter("Received wrong packet! State CLOSE.\n");
				packet -> kill();
			}
		}
		else if(state == SYN_SENT)
		{
			if(rec_header -> Flag == SYN_ACK && rec_header -> AckNum == ins + 1)
			{
				_timer1.unschedule();
				cur = Packet::make(TCPHEADERSIZE);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				//cur_header -> initialize();
				settcpheader(cur_header, ins + 1, rec_header -> SeqNum + 1, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				state = ESTABLISHED;
				bufoffset = 0;
				click_chatter("ESTABLISHED!\n");
				output(1).push(cur->clone());
				senddata();
			}
			else
			{
				click_chatter("Received wrong packet! State SYN_SENT.\n");
				packet -> kill();
			}
		}
		else if(state == SYN_RECEIVED)
		{
			if(rec_header -> Flag == ACK && rec_header -> AckNum == ins + 1)
			{
				_timer1.unschedule();
				state = ESTABLISHED;
				bufoffset = 0;
				click_chatter("ESTABLISHED!\n");
			}
			else
			{
				click_chatter("Received wrong packet! State SYN_RECEIVED.\n");
				packet -> kill();
			}
		}
		else if(state == ESTABLISHED) 
		{
			if(rec_header -> Flag == SYN_ACK && rec_header -> AckNum == ins + 1)
			{
				//_timer1.unschedule();
				cur = Packet::make(TCPHEADERSIZE);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				//cur_header -> initialize();
				settcpheader(cur_header, ins + 1, rec_header -> SeqNum + 1, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				state = ESTABLISHED;
				bufoffset = 0;
				click_chatter("ESTABLISHED!\n");
				output(1).push(cur->clone());
				senddata();
			}
            else if(rec_header -> Flag == PSH)
			{
				if(rec_header -> Offset == (bufoffset + datasentup) % 256)
				{
					/*int len = 0;
					if(buf != NULL)
					{
						len = strlen(buf);
						char *tmp = new char[len];
						strcpy(tmp, buf);
						buf = new char[len + packet -> length() - TCPHEADERSIZE];
						strcpy(buf, tmp);
						delete []tmp;
					}
					else
					{
						buf = new char[len + packet -> length() - TCPHEADERSIZE];
					}*/
					if(buf == NULL)
						buf = new char[RECV_BUFSIZE];
					memcpy(buf + bufoffset, (const char*)packet -> data() + TCPHEADERSIZE, packet->length() - TCPHEADERSIZE);
					bufoffset += packet -> length() - TCPHEADERSIZE;
					if(bufoffset == RECV_BUFSIZE)
					{
						cur = Packet::make(buf, bufoffset);
						datasentup += bufoffset;
						output(0).push(cur->clone());
						delete []buf;
						buf = new char[RECV_BUFSIZE];
						bufoffset = 0;
					}
					cur = Packet::make(TCPHEADERSIZE);
					tcpheader *cur_header = (tcpheader *)(cur -> data());
					settcpheader(cur_header, 0, 0, bufoffset + datasentup, ACK, RECV_BUFSIZE - bufoffset);
					click_chatter("Sent ACK!\n");
					output(1).push(cur->clone());
				}
				else{
					click_chatter("ESTABLISHED State. Resend ACK!\n");
					output(1).push(cur -> clone());
					
				}
			}
			else if(rec_header -> Flag == ACK && rec_header -> Offset == bufoffset % 256)
			{
				_timer1.unschedule();
				if(buf != NULL)
				{
					senddata();
					window = rec_header -> Winsize; 
				}
				else
				{
					cur = Packet::make(TCPHEADERSIZE);
					tcpheader *cur_header = (tcpheader *)(cur -> data());
					//cur_header -> initialize();
					settcpheader(cur_header, 0, 0, 0, FIN); // FIN
					click_chatter("Sent FIN! And Chang to FIN_WAIT1\n");
					state = FIN_WAIT1;
					sendcnt = 0;
					_timer1.schedule_after_sec(1);
					output(1).push(cur->clone());
				}
			}
			else if(rec_header -> Flag == FIN)
			{
				if(bufoffset != 0){
					cur = Packet::make(buf, bufoffset);
					output(0).push(cur->clone());
				}
				cur = Packet::make(TCPHEADERSIZE);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				//cur_header -> initialize();
				settcpheader(cur_header, 0, 0, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				state = CLOSE_WAIT;
				_timer2.schedule_after_sec(1);
				output(1).push(cur->clone());
			}
			else
			{
				click_chatter("Received wrong packet! State ESTABLISHED.\n");
				packet -> kill();
			}
		}
		else if(state == LAST_ACK)
		{
			if(rec_header -> Flag == FIN){
				_timer2.unschedule();
				cur = Packet::make(buf, bufoffset);
				output(0).push(cur->clone());
				cur = Packet::make(TCPHEADERSIZE);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				//cur_header -> initialize();
				settcpheader(cur_header, 0, 0, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				state = CLOSE_WAIT;
				_timer2.schedule_after_sec(1);
				output(1).push(cur->clone());				
			}
			else if(rec_header -> Flag == ACK)
			{
				_timer1.unschedule();
	            state = CLOSED;
	            sendcnt = 0;
				delete[] buf;
	            buf = NULL;
	            bufoffset = 0;
				window = MSS;
				datasentup = 0;
				//state = CLOSED;
				click_chatter("Closed! ins: %d\n", ins);
			}
			else
			{
				click_chatter("Received wrong packet! State LAST_ACK.\n");
				packet -> kill();
			}
		}
		else if(state == FIN_WAIT1)
		{
			if(rec_header -> Flag == ACK)
			{
				_timer1.unschedule();
				state = FIN_WAIT2;
			}
			else
			{
				click_chatter("Received wrong packet! State FIN_WAIT1.\n");
				packet -> kill();
			}
		}
		else if(state == FIN_WAIT2)
		{
			//tcpheader *rec_header = (tcpheader *)(packet -> data());
			if(rec_header -> Flag == FIN)
			{
				cur = Packet::make(TCPHEADERSIZE);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				//cur_header -> initialize();
				settcpheader(cur_header, 0, 0, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				state = TIMED_WAIT;
				_timer2.schedule_after_sec(1);
				output(1).push(cur->clone());
			}
			else
			{
				click_chatter("Received wrong packet! State FIN_WAIT2.\n");
				packet -> kill();
			}
		}
	}
}

void TcpClient::run_timer(Timer *timer)
{
	if(timer == &_timer1) 
	{
		click_chatter("timer fired at state: %d\n", state);
		if(sendcnt == MAXSENDCNT) 
		{
			cur -> kill();
	        state = CLOSED;
	        sendcnt = 0;
			delete [] buf;
	        buf = NULL;
	        bufoffset = 0;
			window = MSS;
			datasentup = 0;
			click_chatter("cannot send a packet! connection closed\n");
		}
		else
		{
			output(1).push(cur -> clone());
			++sendcnt;
			_timer1.reschedule_after_sec(2);
		}
	}
	else
	{
		if(state == CLOSE_WAIT)
		{
			cur = Packet::make(TCPHEADERSIZE);
			tcpheader *cur_header = (tcpheader *)(cur -> data());
			//cur_header -> initialize();
			settcpheader(cur_header, 0, 0, 0, FIN); // FIN
			click_chatter("Sent FIN!\n");
			state = LAST_ACK;
			sendcnt = 0;
			_timer1.schedule_after_sec(PACKET_TIMEOUT);
			output(1).push(cur -> clone());
			
			_timer2.schedule_after_sec(CONNECT_TIMEOUT);
		}
		else if(state == TIMED_WAIT)
		{	
			state = CLOSED;
			click_chatter("Closed! ins: %d\n", ins);
		}
		else if(state == LAST_ACK){
			state = CLOSED;
			click_chatter("Closed! ins: %d\n", ins);
		}
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(TcpClient)
