CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

all: traceroute

icmp_tool.o: icmp_tool.h icmp_tool.c
traceroute.o: traceroute.c
traceroute: icmp_tool.o traceroute.o

clean:
	rm -rf *.o

distclean: clean
		rm -rf traceroute