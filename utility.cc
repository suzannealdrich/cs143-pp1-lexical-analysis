/*
 * File: utiliy.cc
 * ---------------
 * Some of this code is based on code from Eric Roberts' cslib libraries.
 */

#include "utility.h"
#include <stdarg.h>
#include <string.h>


#define BufferSize   2056


void ReportError(struct yyltype *pos, char *format, ...)
{
  va_list args;
  char errbuf[BufferSize];
  
  va_start(args, format);
  vsprintf(errbuf,format, args);
  va_end(args);
  if (strlen(errbuf) > BufferSize) {
    Failure("Error message too long\n");
  } else {
    fflush(stdout); // make sure any buffered text has been output
    if (pos)
	fprintf(stderr,"\n*** Error line %d column %d\n", pos->first_line, pos->first_column);
    else
	fprintf(stderr,"\n*** Error at unidentified position\n");
   fprintf(stderr,"*** %s\n\n", errbuf);
  }
}


// Map standard yacc error function to ours
void yyerror(char *msg)
{
  ReportError(&yylloc, "%s", msg);
}

void Failure(const char *format, ...)
{
  va_list args;
  char errbuf[BufferSize];
  
  va_start(args, format);
  vsprintf(errbuf, format, args);
  va_end(args);
  fflush(stdout);
  if (strlen(errbuf) > BufferSize) 
    fprintf(stderr, "\n*** Failure: Failure message too long\n\n");
  else 
    fprintf(stderr,"\n*** Failure: %s\n\n", errbuf);
  exit(1);
}



#define MaxDebugKeys 256

static char *gDebugKeys[MaxDebugKeys];
static int gNumDebugKeys = 0;

static bool KeyInList(const char *key)
{
  int i;
  for (i = 0; i < gNumDebugKeys; i++) 
    if (strcmp(key, gDebugKeys[i]) == 0)
      return true;
  return false;
}

void DebugOn(const char *key)
{
  if (KeyInList(key))
    return;
  Assert(gNumDebugKeys < MaxDebugKeys - 1);
  gDebugKeys[gNumDebugKeys++] = strdup(key);
}



void PrintDebug(const char *key, const char *format, ...)
{
  va_list args;
  char buf[BufferSize];

  if (!KeyInList(key)) 
    return;
  
  va_start(args, format);
  vsprintf(buf, format, args);
  va_end(args);
  if (strlen(buf) > BufferSize) 
    Failure("PrintDebug message too long\n");
  else
    printf("+++ Debug(%s): %s\n", key, buf);
}


// CopyString by Suzanne Aldrich for CS106X
char *CopyString(const char *s)
{
	char	*copy = (char *) malloc(strlen(s) + 1);
	
	strcpy(copy, s);
	return copy;
}


// StringHash adapted from Eric Roberts' _The Art and Science of C_
int StringHash(const char *s, int numBuckets)
{
	int				i;
	unsigned long	hashcode = 0;
	const long		MULTIPLIER = -1664117991;
	
	for (i = 0; i < strlen(s); i++)
		hashcode = hashcode * MULTIPLIER + s[i];
	return (hashcode % numBuckets);
}