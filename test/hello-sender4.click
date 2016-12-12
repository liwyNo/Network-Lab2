tcp1::TcpClient(INS 5);
tcp2::TcpClient(INS 20);
file::FileApp(SEND "/home/comnetsii/test/infile.txt", RECV "/home/comnetsii/test/outfile.txt");
file [0] -> [0] tcp1 [1]-> [1] tcp2 [0]-> [0] file

tcp2 [1] -> [1] tcp1 
tcp1 [0] -> Discard
FromDevice(veth1) ->  [0] tcp2
