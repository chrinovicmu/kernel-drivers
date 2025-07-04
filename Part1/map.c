
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

#define MAP_SIZE 4096

int main() {
    int fd = open("/dev/my_driver0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned char *map = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("First 16 bytes of mmaped buffer:\n");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", map[i]);
    }
    printf("\n");

    munmap(map, MAP_SIZE);
    close(fd);
    return 0;
}
