fs :: FileApp(SEND "/home/comnetsii/test/infile.txt")
fr :: FileApp(RECV "/home/comnetsii/test/outfile.txt")
fs[0] -> [0]fr
