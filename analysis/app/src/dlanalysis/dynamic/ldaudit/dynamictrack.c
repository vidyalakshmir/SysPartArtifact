#define _GNU_SOURCE
#include <link.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
FILE *fp1=NULL;
int fd;
char* pos = "before";
void __attribute__((constructor)) init()
{
}
       
void __attribute__((destructor)) cleanup()
{
  fclose(fp1);
}

__inline__ static void trap_instruction(void)
{
	__asm__ volatile("int $0x03");
}

void show_backtrace (FILE *fp);

/*     The dynamic linker invokes this function to inform the auditing
       library that it is about to search for a shared object.
       Flag of LA_SER_ORIG comes from an ELF DT_NEEDED entry, or dlopen
*/
char *
la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag)    
{
  /*show_backtrace(fp1);
    fflush(fp1);
  fprintf(fp1,"I am here");
	if (strstr(name, "libm") != NULL) {
		fclose(fp1);
    trap_instruction();
	}*/
    fprintf(fp1, "%s\t%d\tla_objsearch(): name = %s; cookie = %p ", pos, getpid(), name, cookie);
    fprintf(fp1, "; flag = %s\n",
            (flag == LA_SER_ORIG) ?    "LA_SER_ORIG" :
            (flag == LA_SER_LIBPATH) ? "LA_SER_LIBPATH" :
            (flag == LA_SER_RUNPATH) ? "LA_SER_RUNPATH" :
            (flag == LA_SER_DEFAULT) ? "LA_SER_DEFAULT" :
            (flag == LA_SER_CONFIG) ?  "LA_SER_CONFIG" :
            (flag == LA_SER_SECURE) ?  "LA_SER_SECURE" :
            "???");
    show_backtrace(fp1);
    fflush(fp1);
    return (char *)name;
}

/*
    The dynamic linker calls this function when a new shared object
    is loaded.
*/
unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie)
{
    fprintf(fp1, "%s\t%d\tla_objopen(): loading \"%s\"; lmid = %s; cookie=%p \n",
            pos,
            getpid(),
            map->l_name,
            (lmid == LM_ID_BASE) ?  "LM_ID_BASE" :
            (lmid == LM_ID_NEWLM) ? "LM_ID_NEWLM" :
            "???",
            cookie);
    show_backtrace(fp1);
    fflush(fp1);
    return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}

/*
  Performs the initial handshake between the dynamic
  linker and the auditing library.
*/
unsigned int
la_version(unsigned int version)
{
    char filename[100];
    pid_t pid = getpid();
    sprintf(filename, "output/ldout_%d.txt",pid);
    fp1 = fopen(filename,"a"); 
    fd = fileno(fp1);
    fprintf(fp1, "%d\tla_version(): version = %u; LAV_CURRENT = %u\n", getpid(),version, LAV_CURRENT);
    fflush(fp1);
    return LAV_CURRENT;
}

/*
The dynamic linker invokes this function after all shared objects
have been loaded, before control is passed to the application
(i.e., before calling main()).  Note that main() may still later
dynamically load objects using dlopen(3).
*/     
void
la_preinit(uintptr_t *cookie)
{
    fprintf(fp1, "%d\tla_preinit(): %p\n\n", getpid(), cookie);
    fflush(fp1);
    pos = "after";
}

/*
       The dynamic linker invokes one of these functions when a symbol
       binding occurs between two shared objects that have been marked
       for auditing notification by la_objopen().
        If Flag == LA_SYMB_DLSYM, the binding resulted from a call to dlsym.
*/
uintptr_t
la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook,
        uintptr_t *defcook, unsigned int *flags, const char *symname)
{
    if(*flags == LA_SYMB_DLSYM)
    {          
          fprintf(fp1, "%s\t%d\tla_symbind64(): symname = %s; sym->st_value = %lx\n",
                pos, getpid(), symname, sym->st_value);
          fprintf(fp1, "        ndx = %u; flags = %#x", ndx, *flags);
          fprintf(fp1, "; refcook = %p; defcook = %p\n", refcook, defcook);
          show_backtrace(fp1);
          fflush(fp1);
    }
    return sym->st_value;
}

      
