

#include "err.hpp"

#ifdef __unix__         

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#endif //__unix__         


#define RESET "\033[0m"
#define RED   "\033[0;31m"
namespace Err
{

#ifdef __unix__         

    void segfault(int sig) 
    {
      const int trace = 30;
      void *array[trace];
      size_t size;

      // get void*'s for all entries on the stack
      size = backtrace(array, trace);
      // print out all the frames to stderr
      fprintf(stderr, "Error: signal %d:\n", sig);
      char ** messages = backtrace_symbols(array, size);
      for (int i=1; i<trace; ++i)
      {
        printf("[%sERR%s] #%d %s\n", RED, RESET, i, messages[i]);
        int p = 0;
        while(messages[i][p] != '(' && messages[i][p] != ' ' && messages[i][p] != 0)
            ++p;
        char syscall[256];
        sprintf(syscall,"addr2line %p -e %.*s", array[i], p, messages[i]);
        system(syscall);
      }
      exit(1);
    }

    void Stacktrace()
    {
        signal(SIGSEGV, segfault); 
    }

#else 
    void Stacktrace()
    {
    
    }
#endif //__unix__         

} // namespace Err

#undef RESET
#undef RED