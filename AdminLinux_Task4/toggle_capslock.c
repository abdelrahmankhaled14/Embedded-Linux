#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char *argv[]) 
{ 
	int fd = open("/sys/class/leds/asus::kbd_backlight/brightness", O_WRONLY);
       	if (strcmp(argv[1],"0") == 0)
             { 
		write(fd, "1", 1);
	     }
       	else if (strcmp(argv[1],"1") == 0)
	     {
		 write(fd, "0", 1); 
	     }
       	else
	     {
		     printf("Invalid argument. Use 0 or 1.\n");
	     }
       
	close(fd);
}
