/* File: declaration.cc
 * --------------------
 * Implementation of Declaration class.
 *
 * pp1:  You need to fill in the class implementation and define
 *       additional member data/functions as needed. See declaration.h
 *       for comments.
 */

#include "declaration.h"
#include "utility.h"

Declaration::Declaration(char *ident, int lineFound)
{
    name = CopyString(ident);
    firstLine = lineFound;
    numOccurrences = 1;
}

Declaration::~Declaration()
{
	free(name);
}

void Declaration::IncrementOccurrences()
{
	numOccurrences = numOccurrences + 1;
}

void Declaration::Print()
{
	printf("(%s seen %d time(s), first on line %d)\n", name, numOccurrences, firstLine);
}

char * Declaration::GetName()
{
	return name;
}


