#include "eth32.h"

//2.60b use optimizations that pass the args in registers
// FS_Startup2 takes it in eax, FS_AddGameDirectory in eax, edx
void eth32_FS_Startup3()
{
	static char *gameDir;
	static char arg2[] = "";

	asm __volatile__ (
			"movl %%eax, %0\n\t"
		 	"movl %1, %%eax\n\t"
		  	"movl %2, %%edx\n\t"
			"call *%3\n\t"
			"movl %4, %%eax\n\t"
			"call *%5\n\t"
	: "=g"(gameDir)
	:"g"(eth32.path), "g"(&arg2), "g"(orig_FS_AddGameDirectory2), "g"(gameDir), "g"(orig_FS_Startup2)
	:"%eax", "%edx"
		);
}