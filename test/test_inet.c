#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    int ret;
    struct sockaddr_in sin;
    int s;
    char buf[8192] = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";

    s = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    inet_pton(sin.sin_family, "93.184.216.119", &sin.sin_addr);
    ret = connect(s, (struct sockaddr *)&sin, sizeof(sin));
    if (ret != 0) {
        fprintf(stderr, "connect() failed\n");
        exit(1);
    }
    send(s, buf, strlen(buf), 0);
    recv(s, buf, sizeof(buf), 0);
    puts(buf);

    return 0;
}
