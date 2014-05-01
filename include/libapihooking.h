#include <libdis.h>

void ApiHook_init();
void ApiHook_cleanup();

void ApiHook_asmToString(x86_insn_t* insn, char* line, size_t lineLength);
void ApiHook_printFunction(void* data);

int ApiHook_disasm(char* data, size_t offset, x86_insn_t* insn);
size_t ApiHook_findFirstNop(void* data);
int ApiHook_calcJump(void* from, void* to);
int ApiHook_copyInstructions(char* from, int minByteCount, char* dst);
void ApiHook_hookFunction(void* myFunction, void* otherFunction);
int ApiHook_unprotect(void* func);
