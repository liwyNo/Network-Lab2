// IP1[veth1] ---[veth2] IP3 [veth3]---[veth4] IP2
require(library "/home/comnetsii/elements/routerport.click")
rp1 :: RouterPort(DEV veth1, IN_MAC fe:9c:75:21:4b:6d, OUT_MAC be:66:36:6f:1b:4b)
rp2 :: RouterPort(DEV veth2, IN_MAC be:66:36:6f:1b:4b, OUT_MAC fe:9c:75:21:4b:6d)
rp3 :: RouterPort(DEV veth3, IN_MAC 02:7f:91:ee:8c:ab, OUT_MAC aa:69:c1:d0:1a:9a)
rp4 :: RouterPort(DEV veth4, IN_MAC aa:69:c1:d0:1a:9a, OUT_MAC 02:7f:91:ee:8c:ab)
r1 :: MyRouter(IP 1)
r2 :: MyRouter(IP 2)
r3 :: MyRouter(IP 3)
RatedSource(DATA "hello", RATE 1) -> SetIPHeader(SOURCE_IP 1, DEST_IP 2) -> [0] r1 [0] -> Print(R1_IN)->Discard
RatedSource(DATA "byebye", RATE 1) -> SetIPHeader(SOURCE_IP 2, DEST_IP 1) -> [0] r2 [0] -> Print(R2_IN)->Discard
InfiniteSource->[0]r3[0]->Discard
rp1[0] -> [1]r1[1] -> Print(Nei_1, -1) -> [0]rp1
rp2[0] -> [1]r3[1] -> Print(Nei_2, -1) -> [0]rp2
rp3[0] -> [2]r3[2] -> Print(Nei_3, -1) -> [0]rp3
rp4[0] -> [1]r2[1] -> Print(Nei_4, -1) -> [0]rp4



