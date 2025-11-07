#include <unistd.h>   
#include <string.h>    

void print_string(const char *s)
{
    size_t len = strlen(s);

    asm volatile (
        "mov $1, %%rax\n"     
        "mov $1, %%rdi\n"     
        "mov %0, %%rsi\n"     
        "mov %1, %%rdx\n"    
        "syscall\n"
        :
        : "r"(s), "r"(len)
        : "rax", "rdi", "rsi", "rdx"
    );
}

int main()
{
    print_string("Hello\n");
    return 0;
}
