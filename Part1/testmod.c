#include <stdio.h>
#include <fnctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h> 

#define IOCTL_SET_NUM _IOW('a', 1, int)
#define IOCTL_GET_NUM _IOR('a', 2, int)

int main() {
    char buf[100];
    int fd = open("/dev/my_driver1", O_RDONLY);
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

    int num = 123; 
    if(ioctl(fd, IOCTL_SET_NUM, &num) < 0)
    {
        perror("ioctl SET_NUM"); 
        return 1; 
    }

    int recieve_num = 0; 
    if(ioctl(fd, IOCTL_GET_NUM, &recieve_num) < 0)
    {
        perror("ioctl GET_NUM"); 
        return 1; 
    }

    printf("Got from Kernel space the number %d\n", recieve_num);

    close(fd);
    return 0;
}

