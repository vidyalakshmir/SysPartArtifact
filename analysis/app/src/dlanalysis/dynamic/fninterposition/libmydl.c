#include <stdio.h>
#include <string.h>
//#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>
#include <unistd.h>

FILE* fp1 = NULL;
void __attribute__((constructor)) init()
{
    char filename[100];
    pid_t pid = getpid();
    sprintf(filename, "output/fn_interp/fninterp_%d.txt",pid);
    fp1 = fopen(filename,"a"); 
}
       
void __attribute__((destructor)) cleanup()
{
  	fclose(fp1);
}
extern void *_dl_sym(void *, const char *, void *);

static void *(*real_dlsym)(void *, const char *) = NULL;

extern void *dlsym(void *handle, const char *name)
{
	fprintf(fp1,"\nmydlsym (%s) (0x%lx)", name, handle);
	fprintf(fp1," : %p",__builtin_return_address(0));
	if (real_dlsym == NULL)
		real_dlsym = _dl_sym(RTLD_NEXT, "dlsym", dlsym);
	/* my target binary is even asking for dlsym() via dlsym()... */
	if (!strcmp(name, "dlsym")) 
		return (void *)dlsym;
	void* ret_sym_addr = real_dlsym(handle, name);
	fprintf(fp1, "(0x%lx)", ret_sym_addr);
	fflush(fp1);
	return ret_sym_addr;
}

extern void *dlopen(const char *filename, int flags)
{
	fprintf(fp1, "\nmydlopen (%s)", filename);
	fprintf(fp1, " : %p",__builtin_return_address(0));
	static void* (*real_dlopen)(const char *, int) = NULL;
    if (!real_dlopen)
        real_dlopen = _dl_sym(RTLD_NEXT, "dlopen", dlopen);

    void *handle = real_dlopen(filename, flags);
	fprintf(fp1, "(0x%lx)", handle);
	fflush(fp1);
    return handle;
}
