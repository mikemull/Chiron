/* 
 * tclCkalloc.c --
 *    Interface to malloc and free that provides support for debugging problems
 *    involving overwritten, double freeing memory and loss of memory.
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * This code contributed by Karl Lehenbauer and Mark Diekhans
 *
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <mem.h>
#include "tclCkalloc.h"

#define FALSE	0
#define TRUE	1

#define GUARD_SIZE 8

#define panic printf

struct mem_header {
        long               length;
        char              *file;
        int                line;
        struct mem_header *flink;
        struct mem_header *blink;
        unsigned char      low_guard[GUARD_SIZE];
        char               body[1];
};

static struct mem_header *allocHead = NULL;  /* List of allocated structures */

#define GUARD_VALUE  0341

/* static char high_guard[] = {0x89, 0xab, 0xcd, 0xef}; */

static int total_reallocs = 0;
static int total_mallocs = 0;
static int total_frees = 0;
static int current_bytes_malloced = 0;
static int maximum_bytes_malloced = 0;
static int current_malloc_packets = 0;
static int maximum_malloc_packets = 0;
static int break_on_malloc = 0;
static int trace_on_at_malloc = 0;
static int  alloc_tracing = FALSE;
static int  init_malloced_bodies = FALSE;
#ifdef MEM_VALIDATE
    static int  validate_memory = TRUE;
#else
    static int  validate_memory = FALSE;
#endif


/*
 *----------------------------------------------------------------------
 *
 * dump_memory_info --
 *     Display the global memory management statistics.
 *
 *----------------------------------------------------------------------
 */
static void
dump_memory_info(outFile) 
    FILE *outFile;
{
        fprintf(outFile,"total mallocs             %10d\n", 
                total_mallocs);
        fprintf(outFile,"total reallocs            %10d\n", 
                total_reallocs);
        fprintf(outFile,"total frees               %10d\n", 
                total_frees);
        fprintf(outFile,"current packets allocated %10d\n", 
                current_malloc_packets);
        fprintf(outFile,"current bytes allocated   %10d\n", 
                current_bytes_malloced);
        fprintf(outFile,"maximum packets allocated %10d\n", 
                maximum_malloc_packets);
        fprintf(outFile,"maximum bytes allocated   %10d\n", 
                maximum_bytes_malloced);
}

/*
 *----------------------------------------------------------------------
 *
 * ValidateMemory --
 *     Procedure to validate allocted memory guard zones.
 *
 *----------------------------------------------------------------------
 */
static void
ValidateMemory (memHeaderP, file, line, nukeGuards)
    struct mem_header *memHeaderP;
    char              *file;
    int                line;
    int                nukeGuards;
{
    unsigned char *hiPtr;
    int   idx;
    int   guard_failed = FALSE;

    for (idx = 0; idx < GUARD_SIZE; idx++)
        if (*(memHeaderP->low_guard + idx) != GUARD_VALUE) {
            guard_failed = TRUE;
            fflush (stdout);
            fprintf(stdout, "low guard byte %d is 0x%x\n", idx,
                    *(memHeaderP->low_guard + idx) & 0xff);
        }

    if (guard_failed) {
        dump_memory_info (stdout);
        fprintf (stdout, "low guard failed at %lx, %s %d\n",
                 memHeaderP->body, file, line);
        fflush (stdout);  /* In case name pointer is bad. */
        fprintf (stdout, "Allocated at (%s %d)\n", memHeaderP->file,
                 memHeaderP->line);
        panic ("Memory validation failure");
    }

    hiPtr = (unsigned char *)memHeaderP->body + memHeaderP->length;
    for (idx = 0; idx < GUARD_SIZE; idx++)
        if (*(hiPtr + idx) != GUARD_VALUE) {
            guard_failed = TRUE;
            fflush (stdout);
            fprintf(stdout, "hi guard byte %d is 0x%x\n", idx,
                    *(hiPtr+idx) & 0xff);
        }

    if (guard_failed) {
        dump_memory_info (stdout);
        fprintf (stdout, "high guard failed at %lx, %s %d\n",
                 memHeaderP->body, file, line);
        fflush (stdout);  /* In case name pointer is bad. */
        fprintf (stdout, "Allocated at (%s %d)\n", memHeaderP->file,
                 memHeaderP->line);
        panic ("Memory validation failure");
    }

    if (nukeGuards) {
        memset ((char *) memHeaderP->low_guard, 0, GUARD_SIZE); 
        memset ((char *) hiPtr, 0, GUARD_SIZE); 
    }

}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ValidateAllMemory --
 *     Validates guard regions for all allocated memory.
 *
 *----------------------------------------------------------------------
 */
void
Tcl_ValidateAllMemory (file, line)
    char  *file;
    int    line;
{
    struct mem_header *memScanP;

    for (memScanP = allocHead; memScanP != NULL; memScanP = memScanP->flink)
        ValidateMemory (memScanP, file, line, FALSE);

}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AnyActiveMemory --
 *     Returns 1 if any memory still allocated
 *----------------------------------------------------------------------
 */
int
Tcl_AnyActiveMemory ()
{
  return allocHead ? TRUE : FALSE;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DumpMemoryStats --
 *     Displays all stats on allocated memory to stdout.
 *
 * Results:
 *     Return TCL_ERROR if an error accessing the file occurs, `errno' 
 *     will have the file error number left in it.
 *----------------------------------------------------------------------
 */
int
Tcl_DumpMemoryStats (fileName)
     char *fileName;
{
  FILE *fileP;

  if (fileName) {
    fileP = fopen (fileName, "w");
    if (fileP == NULL)
      return TCL_ERROR;
  } else
    fileP = stdout;

  dump_memory_info (stdout);

  if (fileName)
    fclose(fileP);
  return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_DumpActiveMemory --
 *     Displays all allocated memory to stdout.
 *
 * Results:
 *     Return TCL_ERROR if an error accessing the file occurs, `errno' 
 *     will have the file error number left in it.
 *----------------------------------------------------------------------
 */
int
Tcl_DumpActiveMemory (fileName)
    char *fileName;
{
    FILE              *fileP;
    struct mem_header *memScanP;
    char              *address;

    if (fileName) {
      fileP = fopen (fileName, "w");
      if (fileP == NULL)
        return TCL_ERROR;
    } else
      fileP = stdout;

    if (allocHead) {
      fprintf (fileP, "\nAllocated region        size\tfile\tline\n");
      for (memScanP = allocHead; memScanP != NULL; memScanP = memScanP->flink) {
        address = &memScanP->body [0];
        fprintf (fileP, "%8lx - %8lx  %7d @ %s %d\n", address,
                 address + memScanP->length - 1, memScanP->length,
                 memScanP->file, memScanP->line);
      }
      fprintf( fileP, "\n");
    }
    if (fileName)
      fclose (fileP);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbCkalloc - debugging ckalloc
 *
 *        Allocate the requested amount of space plus some extra for
 *        guard bands at both ends of the request, plus a size, panicing 
 *        if there isn't enough space, then write in the guard bands
 *        and return the address of the space in the middle that the
 *        user asked for.
 *
 *        The second and third arguments are file and line, these contain
 *        the filename and line number corresponding to the caller.
 *        These are sent by the ckalloc macro; it uses the preprocessor
 *        autodefines __FILE__ and __LINE__.
 *
 *----------------------------------------------------------------------
 */
char *
Tcl_DbCkalloc(size, file, line)
    unsigned int size;
    char        *file;
    int          line;
{
    struct mem_header *result;

    if (validate_memory)
        Tcl_ValidateAllMemory (file, line);

    result = (struct mem_header *)malloc((unsigned)size + 
                              sizeof(struct mem_header) + GUARD_SIZE);
    if (result == NULL) {
        fflush(stdout);
        dump_memory_info(stdout);
        panic("unable to alloc %d bytes, %s line %d", size, file, 
              line);
    }

    /*
     * Fill in guard zones and size.  Link into allocated list.
     */
    result->length = size;
    result->file = file;
    result->line = line;
    memset ((char *) result->low_guard, GUARD_VALUE, GUARD_SIZE);
    memset (result->body + size, GUARD_VALUE, GUARD_SIZE);
    result->flink = allocHead;
    result->blink = NULL;
    if (allocHead != NULL)
        allocHead->blink = result;
    allocHead = result;

    total_mallocs++;
    if (trace_on_at_malloc && (total_mallocs >= trace_on_at_malloc)) {
        (void) fflush(stdout);
        fprintf(stdout, "reached malloc trace enable point (%d)\n",
                total_mallocs);
        fflush(stdout);
        alloc_tracing = TRUE;
        trace_on_at_malloc = 0;
    }

    if (alloc_tracing)
        fprintf(stdout,"ckalloc %lx %d %s %d\n", result->body, size,
                file, line);

    if (break_on_malloc && (total_mallocs >= break_on_malloc)) {
        break_on_malloc = 0;
        (void) fflush(stdout);
        fprintf(stdout,"reached malloc break limit (%d)\n",
                total_mallocs);
        fprintf(stdout, "program will now enter C debugger\n");
        (void) fflush(stdout);
/*        kill (getpid(), SIGINT);*/
    }

    current_malloc_packets++;
    if (current_malloc_packets > maximum_malloc_packets)
        maximum_malloc_packets = current_malloc_packets;
    current_bytes_malloced += size;
    if (current_bytes_malloced > maximum_bytes_malloced)
        maximum_bytes_malloced = current_bytes_malloced;

    if (init_malloced_bodies)
        memset (result->body, 0xff, (int) size);

    return result->body;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbCkrealloc - debugging ckrealloc
 *
 *        Reallocate the ptr for the requested amount of space plus some
 *	  extra for
 *        guard bands at both ends of the request, plus a size, panicing 
 *        if there isn't enough space, then write in the guard bands
 *        and return the address of the space in the middle that the
 *        user asked for.
 *
 *        The third and fourth arguments are file and line, these contain
 *        the filename and line number corresponding to the caller.
 *        These are sent by the ckalloc macro; it uses the preprocessor
 *        autodefines __FILE__ and __LINE__.
 *
 *----------------------------------------------------------------------
 */
char *
Tcl_DbCkrealloc(ptr, size, file, line)
     char	*ptr;
    unsigned int size;
    char        *file;
    int          line;
{
    struct mem_header *memp = 0; /* Must be zero for size calc */
    unsigned osize;

    /*
     * Since header ptr is zero, body offset will be size
     */
    memp = (struct mem_header *)(((char *) ptr) - (int)memp->body);

    if (alloc_tracing)
        fprintf(stdout, "ckrealloc %lx %ld (new: %ld) %s %d\n", memp->body,
                memp->length, size, file, line);

    if (validate_memory)
        Tcl_ValidateAllMemory (file, line);

    ValidateMemory (memp, file, line, FALSE);

    memp = (struct mem_header *)realloc(memp, (unsigned)size + 
                              sizeof(struct mem_header) + GUARD_SIZE);
    if (memp == NULL) {
        fflush(stdout);
        dump_memory_info(stdout);
        panic("unable to realloc %d bytes, %s line %d", size, file, 
              line);
    }

    /*
     * Fill in size.  Re-Link into allocated list.
     */
    osize = memp->length;
    memp->length = size;
    memp->file = file;
    memp->line = line;
    memset (memp->body + size, GUARD_VALUE, GUARD_SIZE);
    if (memp->flink)
      memp->flink->blink = memp;
    if (memp->blink)
      memp->blink->flink = memp;
    else
      allocHead = memp;

    total_reallocs++;

    current_bytes_malloced += size - osize;
    if (current_bytes_malloced > maximum_bytes_malloced)
        maximum_bytes_malloced = current_bytes_malloced;

    return memp->body;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DbCkfree - debugging ckfree
 *
 *        Verify that the low and high guards are intact, and if so
 *        then free the buffer else panic.
 *
 *        The guards are erased after being checked to catch duplicate
 *        frees.
 *
 *        The second and third arguments are file and line, these contain
 *        the filename and line number corresponding to the caller.
 *        These are sent by the ckfree macro; it uses the preprocessor
 *        autodefines __FILE__ and __LINE__.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_DbCkfree(ptr, file, line)
    char *  ptr;
    char     *file;
    int       line;
{
    struct mem_header *memp = 0;  /* Must be zero for size calc */

    /*
     * Since header ptr is zero, body offset will be size
     */
    memp = (struct mem_header *)(((char *) ptr) - (int)memp->body);

    if (alloc_tracing)
        fprintf(stdout, "ckfree %lx %ld %s %d\n", memp->body,
                memp->length, file, line);

    if (validate_memory)
        Tcl_ValidateAllMemory (file, line);

    ValidateMemory (memp, file, line, TRUE);

    total_frees++;
    current_malloc_packets--;
    current_bytes_malloced -= memp->length;

    /*
     * Delink from allocated list
     */
    if (memp->flink != NULL)
        memp->flink->blink = memp->blink;
    if (memp->blink != NULL)
        memp->blink->flink = memp->flink;
    if (allocHead == memp)
        allocHead = memp->flink;
    free((char *) memp);
    return 0;
}
