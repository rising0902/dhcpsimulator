#
# Makefile
#
LDFLAGS := -lwinsock2
CFLAGS := -O2 -Wall

PROGS = dhcpsimulator

.PHONY: clean resource

all: ${PROGS}

clean:
	$(RM) $(PROGS) *.o *.a *~ *.exe

dhcpsimulator: src/dhcpsimulator.c
	$(CC) $(CFLAGS) -lwinsock2 -o $@ $(LDFLAGS)

