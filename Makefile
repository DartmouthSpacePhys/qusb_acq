CC = gcc
CFLAGS = -std=gnu99 -pipe -O2 -Wall -g
LDFLAGS = -pipe -Wall -lquickusb -lm -pthread -g

SOURCES = qusb_acq.c qusb_helpers.c
OBJECTS = $(SOURCES:.c=.o)
EXEC = qusb_acq

all: $(SOURCES) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS)

porttest: porttest.c
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) porttest.c qupp.c

.c.o: defaults.h qusb_*.h
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXEC)

