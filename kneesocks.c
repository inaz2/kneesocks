#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(1);
    }
    char ld_preload[256] = "libkneesocks.so:";
    char *buf = getenv("LD_PRELOAD");
    if (buf) {
        strncat(ld_preload, buf, sizeof(ld_preload));
    }
    setenv("LD_PRELOAD", ld_preload, 1);
    execvp(argv[1], argv+1);
}
