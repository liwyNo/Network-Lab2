require(library "/home/comnetsii/elements/routerport.click")
rp1 :: RouterPort(DEV veth1, IN_MAC 7a:e0:da:e5:63:fc, OUT_MAC 7e:c7:04:76:58:e9)
rp2 :: RouterPort(DEV veth2, IN_MAC 7e:c7:04:76:58:e9, OUT_MAC 7a:e0:da:e5:63:fc)
rp3 :: RouterPort(DEV veth3, IN_MAC 7a:14:ed:27:5f:18, OUT_MAC 2e:18:a3:64:92:8a)
rp4 :: RouterPort(DEV veth4, IN_MAC 2e:18:a3:64:92:8a, OUT_MAC 7a:14:ed:27:5f:18)
rp5 :: RouterPort(DEV veth5, IN_MAC ea:32:3d:58:1a:14, OUT_MAC 46:c8:a6:89:c2:16)
rp6 :: RouterPort(DEV veth6, IN_MAC 46:c8:a6:89:c2:16, OUT_MAC ea:32:3d:58:1a:14)
rp7 :: RouterPort(DEV veth7, IN_MAC 86:51:9f:5b:05:b7, OUT_MAC a6:58:99:a0:38:83)
rp8 :: RouterPort(DEV veth8, IN_MAC a6:58:99:a0:38:83, OUT_MAC 86:51:9f:5b:05:b7)
rp9 :: RouterPort(DEV veth9, IN_MAC 9a:a3:fc:c5:2d:57, OUT_MAC d2:51:60:ed:e9:c0)
rp10 :: RouterPort(DEV veth10, IN_MAC d2:51:60:ed:e9:c0, OUT_MAC 9a:a3:fc:c5:2d:57)
rp11 :: RouterPort(DEV veth11, IN_MAC b2:15:8a:3e:e3:f0, OUT_MAC f2:fd:c8:dc:5a:04)
rp12 :: RouterPort(DEV veth12, IN_MAC f2:fd:c8:dc:5a:04, OUT_MAC b2:15:8a:3e:e3:f0)

r1 :: MyRouter(IP 1)
r2 :: MyRouter(IP 2)
r3 :: MyRouter(IP 3)
r4 :: MyRouter(IP 4)
r5 :: MyRouter(IP 5)
r6 :: MyRouter(IP 6)
r7 :: MyRouter(IP 7)

tcp1 :: TcpClient(INS 10, SRC_PORT 22, DES_PORT 22, BROADCAST true)
tcp2 :: TcpClient(INS 20, SRC_PORT 22, DES_PORT 22, BROADCAST true)
tcp3 :: TcpClient(INS 30, SRC_PORT 23, DES_PORT 23, BROADCAST true)
tcp4 :: TcpClient(INS 40, SRC_PORT 23, DES_PORT 23, BROADCAST true)

tcpl1 :: TCPRouter()
tcpl2 :: TCPRouter()
tcpl3 :: TCPRouter()
rp1->[1]r1[1]->rp1
rp2->[1]r2[1]->rp2
rp3->[2]r2[2]->rp3
rp4->[1]r3[1]->rp4
rp5->[2]r3[2]->rp5
rp6->[1]r4[1]->rp6
rp7->[3]r2[3]->rp7
rp8->[1]r5[1]->rp8
rp9->[2]r5[2]->rp9
rp10->[1]r6[1]->rp10
rp11->[2]r6[2]->rp11
rp12->[1]r7[1]->rp12

file1 :: FileApp(SEND "/home/comnetsii/test/infile1.txt", RECV "/home/comnetsii/test/outfile1.txt")
file2 :: FileApp(SEND "/home/comnetsii/test/infile2.txt", RECV "/home/comnetsii/test/outfile2.txt")
InfiniteSource(ACTIVE false)->[0]r2[0]->Discard
InfiniteSource(ACTIVE false)->[0]r3[0]->Discard
InfiniteSource(ACTIVE false)->[0]r5[0]->Discard
InfiniteSource(ACTIVE false)->[0]r6[0]->Discard
InfiniteSource(ACTIVE false)->[0]tcp3[1]->SetIPHeader(SOURCE_IP 4, DEST_IP 1) -> [1]tcpl2[0] -> [0]r4[0] -> [0] tcpl2 [1]->[1]tcp3[0]->[0]file1
InfiniteSource(ACTIVE false)->[0]tcp2[1]->SetIPHeader(SOURCE_IP 7, DEST_IP 1) -> [1]tcpl3[0] -> [0]r7[0] -> [0] tcpl3 [1]->[1]tcp2[0]->[0]file2


file1[0] -> [0]tcp1[1] -> SetIPHeader(SOURCE_IP 1, DEST_IP 7) -> [1]tcpl1[0] -> [0]r1[0] -> [0]tcpl1[1] -> [1]tcp1[0] -> Print(LABEL "4TO1", MAXLENGTH -1, CONTENTS ASCII) -> Discard
file2[0] -> [0]tcp4[1] -> SetIPHeader(SOURCE_IP 1, DEST_IP 4) -> [2]tcpl1
	tcpl1[2] -> [1]tcp4[0] -> Print(LABEL "7TO1", MAXLENGTH -1, CONTENTS ASCII) -> Discard

