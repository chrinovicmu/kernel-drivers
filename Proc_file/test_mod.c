#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 

int main() {
    int fp;
    char buffer[128];
    char write_buffer[] = "WRITING THIS FROM USERLAND\n";

    fp = open("/proc/helloworld", O_RDWR);
   // if (fp == NULL) {
     //   perror("Failed to open /proc/helloworld");
       // return EXIT_FAILURE;
   // }

    printf("Reading from /proc/helloworld:\n");

    //while (fgets(buffer, sizeof(buffer), fp) != NULL) {
      //  printf("%s", buffer);
  //  }

    char buf[50]; 

    ssize_t read_buffer = read(fp, buf, sizeof(buf));
    if(read_buffer < 0)
    {
        perror("read");
        close(fp); 
        return -1; 
    }else if (read_buffer == 0)
    {
        printf("no data avaible\n");
    }
     else {
        buf[read_buffer] = '\0';
        printf("Read %zd bytes: %s\n", read_buffer, buf);
    }

    ssize_t bytes_written = write(fp, write_buffer, sizeof(write_buffer)); 
    if(bytes_written < 0)
    {
        perror("write"); 
        close(fp);
        return -1; 
    }

    printf("wrote %d bytes to kernel space\n", bytes_written);
    close(fp);
    return EXIT_SUCCESS;
}
