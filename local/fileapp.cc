#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "fileapp.hh"
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
CLICK_DECLS

FileApp::FileApp()
{
	buf = NULL;
}

FileApp::~FileApp()
{
	if(buf != NULL)
		delete []buf;
}

int FileApp::configure(Vector<String> &conf, ErrorHandler *errh)
{
	return 0;
}

void FileApp::push(int port, Packet *packet)
{
	if(port == 0)
	{
		FILE* infile = fopen((const char*)packet -> data(), "rb");
		if(!infile)
		{
			click_chatter("Can not open the file!\n");
			return;
		}
		struct stat info;
		stat((const char*)packet -> data(), &info);
		int len = info.st_size;
		buf = new char[len];
		fread(buf, sizeof(char), len, infile);
		fclose(infile);
		click_chatter("%s\n", buf);
		WritablePacket *cur = Packet::make(0, buf, len, 0);
		output(0).push(cur);
	}
	else
	{
		//click_chatter("%s\n", packet -> data());
		FILE* outfile = fopen("/home/comnetsii/test/outfile.txt", "wb");
		fwrite(buf, sizeof(char), (int)packet -> length(), outfile);
		fclose(outfile);
		output(1).push(packet);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FileApp)
