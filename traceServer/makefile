CPP = g++
LD = g++

traceServer: progArgs.o serverMain.o
	$(LD) -o traceServer progArgs.o serverMain.o

progArgs.o: progArgs.h progArgs.cpp
	$(CPP) -c progArgs.cpp

serverMain.o: serverMain.cpp
	$(CPP) -c serverMain.cpp

clean:
	rm -f *.o traceServer
