#ifndef MY_IPHEADER_H
#define MY_IPHEADER_H
#define IPHEADERSIZE sizeof(ipheader)
struct ipheader{
    unsigned Version:4;
    unsigned IHL:4;
    unsigned DSCP:6;
    unsigned ECN:2;
    unsigned TotalLength:16;
    unsigned Identification:16;
    unsigned Flags:3;
    unsigned FragmentOffset:13;
    unsigned TTL:8;
    unsigned Protocol:8;
    unsigned CheckSum:16;
    unsigned SrcIP;
    unsigned DesIP;
    ipheader(){}
    void initialize(uint8_t ptc = 6, uint32_t src = 0, uint32_t des = 0){
        Version = 4;
        IHL = IPHEADERSIZE;
        TTL = 255;
        Protocol = ptc;
        SrcIP = src;
        DesIP = des;
    }
};
#endif
