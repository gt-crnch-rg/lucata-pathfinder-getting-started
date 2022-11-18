#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cilk.h>

#include <memoryweb/memoryweb.h>
#include <memoryweb/timing.h>

const char str[] = "Hello, world!";

static inline void copy_ptr (char *pc, const long *pl) { *pc = (char)*pl; }

replicated long * ptr;
replicated char * str_out;

int main (void)
{
     long n = strlen (str) + 1;

     mw_replicated_init ((long*)&ptr, (long)mw_malloc1dlong (n));
     mw_replicated_init ((long*)&str_out, (long)malloc (n * sizeof (char)));

     // starttiming(); Deprecated
     lu_profile_perfcntr(PFC_CLEAR, "HELLO WORLD SPAWN CLEARING COUNTERS");
     //lu_profile_perfcntr(PFC_READ, "HELLO WORLD SPAWN READING COUNTERS AFTER CLEARING");
     lu_profile_perfcntr(PFC_START, "HELLO WORLD SPAWN STARTING COUNTERS");
    
     for (long k = 0; k < n; ++k)
          ptr[k] = (long)str[k]; // Remote writes

     for (long k = 0; k < n; ++k)
          cilk_spawn copy_ptr (&str_out[k], &ptr[k]);

     cilk_sync;

     printf("%s\n", str_out);  // Migration back
     lu_profile_perfcntr(PFC_STOP, "HELLO WORLD SPAWN STOPPING COUNTERS AT END");
    
     return 0;
}
