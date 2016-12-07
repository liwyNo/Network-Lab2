#ifndef MY_TCPHEADER_HH
#define MY_TCPHEADER_HH
#define TCPHEADERSIZE sizeof(tcpheader)

struct tcpheader
{
	unsigned SeqNum;
	unsigned AckNum;
	unsigned Offset: 8;
	unsigned Flag: 8;
	unsigned Checksum: 16;
	void initialize()
	{
		SeqNum = AckNum = 0;
		Offset = 0;
		Flag = 0;
		Checksum = 0;
	}
};

#endif