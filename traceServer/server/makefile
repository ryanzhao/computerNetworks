CPP = g++
LD = g++

traceServer: progArgs.o eventsLog.o lib/sockWrap.o serverMain.o
	$(LD) -o traceServer progArgs.o eventsLog.o lib/sockWrap.o serverMain.o

progArgs.o: progArgs.h progArgs.cpp
	$(CPP) -c progArgs.cpp

eventsLog.o: eventsLog.h eventsLog.cpp
	$(CPP) -c eventsLog.cpp

sockWrap.o: lib/sockWrap.h lib/sockWrap.cpp
	$(CPP) -c lib/sockWrap.cpp

serverMain.o: serverMain.cpp
	$(CPP) -c serverMain.cpp

clean:
	rm -f lib/*.o *.o traceServer

kleen: clean
	rm -f log.txt 
