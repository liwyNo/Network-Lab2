tcp1::TcpClient(INS 5);
tcp2::TcpClient(INS 20);
RatedSource(DATA "/home/comnetsii/test/infile.txt", RATE 1, LIMIT 1)
						-> [0] tcp1 [1]
						-> [1] tcp2 [0]
						-> Print(content, -1)
						-> Discard

tcp2 [1] -> [1] tcp1 
tcp1 [0] -> Discard
FromDevice(veth1) ->  [0] tcp2
