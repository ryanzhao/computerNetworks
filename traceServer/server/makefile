CPP = g++
LD = g++

traceServer: progArgs.o eventsLog.o lib/syscallWrap.o serverMain.o
	$(LD) -o traceServer progArgs.o eventsLog.o lib/syscallWrap.o serverMain.o

progArgs.o: progArgs.h progArgs.cpp
	$(CPP) -c progArgs.cpp

eventsLog.o: eventsLog.h eventsLog.cpp
	$(CPP) -c eventsLog.cpp

syscallWrap.o: lib/syscallWrap.h lib/syscallWrap.cpp
	$(CPP) -c lib/syscallWrap.cpp

serverMain.o: serverMain.cpp
	$(CPP) -c serverMain.cpp

clean:
	rm -f lib/*.o *.o traceServer

kleen: clean
	rm -f log.txt 
