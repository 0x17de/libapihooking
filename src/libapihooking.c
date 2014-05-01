
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <libdis.h>

int ApiHook_disasm(char* data, size_t offset, x86_insn_t* insn)
{
	int size = x86_disasm((unsigned char*)data, 0x100, 0, offset, insn);
	if (size)
	{
		return size;
	}
	return 0;
}

void ApiHook_asmToString(x86_insn_t* insn, char* line, size_t lineLength)
{
	x86_format_insn(insn, line, lineLength, intel_syntax);
}

size_t ApiHook_findFirstNop(void* data)
{
	size_t offset = 0;
	x86_insn_t insn;
	while(1)
	{
		int size = ApiHook_disasm((char*)data, offset, &insn);
		if (insn.type == insn_nop)
			return offset;
		offset += size;
	}
	return !0;
}

int ApiHook_calcJump(void* from, void* to)
{
	size_t ifrom = (size_t)from;
	size_t ito = (size_t)to;

	int offset = ito - ifrom - 5;

	return offset;
}

void ApiHook_printFunction(void* data)
{
  x86_insn_t insn;
  size_t offset = 0;
  char line[200];
  while(1)
  {
    int size = ApiHook_disasm((char*)data, offset, &insn);
    ApiHook_asmToString(&insn, line, 200);
    if (offset == 0)
    {
        if (insn.type == insn_jmp)
        {

        }
    }
    if (!size)
    {
      printf("Invalid\n");
      break;
    }
    ApiHook_asmToString(&insn, line, 200);
    printf("%08lX | %-30s= %-4d | ", (size_t)((char*)data+offset), line, size);
    for(int i = 0; i < size; ++i)
    {
      printf("%02X", (unsigned int)((char*)data)[offset+i] & 0xFF);
    }
    printf("\n");
    if (insn.type == insn_return)
      break;
    offset += size;
  }
}


int ApiHook_copyInstructions(char* from, int minByteCount, char* dst)
{
	x86_insn_t insn;
	int offset = 0;
	while(offset < minByteCount)
	{
		int size = ApiHook_disasm(from, offset, &insn);
		if (!size)
		{
			printf("Invalid");
			return !0;
		}
		memcpy(dst + offset, from + offset, size);
		printf("Copy from %lX to %lX, size %d\n", (size_t)(from + offset), (size_t)(dst + offset), size);
		fflush(stdout);
		offset += size;
	}
	return offset;
}

void ApiHook_hookFunction(void* myFunction, void* otherFunction)
{
	char* startOfFunction = ((char*)otherFunction + 4); // TODO: don't use magic numbers, look for mov ebp, esp
	char* endOfMyFunction = (char*)myFunction + ApiHook_findFirstNop(myFunction);

	// cleanup stack
	endOfMyFunction[0] = 0x5D; // pop ebp
	endOfMyFunction += 1; // move forward

	x86_insn_t insn;
	ApiHook_disasm(otherFunction, 0, &insn);

	char line[200];
	ApiHook_asmToString(&insn, line, 200);
	printf("Original %s\n", line);

	if (insn.type == insn_jmp)
	{
        // jump back to original code
        memcpy(endOfMyFunction, otherFunction, insn.size);
        int* startOfJumpToOrig = (int*)(endOfMyFunction + 2);
        *startOfJumpToOrig += endOfMyFunction - startOfFunction; // @TODO: verify

        // nop the old area
        memset(otherFunction, 0x90, insn.size);
        // jump from original to own function
        ((char*)otherFunction)[0] = 0xE9;
        int* startOfJump = (int*)((char*)otherFunction + 1);
        startOfJump[0] = ApiHook_calcJump((char*)otherFunction, myFunction);

        ApiHook_disasm(otherFunction, 0, &insn);
        ApiHook_asmToString(&insn, line, 200);
        printf("newOtherFunc: %s\n", line);
        fflush(stdout);

        ApiHook_disasm(endOfMyFunction, 0, &insn);
        ApiHook_asmToString(&insn, line, 200);

        printf("JumpBack: %s\n", line);
        fflush(stdout);
	}
	else
    {
        // backup original code in own function
        int sizeOfCopiedInstructions = ApiHook_copyInstructions(startOfFunction, 5, endOfMyFunction);
        printf("Size of copied instructions: %d\n", sizeOfCopiedInstructions);
        endOfMyFunction += sizeOfCopiedInstructions;

        // jump from original to own function
        startOfFunction[0] = 0xE9;
        int* startOfJump = (int*)(startOfFunction + 1);
        startOfJump[0] = ApiHook_calcJump(startOfFunction, myFunction);

        // jump back to original code
        endOfMyFunction[1] = 0xE9;
        int* startOfJumpToOrig = (int*)(endOfMyFunction + 2);
        startOfJumpToOrig[0] = ApiHook_calcJump(endOfMyFunction + 1, startOfFunction + 5);
    }
}

int ApiHook_unprotect(void* func)
{
	const size_t pageSize = sysconf(_SC_PAGESIZE);
	const size_t funcOffset = (size_t) func;
	const size_t pageFunc = funcOffset - funcOffset % pageSize;
	return mprotect((void*)pageFunc, pageSize, PROT_READ | PROT_WRITE | PROT_EXEC);
}

void ApiHook_init()
{
	x86_init(opt_none, 0, 0);
}

void ApiHook_cleanup()
{
    x86_cleanup();
}
