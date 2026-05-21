CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm
OBJDIR = ../obj
TARGET = ../bin/iq_analyzer
SRCS = iq_analyzer.c luminance.c color.c noise.c sharpness.c report.c
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

$(TARGET): $(OBJS)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.c iq_analyzer.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
