/*
 * File: utility.h
 * ---------------
 * This file just includes a few support functions you might find
 * helpful in writing the projects (error handling, debug printing)
 */

#ifndef _H_utility
#define _H_utility

#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"


/*
 * Function: ReportError()
 * Usage: ReportError(&yylloc, "Unexpected char: %c\n", ch);
 * ---------------------------------------------------------
 * This function is used to report an error to the user.  You can
 * call this function to report any errors encountered during scanning
 * or parsing.  The first argument is the pointer to the position
 * structure that identifies where the problem is. It can be NULL
 * if there is no appropriate position to point out. The second
 * argument is the message to print, it accepts printf-style arguments
 * in the format string.
 */
void ReportError(struct yyltype *pos, char *format, ...);


/*
 * Function: Failure()
 * Usage: Failure("Out of memory!");
 * --------------------------------
 * Reports an error and exits the program immediately.  You should not
 * need to call this since you should always try to continue parsing,
 * even after an error is encountered.  Some of the provided code calls
 * this in unrecoverable error situations (cannot allocate memory, etc.)
 * Failure accepts printf-style arguments in the message to be printed.
 */
void Failure(const char *format, ...);




/*
 * Macro: Assert()
 * Usage: Assert(num > 0);
 * ----------------------
 * This macro is designed to assert the truth of a necessary condition.
 * It tests the given expression, and if it evalutes true, nothing happens.
 * If it is false, it calls Failure to print a message and abort.
 * For example:  Assert(ptr != NULL)
 * will print something similar to the following if ptr is NULL:
 *   *** Failure: Assertion failed: hashtable.cc, line 55:
 *       ptr != NULL
 */ 
#define Assert(expr)  \
  ((expr) ? (void)0 : Failure("Assertion failed: %s, line %d:\n    %s", __FILE__, __LINE__, #expr))




/*
 * Function: PrintDebug()
 * Usage: PrintDebug("parser", "found ident %s\n", ident);
 * -------------------------------------------------------
 * Print a message if we have turned debugging messages on for the given
 * key.  For example, the usage line shown above will only print a message
 * if the call is preceded by a call to  DebugOn("parser").  The function
 * accepts printf arguments.  The provided main.cc parses the command line
 * to turn on debug flags.  See main.cc for more information.
 */
void PrintDebug(const char *key, const char *format, ...);


/* Function: DebugOn()
 * Usage: DebugOn("pp3");
 * ----------------------
 * Turn on debugging messages for the given key.  See PrintDebug
 * for an example. Can be called manually when desired and will
 * be called from the provided main for flags passed with -d.
 */
void DebugOn(const char *key);

char *CopyString(const char *s);

int StringHash(const char *s, int numBuckets);

#endif
