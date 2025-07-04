
// fb_read.c
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <stdint.h>

int main() {
    int fb_fd = open("/dev/fb0", O_RDONLY);
    if (fb_fd < 0) {
        perror("open");
        return 1;
    }

    // Get variable screen info (resolution, bits per pixel)
    struct fb_var_screeninfo vinfo;
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl");
        close(fb_fd);
        return 1;
    }

    // Calculate screen size in bytes
    size_t screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map framebuffer device to memory
    uint8_t *fbp = (uint8_t *)mmap(NULL, screensize, PROT_READ, MAP_SHARED, fb_fd, 0);
    if (fbp == MAP_FAILED) {
        perror("mmap");
        close(fb_fd);
        return 1;
    }

    // Print first 16 pixels (assuming 32bpp, 4 bytes per pixel)
    printf("First 16 pixels (in hex, 4 bytes each):\n");
    for (int i = 0; i < 16; i++) {
        uint32_t pixel = *((uint32_t *)(fbp + i * 4));
        printf("%08X ", pixel);
    }
    printf("\n");

    munmap(fbp, screensize);
    close(fb_fd);
    return 0;
}

