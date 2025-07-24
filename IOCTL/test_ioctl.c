
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/mydevice"
#define MY_IOCTL_MAGIC 'x'
#define MY_IOCTL_CMD _IOW(MY_IOCTL_MAGIC, 1, int)

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    int value = 42;
    printf("Sending value: %d via ioctl\n", value);

    if (ioctl(fd, MY_IOCTL_CMD, &value) < 0) {
        perror("ioctl failed");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("ioctl call successful!\n");
    close(fd);
    return EXIT_SUCCESS;
}
