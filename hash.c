// hash.c by Suzanne Aldrich
// borrowed for CS143 from hashtable.c and darray.c for CS107

#include <assert.h>
#include <strings.h> 
#include <stdlib.h>
#include <string.h>
#include "hash.h"

#define DEFAULT_ELEMS 2

struct HashImplementation {
    DArray              *buckets;
    int                 nBuckets;       // number of buckets
    int                 elemSize;       // byte size of each element
    TableHashFn         hashFn;         // client element hash function
    TableCompareFn      compFn;         // client element compare function
    TableElementFreeFn  freeFn;         // client element clean up function
};


HashTable TableNew(int elemSize, int nBuckets, TableHashFn hashFn, TableCompareFn compFn, TableElementFreeFn freeFn)
{
    HashTable       table;
    DArray          *buckets;
    int             n;
    
    assert(elemSize > 0);
    assert(nBuckets > 0);
    assert(hashFn != NULL);
    assert(compFn != NULL);
    
    // allocate memory for table structure and its buckets
    
    table = (HashTable) malloc(sizeof(struct HashImplementation));
    assert(table != NULL);
    buckets = (DArray *) malloc(nBuckets * sizeof(DArray *));
    assert(buckets != NULL);
    for (n = 0; n < nBuckets; n++)
        buckets[n] = NULL;
    
    // initialize table structure
    
    table->buckets = buckets;
    table->nBuckets = nBuckets;
    table->elemSize = elemSize;
    table->hashFn = hashFn;
    table->compFn = compFn;
    table->freeFn = freeFn;
    
    return table;   
}


void TableFree(HashTable table)
{
    int     n;
    
    assert(table != NULL);
    assert(table->buckets != NULL);
    
    for (n = table->nBuckets-1; n >= 0; n--)
        if (table->buckets[n] != NULL)
            ArrayFree(table->buckets[n]);
            
    free(table->buckets);
    free(table);
}


int TableCount(HashTable table)
{
    int     n;
    int     count = 0;
    
    assert(table != NULL);
    assert(table->buckets != NULL);
    
    // count the number of entries in each bucket
    
    for (n = 0; n < table->nBuckets; n++)
        if (table->buckets[n] != NULL)
            count += ArrayLength(table->buckets[n]);
        
    return count;
}


void TableEnter(HashTable table, const void *newElem)
{
    int     hashBucket;
    int     bucketElem;
    
    assert(table != NULL);
    
    // determine which bucket to put new element in
    
    hashBucket = (*table->hashFn)(newElem, table->nBuckets);
    
    // if the bucket is empty, make new list for new element
    
    if (table->buckets[hashBucket] == NULL) {
        table->buckets[hashBucket] = ArrayNew(table->elemSize, DEFAULT_ELEMS, table->freeFn);
        assert(table->buckets[hashBucket] != NULL);
    }
    
    // determine which matched element to replace
    // or append unique element to the end of the list
    
    bucketElem = ArraySearch(table->buckets[hashBucket], newElem, table->compFn, 0, false);
    
    if (bucketElem == NOT_FOUND)
        ArrayAppend(table->buckets[hashBucket], newElem);
    else
        ArrayReplaceAt(table->buckets[hashBucket], newElem, bucketElem);
}


void *TableLookup(HashTable table, const void *elemKey)
{
    int     hashBucket;
    int     bucketElem;
    
    assert(table != NULL);
    assert(elemKey != NULL);
    
    
    // determine which bucket to search for key element
    
    hashBucket = (*table->hashFn)(elemKey, table->nBuckets);
    
    // if the bucket is empty, key element is not found
    
   if (table->buckets[hashBucket] == NULL) 
        return NULL;
    
    // find element in bucket which matches key element
    
    bucketElem = ArraySearch(table->buckets[hashBucket], elemKey, table->compFn, 0, false);
    
    if (bucketElem == NOT_FOUND)
        return NULL;
        
    return ArrayNth(table->buckets[hashBucket], bucketElem);
    
}


void TableMap(HashTable table, TableMapFn fn, void *clientData)
{
    int     n;
    
    assert(table != NULL);
    assert(fn != NULL);
    
    // iterate over all buckets and over their lists of elements 
    
    for (n = 0; n < table->nBuckets; n++)
        if (table->buckets[n] != NULL)
            ArrayMap(table->buckets[n], fn, clientData);
}

/* File: darray.c
 * --------------
 * Implementation of the dynamic array interface.
 */



#define DEFAULT_ALLOC 8


struct DArrayImplementation {
    void                *data;
    int                 numElems;       // number of logical elements
    int                 numAlloc;       // number of allocated elements
    int                 elemSize;       // byte size of each element
    int                 growSize;       // byte size of array grow increment
    ArrayElementFreeFn  elemFreeFn;     // client element clean up function
};


DArray ArrayNew(int elemSize, int numElemsToAllocate, ArrayElementFreeFn elemFreeFn)
{
    DArray      array;
    void        *data;
    int         numAlloc;
    int         growSize;

    // calculate initial number of allocated slots 
    // and size of allocation increment 

    assert(numElemsToAllocate >= 0);
    numAlloc = (numElemsToAllocate == 0 ? DEFAULT_ALLOC : numElemsToAllocate);
    assert(elemSize > 0);
    growSize = elemSize * numAlloc;

    // allocate memory for array structure and its data
    
    array = (DArray) malloc(sizeof(struct DArrayImplementation));
    assert(array != NULL);
    data = malloc(growSize);
    assert(data != NULL);
    
    // initialize array structure
    
    array->data = data;
    array->numElems = 0;
    array->numAlloc = numAlloc;
    array->elemSize = elemSize;
    array->growSize = growSize;
    array->elemFreeFn = elemFreeFn;
    
    return array; 
}


static void ArrayElemFree(DArray array, int n)
{
    assert(array != NULL);
    
    // free elements using client-supplied function if needed
    
    if (array->elemFreeFn != NULL)
        (*array->elemFreeFn)(ArrayNth(array, n));
}


void ArrayFree(DArray array)
{
    int     n;
    
    assert(array != NULL);
    assert(array->data != NULL);
    for (n = array->numElems-1; n >= 0; n--) 
        ArrayElemFree(array, n);    
    free(array->data);
    free(array);
}


int ArrayLength(const DArray array)
{
    assert(array != NULL);
    return array->numElems;
}


static bool ArrayInBounds(DArray array, int n)
{
    assert(array != NULL);

    // is n a valid index?

    return (0 <= n && n < array->numElems);
}


void *ArrayNth(DArray array, int n)
{
    assert(array != NULL);
    assert(array->data != NULL);
    assert(ArrayInBounds(array, n));

    // cast data pointer to char for byte math 
    // and calculate element memory position

    return (char *)array->data + n * array->elemSize; 
}


static void ArrayGrow(DArray array)
{
    int         newAllocSize;
    void        *data;

    assert(array != NULL);
    assert(array->data != NULL);
    
    // if the array has filled, reallocate data
    
    if (array->numElems == array->numAlloc) {
    
        // calculate current array size and add grow increment
        
        newAllocSize = array->numAlloc * array->elemSize + array->growSize;
        
        // reallocate array data
        
        data = realloc(array->data, newAllocSize);
        assert(data != NULL);
        
        // reset data location and allocation information
        
        array->data = data;
        array->numAlloc = newAllocSize / array->elemSize;
    }
}


static void ArrayElemWrite(DArray array, const void *newElem, int n)
{
    assert(array != NULL);
    assert(newElem != NULL);
    
    // copy new element into array at n
    
    memcpy(ArrayNth(array, n), newElem, array->elemSize);
}


void ArrayAppend(DArray array, const void *newElem)
{
    assert(array != NULL);
    
    // insert element at the end of the array
    
    ArrayInsertAt(array, newElem, array->numElems);
}


void ArrayInsertAt(DArray array, const void *newElem, int n)
{
    int     tailSize;
    
    assert(array != NULL);
    
    // calculate size of chunk to be shifted to make room for inserted element 
    
    tailSize = (array->numElems - n) * array->elemSize;
    
    // make sure there's enough room and increment logical size of array
    
    ArrayGrow(array);
    array->numElems++;
    
    // if inserting in the middle, shift elements up to make room.
    
    assert(ArrayInBounds(array, n));
    if (tailSize > 0) 
        memmove(ArrayNth(array, n+1), ArrayNth(array, n), tailSize);
        
    ArrayElemWrite(array, newElem, n);
}


void ArrayDeleteAt(DArray array, int n)
{
    int     tailSize;
    
    assert(array != NULL);
    assert(ArrayInBounds(array, n));    
    
    // free contents of element
    
    ArrayElemFree(array, n);
    
    // calculate size of chunk to be shifted to cover deleted element
    
    tailSize = (array->numElems - (n+1)) * array->elemSize;
    
    // if deleting in the middle, shift elements down.
    
    if (tailSize > 0)
        memmove(ArrayNth(array, n), ArrayNth(array, n+1), tailSize);
        
    // decrement logical size of array
        
    array->numElems--;
}


void ArrayReplaceAt(DArray array, const void *newElem, int n)
{
    assert(array != NULL);
    assert(newElem != NULL);
    assert(ArrayInBounds(array, n));
    
    // free contents of old element and cover it with new element
    
    ArrayElemFree(array, n);
    ArrayElemWrite(array, newElem, n);
}


void ArraySort(DArray array, ArrayCompareFn comparator)
{
    assert(array != NULL);
    assert(comparator != NULL);
    
    // sort array!
    
    qsort(array->data, array->numElems, array->elemSize, comparator);
}


int ArraySearch(DArray array, const void *key, ArrayCompareFn comparator, int fromIndex, bool isSorted)
{
    int     n;
    void    *foundPtr = NULL;       
    
    assert(array != NULL);
    assert(key != NULL);
    assert(comparator != NULL);
    
    // find nothing in an empty array
    
    if (fromIndex == array->numElems) return NOT_FOUND; 
    assert(ArrayInBounds(array, fromIndex));    
    
    // find pointer to element which matches key
    
    if(isSorted) {
    
        // binary search sorted array 
        
        foundPtr = bsearch(key, ArrayNth(array, fromIndex), array->numElems - fromIndex, array->elemSize, comparator);    
    } else {
    
        // linear search unsorted array
        
        for(n = fromIndex; n < array->numElems; n++) {
        
            // compare elements using client-supplied function
            
            if ((*comparator)(ArrayNth(array, n), key) == 0) {
                foundPtr = ArrayNth(array, n);
                break;
            }
        }
    }
    
    // if pointer wasn't set, found nothing
    
    if (foundPtr == NULL) return NOT_FOUND;
    
    // cast pointer to char for byte math and calculate array index
    
    return ((char *)foundPtr - (char *)array->data) / array->elemSize;
}


void ArrayMap(DArray array, ArrayMapFn fn, void *clientData)
{
    int     n;
    
    assert(array != NULL);
    assert(fn != NULL);
	
	// iterate over array and apply client-supplied function to each element
	
	for (n = 0; n < array->numElems; n++) (*fn)(ArrayNth(array, n), clientData);
}

