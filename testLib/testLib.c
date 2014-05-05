#include <stdio.h>

int testLibFunction(int par1, int par2, int par3, int par4)
{
    printf("LibFunction called: %d %d %d %d\n", par1, par2, par3, par4);
    fflush(stdout);
    return par1 + par2 + par3 + par4;
}
