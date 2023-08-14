#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    char buffer[2] ; // only need one character
    long len_tc = 1 ; // define long because long is 64 bits 
    long ret ; 
    printf("使用 'syscall' 呼叫system call\n");
    __asm__ volatile ( 
        "mov $0, %%rax\n"   //system call number write is 0
        "mov $0, %%rdi\n"   //stdin
        "mov %1, %%rsi\n"   // buffer address
        "mov %2, %%rdx\n"   // lenth 
        "syscall\n"
        "mov %%rax, %0" // syscall return number 
        :  "=m"(ret)
        : "g" (buffer), "g" (len_tc)
        : "rax", "rdi", "rsi", "rdx");
    printf("讀入的字元是 : %c\n",buffer[0]);
    printf("回傳值是：%ld\n", ret);
}
