#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main()
{
    int ret;
    struct addrinfo *info;
    int s;
    char buf[8192] = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";

    ret = getaddrinfo("www.example.com", "http", NULL, &info);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo() failed\n");
        exit(1);
    }
    s = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    ret = connect(s, info->ai_addr, info->ai_addrlen);
    if (ret != 0) {
        fprintf(stderr, "connect() failed\n");
        exit(1);
    }
    send(s, buf, strlen(buf), 0);
    recv(s, buf, sizeof(buf), 0);
    puts(buf);

    return 0;
}
