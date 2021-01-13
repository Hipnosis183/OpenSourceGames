/*
 * libxml2_xmlmemory.c:  libxml memory allocator wrapper.
 *
 * daniel@veillard.com
 * See Copyright for the status of this software.
 * Portion Copyright � 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#include <string.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

/**
 * MEM_LIST:
 *
 * keep track of all allocated blocks for error reporting
 * Always build the memory list !
 */
#ifdef DEBUG_MEMORY_LOCATION
#ifndef MEM_LIST
#define MEM_LIST /* keep a list of all the allocated memory blocks */
#endif
#endif

#include <libxml2_globals.h>

void xmlMallocBreakpoint(void);

/************************************************************************
 *                                                                      *
 *      Macros, variables and associated types                          *
 *                                                                      *
 ************************************************************************/


/*
 * Each of the blocks allocated begin with a header containing information
 */

#define MEMTAG 0x5aa5

#define MALLOC_TYPE 1
#define REALLOC_TYPE 2
#define STRDUP_TYPE 3
#define MALLOC_ATOMIC_TYPE 4
#define REALLOC_ATOMIC_TYPE 5

typedef struct memnod {
    unsigned int   mh_tag;
    unsigned int   mh_type;
    unsigned long  mh_number;
    size_t         mh_size;
#ifdef MEM_LIST
   struct memnod* mh_next;
   struct memnod* mh_prev;
#endif
   const char*    mh_file;
   unsigned int   mh_line;
}  MEMHDR;


#ifdef SUN4
#define ALIGN_SIZE  16
#else
#define ALIGN_SIZE  sizeof(double)
#endif
#define HDR_SIZE    sizeof(MEMHDR)
#define RESERVE_SIZE (((HDR_SIZE + (ALIGN_SIZE-1)) \
                      / ALIGN_SIZE ) * ALIGN_SIZE)


#define CLIENT_2_HDR(a) ((MEMHDR*) (((char *) (a)) - RESERVE_SIZE))
#define HDR_2_CLIENT(a) ((void*)   (((char *) (a)) + RESERVE_SIZE))

#ifdef MEM_LIST
static MEMHDR *memlist = NULL;
#endif

static void debugmem_tag_error(void *addr);
#ifdef MEM_LIST
static void debugmem_list_add(MEMHDR*);
static void debugmem_list_delete(MEMHDR*);
#endif
#define Mem_Tag_Err(a) debugmem_tag_error(a);

#ifndef TEST_POINT
#define TEST_POINT
#endif

/**
 * xmlMallocBreakpoint:
 *
 * Breakpoint to use in conjunction with xmlMemStopAtBlock. When the block
 * number reaches the specified value this function is called. One need to add a breakpoint
 * to it to get the context in which the given block is allocated.
 */

void
xmlMallocBreakpoint(void) {
	LOAD_GS_DIRECT
    xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("xmlMallocBreakpoint reached on block %d\n"), xmlMemStopAtBlock);
}

/**
 * xmlMallocLoc:
 * @param size an int specifying the size in byte to allocate.
 * @param file the file name or NULL
 * @param line the line number
 *
 * a malloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */
XMLPUBFUNEXPORT void*
xmlMallocLoc(size_t size, const char * file, int line)
{
    LOAD_GS_DIRECT
    MEMHDR *p;
    void *ret;

    if (!xmlMemInitialized) xmlInitMemory();
#ifdef DEBUG_MEMORY
    xmlGenericError(xmlGenericErrorContext, "Malloc(%d)\n",size);
#endif

    TEST_POINT

    p = (MEMHDR *) malloc(RESERVE_SIZE+size);

    if (!p) {
        xmlGenericError(xmlGenericErrorContext, "xmlMallocLoc : Out of free space\n");
        xmlMemoryDump();
        return(NULL);
    }
    p->mh_tag = MEMTAG;
    p->mh_size = size;
    p->mh_type = MALLOC_TYPE;
    p->mh_file = file;
    p->mh_line = line;
    xmlMutexLock(xmlMemMutex);
    p->mh_number = ++g_block;
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif
    xmlMutexUnlock(xmlMemMutex);

#ifdef DEBUG_MEMORY
    xmlGenericError(xmlGenericErrorContext, "Malloc(%d) Ok\n",size);
#endif

    if (xmlMemStopAtBlock == p->mh_number) xmlMallocBreakpoint();

    // agathe problem here
    ret = HDR_2_CLIENT(p);

    if (xmlMemTraceBlockAt == ret) {
        xmlGenericError(xmlGenericErrorContext, "%p : Malloc(%d) Ok\n", xmlMemTraceBlockAt, size);
        xmlMallocBreakpoint();
    }

    TEST_POINT

    return(ret);
}

/**
 * xmlMallocAtomicLoc:
 * @param size an int specifying the size in byte to allocate.
 * @param file the file name or NULL
 * @param line the line number
 *
 * a malloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

XMLPUBFUNEXPORT void *
xmlMallocAtomicLoc(size_t size, const char * file, int line)
{
    LOAD_GS_DIRECT
    MEMHDR *p;
    void *ret;

    if (!xmlMemInitialized) xmlInitMemory();
#ifdef DEBUG_MEMORY
    xmlGenericError(xmlGenericErrorContext,
        "Malloc(%d)\n",size);
#endif

    TEST_POINT

    p = (MEMHDR *) malloc(RESERVE_SIZE+size);

    if (!p) {
        xmlGenericError(xmlGenericErrorContext,
                "xmlMallocLoc : Out of free space\n");
        xmlMemoryDump();
        return(NULL);
    }
    p->mh_tag = MEMTAG;
    p->mh_size = size;
    p->mh_type = MALLOC_ATOMIC_TYPE;
    p->mh_file = file;
    p->mh_line = line;
    xmlMutexLock(xmlMemMutex);
    p->mh_number = ++g_block;
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif
    xmlMutexUnlock(xmlMemMutex);

#ifdef DEBUG_MEMORY
    xmlGenericError(xmlGenericErrorContext,
            "Malloc(%d) Ok\n",size);
#endif

    if (xmlMemStopAtBlock == p->mh_number) xmlMallocBreakpoint();

    ret = HDR_2_CLIENT(p);

    if (xmlMemTraceBlockAt == ret) {
        xmlGenericError(xmlGenericErrorContext,
                        "%p : Malloc(%d) Ok\n", xmlMemTraceBlockAt, size);
        xmlMallocBreakpoint();
    }

    TEST_POINT

    return(ret);
}
/**
 * xmlMemMalloc:
 * @param size an int specifying the size in byte to allocate.
 *
 * a malloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

XMLPUBFUNEXPORT void *
xmlMemMalloc(size_t size)
{
     return(xmlMallocLoc(size, "none", 0));
}

/**
 * xmlReallocLoc:
 * @param ptr the initial memory block pointer
 * @param size an int specifying the size in byte to allocate.
 * @param file the file name or NULL
 * @param line the line number
 *
 * a realloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */

XMLPUBFUNEXPORT void *
xmlReallocLoc(void *ptr,size_t size, const char * file, int line)
{
    LOAD_GS_DIRECT
    MEMHDR *p;
    unsigned long number;

    if (ptr == NULL)
        return(xmlMallocLoc(size, file, line));

    if (!xmlMemInitialized) xmlInitMemory();
    TEST_POINT

    p = CLIENT_2_HDR(ptr);
    number = p->mh_number;
    if (p->mh_tag != MEMTAG) {
       Mem_Tag_Err(p);
         goto error;
    }
    p->mh_tag = ~MEMTAG;
    xmlMutexLock(xmlMemMutex);
    debugMemSize -= p->mh_size;
#ifdef MEM_LIST
    debugmem_list_delete(p);
#endif
    xmlMutexUnlock(xmlMemMutex);

    p = (MEMHDR *) realloc(p,RESERVE_SIZE+size);
    if (!p) {
         goto error;
    }
    if (xmlMemTraceBlockAt == ptr) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("%p : Realloced(%d -> %d) Ok\n"),
                        xmlMemTraceBlockAt, p->mh_size, size);
        xmlMallocBreakpoint();
    }
    p->mh_tag = MEMTAG;
    p->mh_number = number;
    p->mh_type = REALLOC_TYPE;
    p->mh_size = size;
    p->mh_file = file;
    p->mh_line = line;
    xmlMutexLock(xmlMemMutex);
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif
    xmlMutexUnlock(xmlMemMutex);

    TEST_POINT

    return(HDR_2_CLIENT(p));

error:
    return(NULL);
}

/**
 * xmlMemRealloc:
 * @param ptr the initial memory block pointer
 * @param size an int specifying the size in byte to allocate.
 *
 * a realloc() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the allocated area or NULL in case of lack of memory.
 */
XMLPUBFUNEXPORT void *
xmlMemRealloc(void *ptr,size_t size) {
    return(xmlReallocLoc(ptr, size, "none", 0));
}

/**
 * xmlMemFree:
 * @param ptr the memory block pointer
 *
 * a free() equivalent, with error checking.
 */
XMLPUBFUNEXPORT void
xmlMemFree(void *ptr)
{
    LOAD_GS_DIRECT
    MEMHDR *p;
    char *target;

    if (ptr == (void *) -1) {
        xmlGenericError(xmlGenericErrorContext,
            EMBED_ERRTXT("trying to free pointer from freed area\n"));
        goto error;
    }

    if (xmlMemTraceBlockAt == ptr) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("%p : Freed()\n"), xmlMemTraceBlockAt);
        xmlMallocBreakpoint();
    }

    TEST_POINT

    target = (char *) ptr;

    p = CLIENT_2_HDR(ptr);
    if (p->mh_tag != MEMTAG) {
        Mem_Tag_Err(p);
        goto error;
    }
    p->mh_tag = ~MEMTAG;
    memset(target, -1, p->mh_size);
    xmlMutexLock(xmlMemMutex);
    debugMemSize -= p->mh_size;
#ifdef MEM_LIST
    debugmem_list_delete(p);
#endif
    xmlMutexUnlock(xmlMemMutex);

    free(p);

    TEST_POINT

    return;

error:
    xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("xmlMemFree(%lX) error\n"), (unsigned long) ptr);
    xmlMallocBreakpoint();
    return;
}

/**
 * xmlMemStrdupLoc:
 * @param str the initial string pointer
 * @param file the file name or NULL
 * @param line the line number
 *
 * a strdup() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the new string or NULL if allocation error occurred.
 */

XMLPUBFUNEXPORT char *
xmlMemStrdupLoc(const char *str, const char *file, int line)
{
    LOAD_GS_DIRECT
    char *s;
    size_t size = strlen(str) + 1;
    MEMHDR *p;

    if (!xmlMemInitialized) xmlInitMemory();
    TEST_POINT

    p = (MEMHDR *) malloc(RESERVE_SIZE+size);
    if (!p) {
      goto error;
    }
    p->mh_tag = MEMTAG;
    p->mh_size = size;
    p->mh_type = STRDUP_TYPE;
    p->mh_file = file;
    p->mh_line = line;
    xmlMutexLock(xmlMemMutex);
    p->mh_number = ++g_block;
    debugMemSize += size;
    if (debugMemSize > debugMaxMemSize) debugMaxMemSize = debugMemSize;
#ifdef MEM_LIST
    debugmem_list_add(p);
#endif
    xmlMutexUnlock(xmlMemMutex);

    s = (char *) HDR_2_CLIENT(p);

    if (xmlMemStopAtBlock == p->mh_number) xmlMallocBreakpoint();

    if (s != NULL)
      strcpy(s,str);
    else
      goto error;

    TEST_POINT

    if (xmlMemTraceBlockAt == s) {
        xmlGenericError(xmlGenericErrorContext,
                        EMBED_ERRTXT("%p : Strdup() Ok\n"), xmlMemTraceBlockAt);
        xmlMallocBreakpoint();
    }

    return(s);

error:
    return(NULL);
}

/**
 * xmlMemoryStrdup:
 * @param str the initial string pointer
 *
 * a strdup() equivalent, with logging of the allocation info.
 *
 * Returns a pointer to the new string or NULL if allocation error occurred.
 */
XMLPUBFUNEXPORT char *
xmlMemoryStrdup(const char *str) {
    return(xmlMemStrdupLoc(str, "none", 0));
}

/**
 * xmlMemUsed:
 *
 * Provides the amount of memory currently allocated
 *
 * Returns an int representing the amount of memory allocated.
 */

XMLPUBFUNEXPORT int
xmlMemUsed(void) {
	LOAD_GS_DIRECT
     return(debugMemSize);
}

#ifdef MEM_LIST
/**
 * xmlMemContentShow:
 * @param fp a FILE descriptor used as the output file
 * @param p a memory block header
 *
 * tries to show some content from the memory block
 */

static void
xmlMemContentShow(FILE *fp, MEMHDR *p)
{
    int i,j,len = p->mh_size;
    const char *buf = (const char *) HDR_2_CLIENT(p);

    if (p == NULL) {
        fprintf(fp, " NULL");
        return;
    }

    for (i = 0;i < len;i++) {
        if (buf[i] == 0) break;
        if (!isprint((unsigned char) buf[i])) break;
    }
    if ((i < 4) && ((buf[i] != 0) || (i == 0))) {
        if (len >= 4) {
            MEMHDR *q;
            void *cur;

            for (j = 0;j < len -3;j += 4) {
                cur = *((void **) &buf[j]);
                q = CLIENT_2_HDR(cur);
                p = memlist;
                while (p != NULL) {
                    if (p == q) break;
                    p = p->mh_next;
                }
                if ((p != NULL) && (p == q)) {
                    fprintf(fp, " pointer to #%lu at index %d",
                            p->mh_number, j);
                    return;
                }
            }
        }
    } else if ((i == 0) && (buf[i] == 0)) {
        fprintf(fp," null");
    } else {
        if (buf[i] == 0) fprintf(fp," \"%.25s\"", buf);
        else {
            fprintf(fp," [");
            for (j = 0;j < i;j++)
                fprintf(fp,"%c", buf[j]);
            fprintf(fp,"]");
        }
    }
}
#endif

#ifndef XMLENGINE_EXCLUDE_FILE_FUNC
/**
 * xmlMemDisplay:
 * @param fp a FILE descriptor used as the output file, if NULL, the result is
 *       written to the file .memorylist
 *
 * show in-extenso the memory blocks allocated
 */
void
xmlMemDisplay(FILE *fp)
{
#ifdef MEM_LIST
    MEMHDR *p;
    unsigned idx;
    int     nb = 0;
#if defined(HAVE_LOCALTIME) && defined(HAVE_STRFTIME)
    time_t currentTime;
    char buf[500];
    struct tm * tstruct;

    currentTime = time(NULL);
    tstruct = localtime(&currentTime);
    strftime(buf, sizeof(buf) - 1, "%I:%M:%S %p", tstruct);
    fprintf(fp,"      %s\n\n", buf);
#endif


    fprintf(fp,"      MEMORY ALLOCATED : %lu, MAX was %lu\n",
            debugMemSize, debugMaxMemSize);
    fprintf(fp,"BLOCK  NUMBER   SIZE  TYPE\n");
    idx = 0;
    xmlMutexLock(xmlMemMutex);
    p = memlist;
    while (p) {
          fprintf(fp,"%-5u  %6lu %6lu ",idx++,p->mh_number,
                  (unsigned long)p->mh_size);
        switch (p->mh_type) {
           case STRDUP_TYPE:fprintf(fp,"strdup()  in ");break;
           case MALLOC_TYPE:fprintf(fp,"malloc()  in ");break;
          case REALLOC_TYPE:fprintf(fp,"realloc() in ");break;
           case MALLOC_ATOMIC_TYPE:fprintf(fp,"atomicmalloc()  in ");break;
          case REALLOC_ATOMIC_TYPE:fprintf(fp,"atomicrealloc() in ");break;
                    default:fprintf(fp,"   ???    in ");break;
        }
          if (p->mh_file != NULL) fprintf(fp,"%s(%d)", p->mh_file, p->mh_line);
        if (p->mh_tag != MEMTAG)
              fprintf(fp,"  INVALID");
        nb++;
        if (nb < 100)
            xmlMemContentShow(fp, p);
        else
            fprintf(fp," skip");

        fprintf(fp,"\n");
        p = p->mh_next;
    }
    xmlMutexUnlock(xmlMemMutex);
#else
    fprintf(fp,EMBED_ERRTXT("Memory list not compiled (MEM_LIST not defined !)\n"));
#endif
}
#endif /*  XMLENGINE_EXCLUDE_FILE_FUNC */

#ifdef MEM_LIST

static void debugmem_list_add(MEMHDR *p)
{
     p->mh_next = memlist;
     p->mh_prev = NULL;
     if (memlist) memlist->mh_prev = p;
     memlist = p;
#ifdef MEM_LIST_DEBUG
     if (stderr)
     Mem_Display(stderr);
#endif
}

static void debugmem_list_delete(MEMHDR *p)
{
     if (p->mh_next)
     p->mh_next->mh_prev = p->mh_prev;
     if (p->mh_prev)
     p->mh_prev->mh_next = p->mh_next;
     else memlist = p->mh_next;
#ifdef MEM_LIST_DEBUG
     if (stderr)
     Mem_Display(stderr);
#endif
}

#endif

/*
 * debugmem_tag_error:
 *
 * internal error function.
 */

static void debugmem_tag_error(void *p)
{
     xmlGenericError(xmlGenericErrorContext,
         EMBED_ERRTXT("Memory tag error occurs :%p \n\t bye\n"), p);
#ifdef MEM_LIST
     if (stderr)
     xmlMemDisplay(stderr);
#endif
}

#ifdef MEM_LIST
static FILE *xmlMemoryDumpFile = NULL;
#endif

#ifndef XMLENGINE_EXCLUDE_FILE_FUNC
/**
 * xmlMemShow:
 * @param fp a FILE descriptor used as the output file
 * @param nr number of entries to dump
 *
 * show a show display of the memory allocated, and dump
 * the nr last allocated areas which were not freed
 */
void
xmlMemShow(FILE *fp, int nr ATTRIBUTE_UNUSED)
{
#ifdef MEM_LIST
    MEMHDR *p;
#endif

    if (fp != NULL)
        fprintf(fp,EMBED_ERRTXT("      MEMORY ALLOCATED : %lu, MAX was %lu\n"),
                debugMemSize, debugMaxMemSize);
#ifdef MEM_LIST
    xmlMutexLock(xmlMemMutex);
    if (nr > 0) {
        fprintf(fp,"NUMBER   SIZE  TYPE   WHERE\n");
        p = memlist;
        while ((p) && nr > 0) {
              fprintf(fp,"%6lu %6lu ",p->mh_number,(unsigned long)p->mh_size);
            switch (p->mh_type) {
               case STRDUP_TYPE:fprintf(fp,"strdup()  in ");break;
               case MALLOC_TYPE:fprintf(fp,"malloc()  in ");break;
               case MALLOC_ATOMIC_TYPE:fprintf(fp,"atomicmalloc()  in ");break;
              case REALLOC_TYPE:fprintf(fp,"realloc() in ");break;
              case REALLOC_ATOMIC_TYPE:fprintf(fp,"atomicrealloc() in ");break;
                default:fprintf(fp,"   ???    in ");break;
            }
            if (p->mh_file != NULL)
                fprintf(fp,"%s(%d)", p->mh_file, p->mh_line);
            if (p->mh_tag != MEMTAG)
                fprintf(fp,"  INVALID");
            xmlMemContentShow(fp, p);
            fprintf(fp,"\n");
            nr--;
            p = p->mh_next;
        }
    }
    xmlMutexUnlock(xmlMemMutex);
#endif /* MEM_LIST */
}
#endif /*  XMLENGINE_EXCLUDE_FILE_FUNC */

/**
 * xmlMemoryDump:
 *
 * Dump in-extenso the memory blocks allocated to the file .memorylist
 */

XMLPUBFUNEXPORT void
xmlMemoryDump(void)
{
#ifdef MEM_LIST
    FILE *dump;

    if (debugMaxMemSize == 0)
        return;
    dump = fopen(".memdump", "w");
    if (dump == NULL)
        xmlMemoryDumpFile = stderr;
    else xmlMemoryDumpFile = dump;

    xmlMemDisplay(xmlMemoryDumpFile);

    if (dump != NULL) fclose(dump);
#endif /* MEM_LIST */
}


/****************************************************************
 *                                                              *
 *              Initialization Routines                         *
 *                                                              *
 ****************************************************************/

/**
 * xmlInitMemory:
 *
 * Initialize the memory layer.
 *
 * Returns 0 on success
 *
 * OOM: maybe possible -- during calls to standard library (???)
 */
XMLPUBFUNEXPORT int
xmlInitMemory(void)
{
#ifdef DEBUG_MEMORY
     xmlGenericError(xmlGenericErrorContext, "xmlInitMemory() Ok\n");
#endif
     return(0);
}

/**
 * xmlCleanupMemory:
 *
 * Free up all the memory associated with memorys
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlCleanupMemory(void) {
	LOAD_GS_DIRECT
    if (xmlMemInitialized == 0)
        return;

    xmlFreeMutex(xmlMemMutex);
    xmlMemInitialized = 0;
}

