#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include <click/args.hh>
#include "fileapp.hh"
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
CLICK_DECLS

FileApp::FileApp()
{}

FileApp::~FileApp()
{}

int FileApp::configure(Vector<String> &conf, ErrorHandler *errh)
{
	if (Args(conf, this, errh)
    .read("SEND", sendfile)
	.read("RECV", recvfile)
	.complete() < 0)
	return -1;
	
	if(recvfile != "")
		if(ninputs() != 1){
			click_chatter("Error: 1 output unused.\n");
			return -1;
		}
	if(sendfile != ""){
		if(noutputs() != 1){
			click_chatter("Error: 1 input unused.\n");
			return -1;
		}
		send();
	}
	return 0;
}
void FileApp::send(){
	FILE* infile = fopen(sendfile.c_str(), "rb");
	if(!infile)
	{
		click_chatter("Can not open the file!\n");
		return;
	}
	//struct stat info;
	//stat((const char*)packet -> data(), &info);
	fseek(infile,0,SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0,SEEK_SET);
	char * buf = new char[len];
	fread(buf, sizeof(char), len, infile);
	fclose(infile);
	click_chatter("FileApp sendfile, filename<%s> length<%d>:\n%s\n", sendfile.c_str(), len, buf);
	WritablePacket *cur = Packet::make(0, buf, len, 0);
	delete[] buf;
	output(0).push(cur);
}
void FileApp::push(int port, Packet *packet)
{
	//click_chatter("%s\n", packet -> data());
	FILE* outfile = fopen(recvfile.c_str(), "wb");
	fwrite(packet->data(), sizeof(char), (int)packet -> length(), outfile);
	fclose(outfile);
	//output(1).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FileApp)
