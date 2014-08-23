CC = gcc
OBJS = kneesocks libkneesocks.so

all: $(OBJS)

clean:
	rm -f $(OBJS)

install:
	chmod 755 kneesocks
	chmod 644 libkneesocks.so
	cp kneesocks /usr/bin/kneesocks
	cp libkneesocks.so /usr/lib/libkneesocks.so

uninstall:
	rm -f /usr/bin/kneesocks
	rm -f /usr/lib/libkneesocks.so

kneesocks: kneesocks.c
	$(CC) -o kneesocks kneesocks.c

libkneesocks.so: libkneesocks.c
	$(CC) -shared -fPIC -o libkneesocks.so libkneesocks.c -ldl
