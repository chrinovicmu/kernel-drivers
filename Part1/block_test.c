// blocking_read.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buf[100];
    int fd = open("/dev/my_driver0", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Blocking read started...\n");
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
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

