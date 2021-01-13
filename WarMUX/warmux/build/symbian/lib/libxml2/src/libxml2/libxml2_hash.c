/*
 * libxml2_hash.c: chained hash tables
 *
 * Reference: Your favorite introductory book on algorithms
 *
 * Copyright (C) 2000 Bjorn Reese and Daniel Veillard.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
 * Author: breese@users.sourceforge.net
 * Portion Copyright � 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#include <string.h>
#include <libxml2_globals.h>

#define MAX_HASH_LEN 8

/* #define DEBUG_GROW */

/*
 * A single entry in the hash table
 */
typedef struct _xmlHashEntry xmlHashEntry;
typedef xmlHashEntry* xmlHashEntryPtr;
struct _xmlHashEntry {
    struct _xmlHashEntry *next;
    xmlChar *name;
    xmlChar *name2;
    xmlChar *name3;
    void *payload;
    int valid;
};

/*
 * The entire hash table
 */
struct _xmlHashTable {
    xmlHashEntryPtr table;
    int size;
    int nbElems;
};

/*
 * xmlHashComputeKey:
 * Calculate the hash key
 *
 * OOM: never
 */
static unsigned long
xmlHashComputeKey(xmlHashTablePtr table, const xmlChar *name,
              const xmlChar *name2, const xmlChar *name3) {
    unsigned long value = 0L;
    char ch;

    if (name != NULL) {
    value += 30 * (*name);
    while ((ch = *name++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    }
    if (name2 != NULL) {
    while ((ch = *name2++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    }
    if (name3 != NULL) {
    while ((ch = *name3++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    }
    return (value % table->size);
}

static unsigned long
xmlHashComputeQKey(xmlHashTablePtr table,
           const xmlChar *prefix, const xmlChar *name,
           const xmlChar *prefix2, const xmlChar *name2,
           const xmlChar *prefix3, const xmlChar *name3) {
    unsigned long value = 0L;
    char ch;

    if (prefix != NULL)
    value += 30 * (*prefix);
    else
    value += 30 * (*name);

    if (prefix != NULL) {
    while ((ch = *prefix++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)':');
    }
    if (name != NULL) {
    while ((ch = *name++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    }
    if (prefix2 != NULL) {
    while ((ch = *prefix2++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)':');
    }
    if (name2 != NULL) {
    while ((ch = *name2++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    }
    if (prefix3 != NULL) {
    while ((ch = *prefix3++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)':');
    }
    if (name3 != NULL) {
    while ((ch = *name3++) != 0) {
        value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
    }
    }
    return (value % table->size);
}

/**
 * xmlHashCreate:
 * @param size the size of the hash table
 *
 * Create a new xmlHashTablePtr.
 *
 * Returns the newly created object, or NULL if an error occured.
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlHashTablePtr
xmlHashCreate(int size) {
    xmlHashTablePtr table;

    if (size <= 0)
        size = 256;

    table = (xmlHashTablePtr) xmlMalloc(sizeof(xmlHashTable));
    if (table) {
        table->size = size;
        table->nbElems = 0;
        table->table = (xmlHashEntryPtr)xmlMalloc(size * sizeof(xmlHashEntry)); // may set OOM flag
        if (table->table) {
            memset(table->table, 0, size * sizeof(xmlHashEntry));
            return(table);
        }
        xmlFree(table);
    }
    return(NULL);
}

/**
 * xmlHashGrow:
 * @param table the hash table
 * @param size the new size of the hash table
 *
 * resize the hash table
 *
 * Returns 0 in case of success, -1 in case of failure
 *
 * OOM: possible --> sets OOM flag when returns -1
 */
static int
xmlHashGrow(xmlHashTablePtr table, int size) {
    unsigned long key;
    int oldsize, i;
    xmlHashEntryPtr iter, next;
    struct _xmlHashEntry *oldtable;
#ifdef DEBUG_GROW
    unsigned long nbElem = 0;
#endif

    if (table == NULL)
        return(-1);
    if (size < 8)
        return(-1);
    if (size > 8 * 2048)
        return(-1);

    oldsize = table->size;
    oldtable = table->table;
    if (oldtable == NULL)
        return(-1);

    table->table = (xmlHashEntryPtr)xmlMalloc(size * sizeof(xmlHashEntry)); // may set OOM flag
    if (table->table == NULL) {
        table->table = oldtable;
        return(-1);
    }
    memset(table->table, 0, size * sizeof(xmlHashEntry));
    table->size = size;

    /*  If the two loops are merged, there would be situations where
    a new entry needs to allocated and data copied into it from
    the main table. So instead, we run through the array twice, first
    copying all the elements in the main array (where we can't get
    conflicts) and then the rest, so we only free (and don't allocate)
    */
    for (i = 0; i < oldsize; i++) {
        
        if (oldtable[i].valid == 0)
            continue;
        key = xmlHashComputeKey(
                    table,
                    oldtable[i].name,
                    oldtable[i].name2,
                    oldtable[i].name3);
        memcpy(&(table->table[key]), &(oldtable[i]), sizeof(xmlHashEntry));
        table->table[key].next = NULL;
    }

    for (i = 0; i < oldsize; i++) {
        iter = oldtable[i].next;
        while (iter) {
            next = iter->next;

            /*
            * put back the entry in the new table
            */

            key = xmlHashComputeKey(
                    table,
                    iter->name,
                    iter->name2,
                    iter->name3);
            if (table->table[key].valid == 0) {
                memcpy(&(table->table[key]), iter, sizeof(xmlHashEntry));
                table->table[key].next = NULL;
                xmlFree(iter);
            } else {
                iter->next = table->table[key].next;
                table->table[key].next = iter;
            }

#ifdef DEBUG_GROW
            nbElem++;
#endif
            iter = next;
        }
    }

    xmlFree(oldtable);

#ifdef DEBUG_GROW
    xmlGenericError(xmlGenericErrorContext,
        "xmlHashGrow : from %d to %d, %d elems\n", oldsize, size, nbElem);
#endif
    return(0);
}

/**
 * xmlHashFree:
 * @param table the hash table
 * @param f the deallocator function for items in the hash
 *
 * Free the hash table and its contents. The userdata is
 * deallocated with f if provided.
 *
 * OOM: never //  same as argument 'f' has when f!=NULL
 */
XMLPUBFUNEXPORT void
xmlHashFree(xmlHashTablePtr table, xmlHashDeallocator f) {
    int i;
    xmlHashEntryPtr iter;
    xmlHashEntryPtr next;
    int inside_table = 0;
    int nbElems;

    if (table == NULL)
        return;
    if (table->table) {
    nbElems = table->nbElems;
    for(i = 0; (i < table->size) && (nbElems > 0); i++) {
        iter = &(table->table[i]);
        if (iter->valid == 0)
        continue;
        inside_table = 1;
        while (iter) {
        next = iter->next;
        if ((f != NULL) && (iter->payload != NULL))
            f(iter->payload, iter->name);
        if (iter->name)
            xmlFree(iter->name);
        if (iter->name2)
            xmlFree(iter->name2);
        if (iter->name3)
            xmlFree(iter->name3);
        iter->payload = NULL;
        if (!inside_table)
            xmlFree(iter);
        nbElems--;
        inside_table = 0;
        iter = next;
        }
        inside_table = 0;
    }
    xmlFree(table->table);
    }
    xmlFree(table);
}

/**
 * xmlHashAddEntry:
 * @param table the hash table
 * @param name the name of the userdata
 * @param userdata a pointer to the userdata
 *
 * Add the userdata to the hash table. This can later be retrieved
 * by using the name. Duplicate names generate errors.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 *
 * OOM: iif returns -1 and flag is set
 */
XMLPUBFUNEXPORT int
xmlHashAddEntry(xmlHashTablePtr table, const xmlChar *name, void *userdata) {
    return(xmlHashAddEntry3(table, name, NULL, NULL, userdata));
}

/**
 * xmlHashAddEntry2:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param userdata a pointer to the userdata
 *
 * Add the userdata to the hash table. This can later be retrieved
 * by using the (name, name2) tuple. Duplicate tuples generate errors.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlHashAddEntry2(xmlHashTablePtr table, const xmlChar *name,
            const xmlChar *name2, void *userdata) {
    return(xmlHashAddEntry3(table, name, name2, NULL, userdata));
}

/**
 * xmlHashUpdateEntry:
 * @param table the hash table
 * @param name the name of the userdata
 * @param userdata a pointer to the userdata
 * @param f the deallocator function for replaced item (if any)
 *
 * Add the userdata to the hash table. This can later be retrieved
 * by using the name. Existing entry for this name will be removed
 * and freed with f if found.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlHashUpdateEntry(xmlHashTablePtr table, const xmlChar *name,
               void *userdata, xmlHashDeallocator f) {
    return(xmlHashUpdateEntry3(table, name, NULL, NULL, userdata, f));
}

/**
 * xmlHashUpdateEntry2:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param userdata a pointer to the userdata
 * @param f the deallocator function for replaced item (if any)
 *
 * Add the userdata to the hash table. This can later be retrieved
 * by using the (name, name2) tuple. Existing entry for this tuple will
 * be removed and freed with f if found.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlHashUpdateEntry2(xmlHashTablePtr table, const xmlChar *name,
               const xmlChar *name2, void *userdata,
           xmlHashDeallocator f) {
    return(xmlHashUpdateEntry3(table, name, name2, NULL, userdata, f));
}

/**
 * xmlHashLookup:
 * @param table the hash table
 * @param name the name of the userdata
 *
 * Find the userdata specified by the name.
 *
 * Returns the pointer to the userdata
 */
XMLPUBFUNEXPORT void *
xmlHashLookup(xmlHashTablePtr table, const xmlChar *name) {
    return(xmlHashLookup3(table, name, NULL, NULL));
}

/**
 * xmlHashLookup2:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 *
 * Find the userdata specified by the (name, name2) tuple.
 *
 * Returns the pointer to the userdata
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void*
xmlHashLookup2(xmlHashTablePtr table, const xmlChar *name,
          const xmlChar *name2) {
    return(xmlHashLookup3(table, name, name2, NULL));
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlHashQLookup:
 * @param table the hash table
 * @param prefix the prefix of the userdata
 * @param name the name of the userdata
 *
 * Find the userdata specified by the QName prefix:name/name.
 *
 * Returns the pointer to the userdata
 *
 * OOM: never
 */
void*
xmlHashQLookup(xmlHashTablePtr table, const xmlChar *prefix,
               const xmlChar *name) {
    return(xmlHashQLookup3(table, prefix, name, NULL, NULL, NULL, NULL));
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlHashQLookup2:
 * @param table the hash table
 * @param prefix the prefix of the userdata
 * @param name the name of the userdata
 * @param prefix2 the second prefix of the userdata
 * @param name2 a second name of the userdata
 *
 * Find the userdata specified by the QNames tuple
 *
 * Returns the pointer to the userdata
 */
XMLPUBFUNEXPORT void *
xmlHashQLookup2(xmlHashTablePtr table, const xmlChar *prefix,
                const xmlChar *name, const xmlChar *prefix2,
            const xmlChar *name2) {
    return(xmlHashQLookup3(table, prefix, name, prefix2, name2, NULL, NULL));
}

/**
 * xmlHashAddEntry3:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param name3 a third name of the userdata
 * @param userdata a pointer to the userdata
 *
 * Add the userdata to the hash table. This can later be retrieved
 * by using the tuple (name, name2, name3). Duplicate entries generate
 * errors.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 *
 * OOM: iif returns -1 and flag is set
 */
XMLPUBFUNEXPORT int
xmlHashAddEntry3(xmlHashTablePtr table, const xmlChar *name,
             const xmlChar *name2, const xmlChar *name3,
         void *userdata) {
	LOAD_GS_DIRECT         
    unsigned long key, len = 0;
    xmlHashEntryPtr entry;
    xmlHashEntryPtr insert;
    int res;

    if ((table == NULL) || name == NULL)
        return(-1);

    /*
     * Check for duplicate and insertion location.
     */
    key = xmlHashComputeKey(table, name, name2, name3);
    if (table->table[key].valid == 0) {
        insert = NULL;
    } else {
        for (insert = &(table->table[key]); insert->next != NULL;
            insert = insert->next) {
            if ((xmlStrEqual(insert->name, name)) &&
            (xmlStrEqual(insert->name2, name2)) &&
            (xmlStrEqual(insert->name3, name3)))
            return(-1);
            len++;
        }
        if ((xmlStrEqual(insert->name, name)) &&
            (xmlStrEqual(insert->name2, name2)) &&
            (xmlStrEqual(insert->name3, name3)))
            return(-1);
    }

    if (insert == NULL) {
        entry = &(table->table[key]);
    } else {
        entry = (xmlHashEntryPtr) xmlMalloc(sizeof(xmlHashEntry)); // may set OOM flag
        if (entry == NULL)
            return(-1);
    }
    // DONE: Check OOM                               // DONE: Try to avoid xmlStrdup(NULL)
    
    
    entry->name  = xmlStrdup(name); // name is never NULL here
    entry->name2 = name2 ? xmlStrdup(name2): (xmlChar*) name2 /* it's NULL here */;
    entry->name3 = name3 ? xmlStrdup(name3): (xmlChar*) name3 /* it's NULL here */;
    if(OOM_FLAG){
        if(entry->name)  xmlFree(entry->name);
        if(entry->name2) xmlFree(entry->name2);
        if(entry->name3) xmlFree(entry->name3);
        if(insert) xmlFree(entry);
        return -1;
    }
    entry->payload = userdata;
    entry->next = NULL;
    entry->valid = 1;

    if (insert != NULL)
        insert->next = entry;

    table->nbElems++;

    res = 0;
    if (len > MAX_HASH_LEN){
       res = xmlHashGrow(table, MAX_HASH_LEN * table->size); 
    }

    return(res);
}

/**
 * xmlHashUpdateEntry3:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param name3 a third name of the userdata
 * @param userdata a pointer to the userdata
 * @param f the deallocator function for replaced item (if any)
 *
 * Add the userdata to the hash table. This can later be retrieved
 * by using the tuple (name, name2, name3). Existing entry for this tuple
 * will be removed and freed with f if found.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlHashUpdateEntry3(xmlHashTablePtr table, const xmlChar *name,
               const xmlChar *name2, const xmlChar *name3,
           void *userdata, xmlHashDeallocator f) {
	LOAD_GS_DIRECT           
    unsigned long key;
    xmlHashEntryPtr entry;
    xmlHashEntryPtr insert;

    if ((table == NULL) || name == NULL)
        return(-1);

    /*
     * Check for duplicate and insertion location.
     */
    key = xmlHashComputeKey(table, name, name2, name3);
    if (table->table[key].valid == 0) {
    insert = NULL;
    } else {
    for (insert = &(table->table[key]); insert->next != NULL;
         insert = insert->next) {
        if ((xmlStrEqual(insert->name, name)) &&
        (xmlStrEqual(insert->name2, name2)) &&
        (xmlStrEqual(insert->name3, name3))) {
        if (f)
            f(insert->payload, insert->name);
        insert->payload = userdata;
        return(0);
        }
    }
    if ((xmlStrEqual(insert->name, name)) &&
        (xmlStrEqual(insert->name2, name2)) &&
        (xmlStrEqual(insert->name3, name3))) {
        if (f)
        f(insert->payload, insert->name);
        insert->payload = userdata;
        return(0);
    }
    }

    if (!insert) {
        entry =  &(table->table[key]);
    } else {
        entry = (xmlHashEntryPtr)xmlMalloc(sizeof(xmlHashEntry));
        if (entry == NULL)
            return(-1);
    }
    // DONE: Check OOM
    entry->name  = xmlStrdup(name); // name is never NULL here
    entry->name2 = name2 ? xmlStrdup(name2): NULL;
    entry->name3 = name3 ? xmlStrdup(name3): NULL;
    if(OOM_FLAG){
        if(entry->name)  xmlFree(entry->name);
        if(entry->name2) xmlFree(entry->name2);
        if(entry->name3) xmlFree(entry->name3);
        if(insert) xmlFree(entry);
        return -1;
    }
    entry->payload = userdata;
    entry->next = NULL;
    entry->valid = 1;
    table->nbElems++;


    if (insert) {
        insert->next = entry;
    }
    return(0);
}

/**
 * xmlHashLookup3:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param name3 a third name of the userdata
 *
 * Find the userdata specified by the (name, name2, name3) tuple.
 *
 * Returns the a pointer to the userdata
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void *
xmlHashLookup3(xmlHashTablePtr table, const xmlChar *name,
           const xmlChar *name2, const xmlChar *name3)
{
    unsigned long key;
    xmlHashEntryPtr entry;

    if (table == NULL)
        return(NULL);
    if (name == NULL)
        return(NULL);
    key = xmlHashComputeKey(table, name, name2, name3);
    if (table->table[key].valid == 0)
        return(NULL);
    for (entry = &(table->table[key]); entry != NULL; entry = entry->next) {
        if ((xmlStrEqual(entry->name, name)) &&
            (xmlStrEqual(entry->name2, name2)) &&
            (xmlStrEqual(entry->name3, name3)))
            return(entry->payload);
    }
    return(NULL);
}

/**
 * xmlHashQLookup3:
 * @param table the hash table
 * @param prefix the prefix of the userdata
 * @param name the name of the userdata
 * @param prefix2 the second prefix of the userdata
 * @param name2 a second name of the userdata
 * @param prefix3 the third prefix of the userdata
 * @param name3 a third name of the userdata
 *
 * Find the userdata specified by the (name, name2, name3) tuple.
 *
 * Returns the a pointer to the userdata
 */
XMLPUBFUNEXPORT void *
xmlHashQLookup3(xmlHashTablePtr table,
                const xmlChar *prefix, const xmlChar *name,
        const xmlChar *prefix2, const xmlChar *name2,
        const xmlChar *prefix3, const xmlChar *name3) {
    unsigned long key;
    xmlHashEntryPtr entry;

    if (table == NULL)
    return(NULL);
    if (name == NULL)
    return(NULL);
    key = xmlHashComputeQKey(table, prefix, name, prefix2,
                             name2, prefix3, name3);
    if (table->table[key].valid == 0)
    return(NULL);
    for (entry = &(table->table[key]); entry != NULL; entry = entry->next) {
    if ((xmlStrQEqual(prefix, name, entry->name)) &&
        (xmlStrQEqual(prefix2, name2, entry->name2)) &&
        (xmlStrQEqual(prefix3, name3, entry->name3)))
        return(entry->payload);
    }
    return(NULL);
}

typedef struct {
    xmlHashScanner hashscanner;
    void *data;
} stubData;

static void
stubHashScannerFull (void *payload, void *data, const xmlChar *name,
                     const xmlChar *name2 ATTRIBUTE_UNUSED,
             const xmlChar *name3 ATTRIBUTE_UNUSED) {
    stubData *stubdata = (stubData *) data;
    stubdata->hashscanner (payload, stubdata->data, (xmlChar *) name);
}

/**
 * xmlHashScan:
 * @param table the hash table
 * @param f the scanner function for items in the hash
 * @param data extra data passed to f
 *
 * Scan the hash table and applied f to each value.
 */
XMLPUBFUNEXPORT void
xmlHashScan(xmlHashTablePtr table, xmlHashScanner f, void *data) {
    stubData stubdata;
    stubdata.data = data;
    stubdata.hashscanner = f;
    xmlHashScanFull (table, stubHashScannerFull, &stubdata);
}

/**
 * xmlHashScanFull:
 * @param table the hash table
 * @param f the scanner function for items in the hash
 * @param data extra data passed to f
 *
 * Scan the hash table and applied f to each value.
 */
XMLPUBFUNEXPORT void
xmlHashScanFull(xmlHashTablePtr table, xmlHashScannerFull f, void *data) {
    int i;
    xmlHashEntryPtr iter;
    xmlHashEntryPtr next;

    if (table == NULL)
    return;
    if (f == NULL)
    return;

    if (table->table) {
    for(i = 0; i < table->size; i++) {
        if (table->table[i].valid == 0)
        continue;
        iter = &(table->table[i]);
        while (iter) {
        next = iter->next;
        if ((f != NULL) && (iter->payload != NULL))
            f(iter->payload, data, iter->name,
              iter->name2, iter->name3);
        iter = next;
        }
    }
    }
}

/**
 * xmlHashScan3:
 * @param table the hash table
 * @param name the name of the userdata or NULL
 * @param name2 a second name of the userdata or NULL
 * @param name3 a third name of the userdata or NULL
 * @param f the scanner function for items in the hash
 * @param data extra data passed to f
 *
 * Scan the hash table and applied f to each value matching
 * (name, name2, name3) tuple. If one of the names is null,
 * the comparison is considered to match.
 */
XMLPUBFUNEXPORT void
xmlHashScan3(xmlHashTablePtr table, const xmlChar *name,
         const xmlChar *name2, const xmlChar *name3,
         xmlHashScanner f, void *data) {
    xmlHashScanFull3 (table, name, name2, name3,
              (xmlHashScannerFull) f, data);
}

/**
 * xmlHashScanFull3:
 * @param table the hash table
 * @param name the name of the userdata or NULL
 * @param name2 a second name of the userdata or NULL
 * @param name3 a third name of the userdata or NULL
 * @param f the scanner function for items in the hash
 * @param data extra data passed to f
 *
 * Scan the hash table and applied f to each value matching
 * (name, name2, name3) tuple. If one of the names is null,
 * the comparison is considered to match.
 */
XMLPUBFUNEXPORT void
xmlHashScanFull3(xmlHashTablePtr table, const xmlChar *name,
         const xmlChar *name2, const xmlChar *name3,
         xmlHashScannerFull f, void *data) {
    int i;
    xmlHashEntryPtr iter;
    xmlHashEntryPtr next;

    if (table == NULL)
    return;
    if (f == NULL)
    return;

    if (table->table) {
    for(i = 0; i < table->size; i++) {
        if (table->table[i].valid == 0)
        continue;
        iter = &(table->table[i]);
        while (iter) {
        next = iter->next;
        if (((name == NULL) || (xmlStrEqual(name, iter->name))) &&
            ((name2 == NULL) || (xmlStrEqual(name2, iter->name2))) &&
            ((name3 == NULL) || (xmlStrEqual(name3, iter->name3))) &&
            (iter->payload != NULL)) {
            f(iter->payload, data, iter->name,
              iter->name2, iter->name3);
        }
        iter = next;
        }
    }
    }
}

/**
 * xmlHashCopy:
 * @param table the hash table
 * @param f the copier function for items in the hash
 *
 * Scan the hash table and applied f to each value.
 *
 * Returns the new table or NULL in case of error.
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlHashTablePtr
xmlHashCopy(xmlHashTablePtr table, xmlHashCopier f)
{
	LOAD_GS_DIRECT
    int i;
    xmlHashEntryPtr iter;
    xmlHashEntryPtr next;
    xmlHashTablePtr ret;
    //int res; 
    void* pCopy;

    if (table == NULL)
        return(NULL);
    if (f == NULL)
        return(NULL);

    ret = xmlHashCreate(table->size); // may set OOM flag
    if(!ret)
        return NULL; // OOM happened, the flag is set already

    if (table->table) {
        for(i = 0; i < table->size; i++)
        {
            if (table->table[i].valid == 0)
                continue;
            iter = &(table->table[i]);
            while (iter)
            {
                next = iter->next;
                pCopy = f(iter->payload, iter->name);
                if(OOM_FLAG)
                    goto oom;
                //res = 
                xmlHashAddEntry3(
                            ret,
                            iter->name,
                            iter->name2,
                            iter->name3,
                            pCopy);
                if(OOM_FLAG)
                    goto oom;
                iter = next;
            }
        }
    }
    ret->nbElems = table->nbElems;
    return(ret);
oom:
    // OOM during copying entry in  f()
    xmlHashFree(ret, NULL); 
    return NULL;
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlHashSize:
 * @param table the hash table
 *
 * Query the number of elements installed in the hash table.
 *
 * Returns the number of elements in the hash table or
 * -1 in case of error
 */
int
xmlHashSize(xmlHashTablePtr table)
{
    if (table == NULL)
        return(-1);
    return(table->nbElems);
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlHashRemoveEntry:
 * @param table the hash table
 * @param name the name of the userdata
 * @param f the deallocator function for removed item (if any)
 *
 * Find the userdata specified by the name and remove
 * it from the hash table. Existing userdata for this tuple will be removed
 * and freed with f.
 *
 * Returns 0 if the removal succeeded and -1 in case of error or not found.
 *
 * OOM:  never / same as argument function 'f' has, if f!=NULL*
 */
XMLPUBFUNEXPORT int xmlHashRemoveEntry(xmlHashTablePtr table, const xmlChar *name,
               xmlHashDeallocator f)
{
    return(xmlHashRemoveEntry3(table, name, NULL, NULL, f));
}

/**
 * xmlHashRemoveEntry2:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param f the deallocator function for removed item (if any)
 *
 * Find the userdata specified by the (name, name2) tuple and remove
 * it from the hash table. Existing userdata for this tuple will be removed
 * and freed with f.
 *
 * Returns 0 if the removal succeeded and -1 in case of error or not found.
 */
XMLPUBFUNEXPORT int
xmlHashRemoveEntry2(xmlHashTablePtr table, const xmlChar *name,
            const xmlChar *name2, xmlHashDeallocator f) {
    return(xmlHashRemoveEntry3(table, name, name2, NULL, f));
}

/**
 * xmlHashRemoveEntry3:
 * @param table the hash table
 * @param name the name of the userdata
 * @param name2 a second name of the userdata
 * @param name3 a third name of the userdata
 * @param f the deallocator function for removed item (if any)
 *
 * Find the userdata specified by the (name, name2, name3) tuple and remove
 * it from the hash table. Existing userdata for this tuple will be removed
 * and freed with f.
 *
 * Returns 0 if the removal succeeded and -1 in case of error or not found.
 *
 * OOM:  never / same as argument function 'f' has, if f!=NULL
 */
XMLPUBFUNEXPORT int
xmlHashRemoveEntry3(xmlHashTablePtr table, const xmlChar *name,
    const xmlChar *name2, const xmlChar *name3, xmlHashDeallocator f)
{
    unsigned long key;
    xmlHashEntryPtr entry;
    xmlHashEntryPtr prev = NULL;

    if (table == NULL || name == NULL)
        return(-1);

    key = xmlHashComputeKey(table, name, name2, name3);
    if (table->table[key].valid == 0) {
        return(-1);
    } else {
        for (entry = &(table->table[key]); entry != NULL; entry = entry->next) {
            if (xmlStrEqual(entry->name, name) &&
                    xmlStrEqual(entry->name2, name2) &&
                    xmlStrEqual(entry->name3, name3)) {
                if ((f != NULL) && (entry->payload != NULL))
                    f(entry->payload, entry->name);
                entry->payload = NULL;
                if(entry->name)
                    xmlFree(entry->name);
                if(entry->name2)
                    xmlFree(entry->name2);
                if(entry->name3)
                    xmlFree(entry->name3);
                if(prev) {
                    prev->next = entry->next;
            xmlFree(entry);
        } else {
            if (entry->next == NULL) {
            entry->valid = 0;
            } else {
            entry = entry->next;
            memcpy(&(table->table[key]), entry, sizeof(xmlHashEntry));
            xmlFree(entry);
            }
        }
                table->nbElems--;
                return(0);
            }
            prev = entry;
        }
        return(-1);
    }
}
