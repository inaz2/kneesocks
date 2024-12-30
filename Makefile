CC = gcc
CFLAGS = -O2 -Wall -Werror
PREFIX = /usr/local

all: libkneesocks.so

clean:
	rm -f libkneesocks.so

install:
	install -D -m 755 kneesocks $(PREFIX)/bin/kneesocks
	install -D -m 644 libkneesocks.so $(PREFIX)/lib/kneesocks/libkneesocks.so
	echo $(PREFIX)/lib/kneesocks > /etc/ld.so.conf.d/kneesocks.conf
	ldconfig

uninstall:
	rm -f $(PREFIX)/bin/kneesocks
	rm -rf $(PREFIX)/lib/kneesocks
	rm -f /etc/ld.so.conf.d/kneesocks.conf
	ldconfig

libkneesocks.so: libkneesocks.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $< -ldl
