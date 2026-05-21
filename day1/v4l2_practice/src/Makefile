CC = gcc
CFLAGS = -Wall -O2
BINDIR = ../bin
TARGETS = $(BINDIR)/capture_frame $(BINDIR)/extract_y

all: $(TARGETS)

$(BINDIR)/capture_frame: capture_frame.c
	$(CC) $(CFLAGS) -o $@ capture_frame.c

$(BINDIR)/extract_y: extract_y.c
	$(CC) $(CFLAGS) -o $@ extract_y.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
