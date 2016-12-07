tcp1::TcpClient(INS 5);
tcp2::TcpClient(INS 20);
RatedSource(DATA "hello", RATE 1, LIMIT 10)
                        -> Print(RAW, -1)
						-> [0] tcp1 [1]
                        -> Print(OUT1, -1)
						-> [1] tcp2 [0]
                        -> Print(OUT2, -1)
						-> Discard
RatedSource(DATA "byebye", RATE 1, LIMIT 10)
                        -> Print(RAW2, -1)
						-> [0] tcp2 [1]
                        -> Print(2OUT1, -1)
						-> [1] tcp1 [0]
                        -> Print(2OUT2, -1)
						-> Discard
