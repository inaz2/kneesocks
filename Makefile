CC = gcc
CFLAGS = -O2 -Wall -Werror
PREFIX = /usr/local

all: libkneesocks.so

clean:
	rm -f libkneesocks.so

install:
	install -m 755 kneesocks $(PREFIX)/bin/kneesocks
	install -m 644 libkneesocks.so $(PREFIX)/lib/libkneesocks.so
	echo $(PREFIX)/lib > /etc/ld.so.conf.d/libkneesocks.conf
	ldconfig

uninstall:
	rm -f $(PREFIX)/bin/kneesocks
	rm -f $(PREFIX)/lib/libkneesocks.so
	rm -f /etc/ld.so.conf.d/libkneesocks.conf
	ldconfig

libkneesocks.so: libkneesocks.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $< -ldl
