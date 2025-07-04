// nonblocking_read.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    char buf[100];
    int fd = open("/dev/my_driver0", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Non-blocking read started...\n");
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            printf("No data available (non-blocking)\n");
        else
            perror("read");
    } else if (n == 0) {
        printf("EOF (no data available)\n");
    } else {
        buf[n] = '\0';
        printf("Read %zd bytes: %s\n", n, buf);
    }

    close(fd);
    return 0;
}
