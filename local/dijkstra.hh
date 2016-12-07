#ifndef SELF_DIJKSTRA_HH
#define SELF_DIJKSTRA_HH
struct entry{
    unsigned src;
    unsigned des;
	entry(unsigned _src, unsigned _des):src(_src), des(_des){
		if(src > des){
			src ^= des;
			des ^= src;
			src ^= des;
		}
	}
	bool operator ==(const entry & b){
		return (src == b.src && des == b.des) || (des == b.src && src == b.des);
	}
	bool operator <(const entry & b){
		if(src != b.src)
			return src < b.src;
		else
			return des < b.des;
	}
};
struct ripheader{
    unsigned char command;
    unsigned char version;
    unsigned short magic;
    unsigned short size;
    unsigned short entry_num;
	unsigned src_ip;
};
#define HELLOMSG 1
#define ROUTEMSG 2
#define RIPMAGIC 0xfa1b
struct neighbor{
	unsigned ip;
	unsigned char stat;
	unsigned short port;
};
#define NEWSTAT 1
#define SUSPSTAT 2
#define FAILSTAT 3
#endif
