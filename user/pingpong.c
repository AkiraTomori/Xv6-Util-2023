#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc > 1) {
        fprintf(2, "Usage: %s <arguments>\n", argv[0]);
        exit(1);
    }
    int p2c[2];
    int c2p[2];
    char buf[1];

    pipe(p2c);
    pipe(c2p);

    int pid = fork();

    if (pid == 0)
    {
        close(p2c[1]);
        close(c2p[0]);

        read(p2c[0], buf, sizeof(buf));
        printf("%d: received ping\n", getpid());
        write(c2p[1], buf, sizeof(buf));

        close(p2c[0]);
        close(c2p[1]);
    }
    else if (pid > 0)
    {
        close(p2c[0]);
        close(c2p[1]);

        write(p2c[1], "ping", 1);
        read(c2p[0], buf, sizeof(buf));
        printf("%d: received pong\n", getpid());
        
        close(p2c[1]);
        close(c2p[0]);
    }
    else if (pid < 0)
    {
        fprintf(2, "Fork error.\n");
        exit(1);
    }
    exit(0);
}