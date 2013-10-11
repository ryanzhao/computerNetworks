CPP = g++
LD = g++
TS = progArgs.o eventsLog.o misc.o interaction.o serverMain.o

traceServer: $(TS) syscallWrap.o 
	$(LD) -o traceServer $(TS) syscallWrap.o

syscallWrap.o: lib/syscallWrap.h lib/syscallWrap.cpp
	$(CPP) -c lib/syscallWrap.cpp

%.o: %.cpp %.h
	$(CPP) -c $<

clean:
	rm -f *.o traceServer

kleen: clean
	rm -f log.txt 
