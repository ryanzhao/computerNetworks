CC=gcc
LD=gcc
OPTS='-g'

all: utils.o keeplog.o dtraceroute.o 
	$(LD) $(OPTS) -o dtraceroute utils.o keeplog.o dtraceroute.o

utils.o: utils.h utils.c
	$(CC) $(OPTS) -c utils.c

keeplog.o: keeplog.h keeplog.c
	$(CC) $(OPTS) -c keeplog.c

dtraceroute.o:
	$(CC) $(OPTS) -c dtraceroute.c

clean:
	rm -f *.o dtraceroute

kleen:
	rm -f *.o dtraceroute log .[a-zA-Z0-9]*
