#include "stdio.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "string.h"
#define KEY 0x1234
#define SIZE 100

int main(void)
{
        char s1[15]     = "not";
        char old_s1[15] = "not";
        int key = shmget(KEY,SIZE,IPC_CREAT | 0666);

        if (key == -1)
        {
            perror("failed");
            
        }
        printf("success\n");
        char *ptr = (char*)shmat(key,NULL,0);
        if (ptr == (char*)-1)
        {
             perror("failed");       
        }
        while (1)
        {
                 while(strcmp(old_s1,s1) == 0)
                {  
                    strcpy(s1,ptr);
                }
                printf("i had recived %s\n", s1);
                if (strcmp(s1,"exit") == 0)
                 {
                    break;
                 }
                strcpy(old_s1,s1);
        }



}


