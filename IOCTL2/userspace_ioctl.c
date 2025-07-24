#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h> 
#include <unistd.h> 
#include <string.h>
#include "chardev.h"

int ioctl_set_msg(int file_desc, char *message)
{
    int rev_val = ioctl(file_desc, IOCTL_SET_MSG, message); 
    if(rev_val < 0)
    {
        printf("ioctl_set_msg failed: %d\n", rev_val);
    }
    return rev_val; 
}

int ioctl_get_msg(int file_desc)
{
    size_t buf_len = 0; 
    if(ioctl(file_desc, IOCTL_BUFFER_LEN, &buf_len) < 0)
    {
        perror("ioctl"); 
        close(file_desc); 
        return -1; 
    }
    char message[buf_len]; 
    int ret_val = ioctl(file_desc, IOCTL_GET_MSG, message); 
    if(ret_val < 0)
    {
        printf("ioctl_get_msg failed:  %d\n", ret_val); 
    }
    else
    {
        printf("get_mesg message: %s\n", message); 
    }
    return ret_val; 
}

int ioctl_get_nth_byte(int file_desc)
{
    int x, c; 
    printf("ioctl_get_nth_byte meesage:");
    x = 0; 
    do{
        c = ioctl(file_desc, IOCTL_GET_NTH_BYTE, x++); 
        if(c < 0)
        {
            printf("\nioctl_get_nth_byte failed at the %d byte\n", x); 
            return c; 
        }
        putchar(c); 
    } while (c != 0);
    printf("\n");
    return 0; 
}

int main(int argc, char *argv[])
{
    int file_desc;
    char *device_path = DEVICE_PATH; 
    char *test_message = "Hello from userspace!";
    
    if(argc > 1)
    {
        device_path = argv[1];
    }
    
    file_desc = open(device_path, O_RDWR);
    if(file_desc < 0)
    {
        printf("Can't open device file: %s\n", device_path);
        return EXIT_FAILURE;
    }
    
    printf("Device %s opened successfully\n", device_path);
    
    printf("\n=== Testing ioctl_set_msg ===\n");
    if(ioctl_set_msg(file_desc, test_message) == 0)
    {
        printf("Message sent successfully: %s\n", test_message);
    }
    
    printf("\n=== Testing ioctl_get_msg ===\n");
    ioctl_get_msg(file_desc);
    
    printf("\n=== Testing ioctl_get_nth_byte ===\n");
    ioctl_get_nth_byte(file_desc);
    
    close(file_desc);
    printf("\nDevice closed successfully\n");
    
    return EXIT_SUCCESS;
}
