#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char ld_preload[512] = "libkneesocks.so";
    char *buf;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(1);
    }

    curval = getenv("LD_PRELOAD");
    if (curval) {
        strncat(ld_preload, " ", sizeof(ld_preload)-1);
        strncat(ld_preload, curval, sizeof(ld_preload)-1);
    }
    setenv("LD_PRELOAD", ld_preload, 1);

    execvp(argv[1], argv+1);
    perror("kneesocks");
    exit(1);
}
