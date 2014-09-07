/* for dlsym(3) */
#define _GNU_SOURCE
#include <dlfcn.h>

/* for getaddrinfo(3), gethostbyname(3), connect(2) */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* for fcntl(2) */
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LOG_DEBUG(...) if (env_debug) fprintf(stderr, "[kneesocks] " __VA_ARGS__)

extern int h_errno;
static int (*orig_getaddrinfo)(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
static struct hostent *(*orig_gethostbyname)(const char *name);
static int (*orig_connect)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

static char *env_debug;
static struct addrinfo *proxy_info;

void __attribute__((constructor))
libkneesocks_init()
{
    char *env_socks_proxy;
    char *proxy_node;
    char *proxy_service;
    char buf[512];

    orig_getaddrinfo = dlsym(RTLD_NEXT, "getaddrinfo");
    orig_gethostbyname = dlsym(RTLD_NEXT, "gethostbyname");
    orig_connect = dlsym(RTLD_NEXT, "connect");

    env_debug = getenv("DEBUG");
    env_socks_proxy = getenv("socks_proxy");
    if (env_socks_proxy) {
        strncpy(buf, env_socks_proxy, sizeof(buf));
        proxy_node = strtok(buf, ":");
        proxy_service = strtok(NULL, ":");
    } else {
        proxy_node = "localhost";
        proxy_service = "1080";
    }
    orig_getaddrinfo(proxy_node, proxy_service, NULL, &proxy_info);
}

static __thread char saved_node[256];

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    LOG_DEBUG("getaddrinfo: node=%s, service=%s\n", node, service);
    strncpy(saved_node, node, sizeof(saved_node));
    return (*orig_getaddrinfo)("0.0.0.1", service, hints, res);
}

struct hostent *gethostbyname(const char *name)
{
    LOG_DEBUG("gethostbyname: name=%s\n", name);
    strncpy(saved_node, name, sizeof(saved_node));
    return (*orig_gethostbyname)("0.0.0.1");
}

int connect_proxy(int sockfd, const struct sockaddr_in *addr, socklen_t addrlen)
{
    int fd_flags;
    int ret;
    struct sockaddr_in *proxy_addr;
    char buf[512];
    int nodelen;

    fd_flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, fd_flags & ~O_NONBLOCK);

    ret = (*orig_connect)(sockfd, proxy_info->ai_addr, sizeof(struct sockaddr));
    if (ret != 0) {
        proxy_addr = (struct sockaddr_in *)(proxy_info->ai_addr);
        inet_ntop(proxy_addr->sin_family, &proxy_addr->sin_addr, buf, sizeof(buf));
        LOG_DEBUG("connect_proxy: failed to connect proxy %s:%d\n", buf, ntohs(proxy_addr->sin_port));
        return ret;
    }

    send(sockfd, "\x05\x01\x00", 3, 0);
    recv(sockfd, buf, 2, 0);
    assert(memcmp(buf, "\x05\x00", 2) == 0);

    if (memcmp(&addr->sin_addr, "\x00\x00\x00\x01", 4) == 0) {
        LOG_DEBUG("connect_proxy: saved_node=%s\n", saved_node);
        memcpy(buf, "\x05\x01\x00\x03", 4);
        nodelen = strlen(saved_node);
        buf[4] = (unsigned char)nodelen;
        memcpy(buf+5, saved_node, nodelen);
        memcpy(buf+5+nodelen, &addr->sin_port, 2);
        send(sockfd, buf, 7+nodelen, 0);
    } else if (addr->sin_family == AF_INET) {
        memcpy(buf, "\x05\x01\x00\x01", 4);
        memcpy(buf+4, &addr->sin_addr, 4);
        memcpy(buf+8, &addr->sin_port, 2);
        send(sockfd, buf, 10, 0);
    } else if (addr->sin_family == AF_INET6) {
        memcpy(buf, "\x05\x01\x00\x04", 4);
        memcpy(buf+4, &addr->sin_addr, 16);
        memcpy(buf+20, &addr->sin_port, 2);
        send(sockfd, buf, 22, 0);
    } else {
        LOG_DEBUG("connect_proxy: addr->sin_family must be AF_INET or AF_INET6");
        exit(1);
    }

    recv(sockfd, buf, 10, 0);
    assert(memcmp(buf, "\x05\x00", 2) == 0);

    fcntl(sockfd, F_SETFL, fd_flags);

    return ret;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int so_type;
    int optlen;
    struct sockaddr_in *addr_in;
    char str_address[64];

    if (addr->sa_family == AF_INET || addr->sa_family == AF_INET6) {
        optlen = sizeof(so_type);
        getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &so_type, &optlen);

        addr_in = (struct sockaddr_in *)addr;
        inet_ntop(addr_in->sin_family, &addr_in->sin_addr, str_address, sizeof(str_address));
        if (so_type & SOCK_STREAM) {
            LOG_DEBUG("connect: type=stream, family=%d, address=%s, port=%d\n", addr_in->sin_family, str_address, ntohs(addr_in->sin_port));
            return connect_proxy(sockfd, addr_in, addrlen);
        } else if (so_type & SOCK_DGRAM) {
            LOG_DEBUG("connect: type=dgram, family=%d, address=%s, port=%d\n", addr_in->sin_family, str_address, ntohs(addr_in->sin_port));
        }
    }

    return (*orig_connect)(sockfd, addr, addrlen);
}
