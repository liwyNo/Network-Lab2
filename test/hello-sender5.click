require(library "/home/comnetsii/elements/routerport.click")
rp1 :: RouterPort(DEV veth1, IN_MAC 00:00:00:00:00:01, OUT_MAC 00:00:00:00:00:02)
rp2 :: RouterPort(DEV veth2, IN_MAC 00:00:00:00:00:02, OUT_MAC 00:00:00:00:00:01)

r1 :: MyRouter(IP 1)
r2 :: MyRouter(IP 2)

rp1 -> [1]r1[1] ->rp1
rp2 -> [1]r2[1] ->rp2

tcp1::TcpClient(INS 5);
tcp2::TcpClient(INS 20);
file::FileApp();
RatedSource(DATA "/home/comnetsii/test/infile.txt", RATE 1, LIMIT 1)
						-> [0] file [0] 
						-> [0] tcp1 [1]
                        -> Print(From1, -1)
						-> SetIPHeader(SOURCE_IP 1, DEST_IP 2)
                        -> [0] r1 [0]
                        -> Print(TO1)
                        -> [1] tcp1 [0]
                        -> Discard

tcp2[1] -> Print(From2) -> SetIPHeader(SOURCE_IP 2, DEST_IP 1) -> [0]r2[0] -> Print(TO2) -> [1] tcp2 [0] -> [1] file [1] -> Discard

 
FromDevice(veth3) ->  [0] tcp2
