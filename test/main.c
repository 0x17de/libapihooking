#include <signal.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

extern int testLibFunction(int par1, int par2, int par3, int par4);
#include "libapihooking.h"

static int running = 1;

static void sigint()
{
    running = 0;
}

void myHook(int par1, int par2, int par3, int par4)
{
    __asm__("nop");


    __asm__("push %rbp");
    __asm__("mov %rsp, %rbp");
    // __asm__("add %rsp, 0x20");

    /*int a = 3;
    printf("mycall: %d %d %d %d\n", par1, par2, par3, par4);
    a += 2;
    if (a == 2)
    {
        a = 3;
    }
    else
    {
        a = 4;
    }*/

    // __asm__("sub %rsp, 0x20");
    __asm__("mov %rbp, %rsp");
    __asm__("pop %rbp");

    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
}

int main()
{
    signal(SIGINT, sigint);

    testLibFunction(1, 2, 3, 4);

    ApiHook_init();
    ApiHook_unprotect(myHook);
    ApiHook_unprotect(testLibFunction);
    ApiHook_hookFunction(myHook, testLibFunction);
    ApiHook_printFunction(myHook);
    ApiHook_cleanup();

    for(int i = 0; i < 4; ++i)
    {
        testLibFunction(1, 2, 3, 4);
    }

    return 0;
}
