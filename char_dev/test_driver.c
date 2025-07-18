
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h> 

#define IOCTL_SET_NUM _IOW('a', 1, int)
#define IOCTL_GET_NUM _IOR('a', 2, int)

int main() {
    char buf[100];
    int fd = open("/dev/my_driver", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    ssize_t bytes = read(fd, buf, sizeof(buf) - 1);
    if (bytes < 0) {
        perror("read");
        return 1;
    }

    buf[bytes] = '\0';
    printf("Read from device: %s\n", buf);

    close(fd);
    return 0;
}


