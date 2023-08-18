
#include <stdio.h>
//#if 0
#define UNW_LOCAL_ONLY
#include <libunwind.h>

void show_backtrace (FILE *fp) {
    unw_cursor_t cursor; unw_context_t uc;
    unw_word_t ip, sp;    
    unw_getcontext(&uc);
    unw_init_local(&cursor, &uc);
    int ret = unw_step(&cursor);
    while (ret > 0) 
    {
      unw_get_reg(&cursor, UNW_REG_IP, &ip);
      unw_get_reg(&cursor, UNW_REG_SP, &sp);
      fprintf (fp, "ip = %lx, sp = %lx", (long) ip, (long) sp);
      char sym[256];
      unw_word_t offset;
      if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
        fprintf(fp," (%s+0x%lx)\n", sym, offset);
      } else {
        fprintf(fp," -- error: unable to obtain symbol name for this frame\n");
      }
      fprintf(fp,"\n");
      ret = unw_step(&cursor);
    }

    if(ret == 0)
      fprintf(fp, "END OF STACK TRACE\n\n");
    else if(ret < 0)
      fprintf(fp, "ERROR STACK TRACE\n\n");
}
/*#else

void show_backtrace (FILE *fp)
{
	fprintf(fp, "No backtrace\n");
}
#endif */
