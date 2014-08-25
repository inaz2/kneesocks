#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    int ret;
    struct hostent *host;
    struct sockaddr_in sin;
    int s;
    char buf[8192] = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";

    host = gethostbyname("www.example.com");
    if (host == NULL) {
        fprintf(stderr, "gethostbyname() failed\n");
        exit(1);
    }
    s = socket(host->h_addrtype, SOCK_STREAM, 0);
    sin.sin_family = host->h_addrtype;
    sin.sin_port = htons(80);
    sin.sin_addr = *(struct in_addr *)host->h_addr_list[0];
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
