require(library "/home/comnetsii/elements/myRouter.click")
tcp1::TcpClient(INS 5);
tcp2::TcpClient(INS 20);
RatedSource(DATA "hello", RATE 1, LIMIT 10)
						-> [0] tcp1 [1]
						-> [1] tcp2 [0]
						-> Discard