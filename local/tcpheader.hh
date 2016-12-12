#ifndef MY_TCPHEADER_HH
#define MY_TCPHEADER_HH
#define TCPHEADERSIZE sizeof(tcpheader)

struct tcpheader
{
	unsigned short src_port;
	unsigned short des_port;
	unsigned SeqNum;
	unsigned AckNum;
	unsigned Winsize;	
	unsigned Offset: 8;
	unsigned Flag: 8;
	unsigned Checksum: 16;
};

#endif
