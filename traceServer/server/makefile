CPP = g++
LD = g++

traceServer: progArgs.o eventsLog.o serverMain.o
	$(LD) -o traceServer progArgs.o eventsLog.o serverMain.o

progArgs.o: progArgs.h progArgs.cpp
	$(CPP) -c progArgs.cpp

eventsLog.o: eventsLog.h eventsLog.cpp
	$(CPP) -c eventsLog.cpp

serverMain.o: serverMain.cpp
	$(CPP) -c serverMain.cpp

clean:
	rm -f *.o traceServer

kleen:
	rm -f *.o traceServer log.txt
