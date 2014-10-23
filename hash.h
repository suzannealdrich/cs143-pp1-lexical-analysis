// hash.h by Suzanne Aldrich
// borrowed for CS143 from hashtable.h and darray.h for CS107

#include <stdio.h>

#ifndef _HASHTABLE_H
#define _HASHTABLE_H


typedef struct HashImplementation *HashTable;

typedef int (*TableHashFn)(const void *elem, int numBuckets);

typedef int (*TableCompareFn)(const void *elem1, const void *elem2);

typedef void (*TableMapFn)(void *elem, void *clientData);

typedef void (*TableElementFreeFn)(void *elem);

HashTable TableNew(int elemSize, int nBuckets, 
                   TableHashFn hashFn, TableCompareFn compFn, 
 					 TableElementFreeFn freeFn);

void TableFree(HashTable table);

int TableCount(HashTable table);

void TableEnter(HashTable table, const void *newElem);

void *TableLookup(HashTable table, const void *elemKey);

void TableMap(HashTable table, TableMapFn fn, void *clientData);


#endif _HASHTABLE_H

#ifndef _DARRAY_H
#define _DARRAY_H


//typedef enum {false, true} bool;

typedef struct DArrayImplementation *DArray;

typedef int (*ArrayCompareFn)(const void *elem1, const void *elem2);

typedef void (*ArrayMapFn)(void *elem, void *clientData);

typedef void (*ArrayElementFreeFn)(void *elem);

DArray ArrayNew(int elemSize, int numElemsToAllocate, 
                ArrayElementFreeFn elemFreeFn);

void ArrayFree(DArray array);

int ArrayLength(const DArray array);

void *ArrayNth(DArray array, int n);

void ArrayAppend(DArray array, const void *newElem);

void ArrayInsertAt(DArray array, const void *newElem, int n);

void ArrayDeleteAt(DArray array, int n);

void ArrayReplaceAt(DArray array, const void *newElem, int n);

void ArraySort(DArray array, ArrayCompareFn comparator);

#define NOT_FOUND -1	// returned when a search fails to find the key

int ArraySearch(DArray array, const void *key, ArrayCompareFn comparator, 
                  int fromIndex, bool isSorted);

void ArrayMap(DArray array, ArrayMapFn fn, void *clientData);

#endif _DARRAY_

