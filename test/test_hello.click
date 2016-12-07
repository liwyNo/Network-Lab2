require(library "/home/comnetsii/elements/routerport.click")
rp1 :: RouterPort(DEV veth1, IN_MAC 9e:a6:93:c5:5c:4a, OUT_MAC f2:ee:a3:cc:af:74)
rp2 :: RouterPort(DEV veth2, IN_MAC f2:ee:a3:cc:af:74, OUT_MAC 9e:a6:93:c5:5c:4a)
r1 :: MyRouter(IP 1)
r2 :: MyRouter(IP 2)
RatedSource(DATA "hello", RATE 1) -> SetIPHeader(SOURCE_IP 1, DEST_IP 2) -> [0] r1 [0] -> Print(R1_IN)->Discard
RatedSource(DATA "byebye", RATE 1) -> SetIPHeader(SOURCE_IP 2, DEST_IP 1) -> [0] r2 [0] -> Print(R2_IN)->Discard
rp1[0] -> [1]r1[1] -> Print(Nei_1) -> [0]rp1
rp2[0] -> [1]r2[1] -> Print(Nei_2) -> [0]rp2


