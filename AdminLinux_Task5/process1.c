#include "stdio.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "string.h"
#define KEY 0x1234
#define SIZE 100

int main(void)
{
        char s1[15] = "wewew!";
        int key = shmget(KEY,SIZE,IPC_CREAT | 0666);
	
        if (key == -1)
        {
            perror("failed");   
        }

        char *ptr = (char*)shmat(key,NULL,0);
        if (ptr == (char*)-1)
        {
             perror("failed");       
        }
        while(1)
        {
            fflush(stdout);
            printf("Send something \n");
            fgets(s1,15,stdin);
            s1[strcspn(s1, "\n")] = 0;
            strcpy(ptr,s1);
            printf("sender had said %s\n", s1);
            if (strcmp(s1,"exit") == 0)
            {
                break;
            }
        }

}


