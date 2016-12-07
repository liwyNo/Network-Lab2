#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "tcpclient.hh"
#include "tcpheader.hh"
#include <cstdio>
#include <cstring>
CLICK_DECLS

TcpClient::TcpClient() : _timer1(this), _timer2(this)
{
	state = CLOSED;
	sendcnt = 0;
	buf = NULL;
	bufoffset = 0;
}

TcpClient::~TcpClient()
{
	if(buf != NULL)
		delete []buf;
}

int TcpClient::configure(Vector<String> &conf, ErrorHandler *errh)
{
	if(cp_va_kparse(conf, this, errh, "INS", cpkM, cpUnsigned, &ins) < 0)
		return -1;
	_timer1.initialize(this);
	_timer2.initialize(this);
	return 0;
}

uint16_t TcpClient::gettcpchk(unsigned char *ptr, int size)
{
	int cksum = 0;
	int index = 0;
	while(index < size)
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

void TcpClient::settcpheader(tcpheader* header, unsigned s, unsigned a, uint8_t o, uint8_t f)
{
	header -> SeqNum = s;
	header -> AckNum = a;
	header -> Offset = o;
	header -> Flag = f;
	header -> Checksum = gettcpchk(cur -> data(), TCPHEADERSIZE);
}

void TcpClient::senddata()
{
	int len = strlen(buf) - bufoffset;
	if(len < MSS)
	{
		cur = Packet::make(0, buf, len, 0);
		cur = cur -> push(TCPHEADERSIZE);
		tcpheader *tcp_header = (tcpheader *)(cur -> data());
		tcp_header -> initialize();
		settcpheader(tcp_header, ins, 0, 0, PSH); 
		click_chatter("Sent DATA!\n");
		output(1).push(cur);
		bufoffset += len;
		delete []buf;
		buf = NULL;
	}
	else
	{
		cur = Packet::make(0, buf + bufoffset, MSS, 0);
		cur = cur -> push(TCPHEADERSIZE);
		tcpheader *tcp_header = (tcpheader *)(cur -> data());
		tcp_header -> initialize();
		settcpheader(tcp_header, ins, 0, bufoffset, PSH); 
		click_chatter("Sent DATA!\n");
		output(1).push(cur);
		bufoffset += MSS;
	}
	_timer1.schedule_after_sec(2);
}
void TcpClient::push(int port, Packet *packet)
{
	if(port == 0) // raw data
	{
		int len = packet -> length();
		buf = new char[len];
		strcpy(buf, packet -> data());
		if(state == CLOSED) 
		{
			cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
			tcpheader *tcp_header = (tcpheader *)(cur -> data());
			tcp_header -> initialize();
			settcpheader(tcp_header, ins, 0, 0, SYN); // SYN
			click_chatter("Sent SYN!\n");
			output(1).push(cur);
			state = SYN_SENT;
			_timer1.schedule_after_sec(2);
		}
	}
	else
	{
		tcpheader *rec_header = (tcpheader *)(packet -> data());
		if(gettcpchk(packet -> data(), TCPHEADERSIZE) != 0)
		{
			packet -> kill();
			return;
		}
		if(state == CLOSED)
		{
			if(rec_header -> Flag == SYN)
			{
				cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				cur_header -> initialize();
				settcpheader(cur_header, ins, rec_header -> SeqNum + 1, 0, SYN_ACK); // SYN_ACK
				click_chatter("Sent SYN_ACK!\n");
				output(1).push(cur);
				state = SYN_RCVD;
				_timer1.schedule_after_sec(2);
			}
			else
			{
				click_chatter("Received wrong packet!\n");
				packet -> kill();
			}
		}
		else if(state == SYN_SENT)
		{
			if(rec_header -> Flag == SYN_ACK && rec_header -> AckNum == ins + 1)
			{
				_timer1.unschedule();
				cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				cur_header -> initialize();
				settcpheader(cur_header, ins + 1, rec_header -> SeqNum + 1, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				output(1).push(cur);
				state = ESTABLISHED;
				bufoffset = 0;
				senddata(bufoffset);
				click_chatter("ESTABLISHED!\n");
				_timer1.schedule_after_sec(2);
			}
			else
			{
				click_chatter("Received wrong packet!\n");
				packet -> kill();
			}
		}
		else if(state == SYN_RCVD)
		{
			if(rec_header -> Flag == ACK && rec_header -> AckNum == ins + 1)
			{
				_timer1.unschedule();
				state = ESTABLISHED;
				click_chatter("ESTABLISHED!\n");
			}
			else
			{
				click_chatter("Received wrong packet!\n");
				packet -> kill();
			}
		}
		else if(state == ESTABLISHED) 
		{
			if(rec_header -> Flag == PSH && rec_header -> Offset == bufoffset)
			{
				int len = 0;
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
				}
				strcpy(buf + len, packet -> data() + TCPHEADERSIZE);
			}
			else if(rec_header -> Flag == ACK && rec_header -> Offset == bufoffset)
			{
				if(buf != NULL)
					senddata();
				else
				{
					cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
					tcpheader *cur_header = (tcpheader *)(cur -> data());
					cur_header -> initialize();
					settcpheader(cur_header, 0, 0, 0, FIN); // FIN
					click_chatter("Sent FIN!\n");
					output(1).push(cur);
					state = FIN_WAIT1;
					_timer2.schedule_after_sec(1);
				}
			}
			else if(rec_header -> Flag == FIN)
			{
				cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				cur_header -> initialize();
				settcpheader(cur_header, 0, 0, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				output(1).push(cur);
				state = CLOSE_WAIT;
				_timer2.schedule_after_sec(1);
			}
			else
			{
				click_chatter("Received wrong packet!\n");
				packet -> kill();
			}
		}
		else if(state == LAST_ACK)
		{
			if(rec_header -> Flag == ACK)
			{
				_timer1.unschedule();
				state = CLOSED;
			}
			else
			{
				click_chatter("Received wrong packet!\n");
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
				click_chatter("Received wrong packet!\n");
				packet -> kill();
			}
		}
		else if(state == FIN_WAIT2)
		{
			//tcpheader *rec_header = (tcpheader *)(packet -> data());
			if(rec_header -> Flag == FIN)
			{
				cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
				tcpheader *cur_header = (tcpheader *)(cur -> data());
				cur_header -> initialize();
				settcpheader(cur_header, 0, 0, 0, ACK); // ACK
				click_chatter("Sent ACK!\n");
				output(1).push(cur);
				state = TIMED_WAIT;
				_timer2.schedule_after_sec(4);
			}
			else
			{
				click_chatter("Received wrong packet!\n");
				packet -> kill();
			}
		}
	}
}

void TcpClient::run_timer(Timer *timer)
{
	if(timer == &_timer1) 
	{
		click_chatter("timer fired at state: %d\n", &state);
		if(sendcnt == MAXSENDCNT) 
		{
			cur -> kill();
			click_chatter("cannot send a packet! connection closed\n");
		}
		else
		{
			output(1).push(cur);
			++sendcnt;
			_timer1.reschedule_after_sec(2);
		}
	}
	else
	{
		if(state == CLOSE_WAIT)
		{
			cur = Packet::make(0, 0, TCPHEADERSIZE, 0);
			tcpheader *cur_header = (tcpheader *)(cur -> data());
			cur_header -> initialize();
			settcpheader(cur_header, 0, 0, 0, FIN); // FIN
			click_chatter("Sent FIN!\n");
			output(1).push(cur);
			state = LAST_ACK;
			_timer1.schedule_after_sec(2);
		}
		else if(state == TIMED_WAIT)
			state = CLOSED;
	}
}