require(library "/home/comnetsii/elements/myRouter.click")
rp::Router(IP 1)
RatedSource(DATA "hello", RATE 1, LIMIT 10) 
                        -> SetIPHeader(SOURCE_IP 1, DEST_IP 2)
						-> Print(From_IP_1)
                        -> [0] rp [0]
                        -> Print(RouteDown)
                        -> Discard
RatedSource(DATA "world", RATE 1, LIMIT 10) 
                        -> SetIPHeader(SOURCE_IP 1, DEST_IP 2)
						-> Print(To_IP_2)
                        -> [1] rp
RatedSource(DATA "Thank", RATE 1, LIMIT 10) 
                        -> SetIPHeader(SOURCE_IP 2, DEST_IP 1)
						-> Print(To_IP_1)
                        -> [1] rp [1]
                        -> Print(RouteUP)
                        -> Discard

