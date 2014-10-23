/*
 * File: declaration.h
 * -------------------
 * The Declaration class defines a simple object that is used to
 * track usage of identifiers (such as variables, function names,
 * etc.) within a program.
 */

#ifndef _H_declaration
#define _H_declaration

/*
 * pp1: Declarations at this point are only used to make sure
 * your scanner is working correctly.  You need to implement the interface
 * that is sketched here, as well as add any other member data/functions
 * that you find necessary.
 */
  
class Declaration
{
  private:
    char *name;
    int firstLine, numOccurrences;

 public:

    /* Pass in the identifier's name and the line where it
     * first occurs to construct a new declaration object */
    Declaration(char *name, int lineFound);

	~Declaration();

    /* call this whenever the same identifier is seen */
    void IncrementOccurrences();

    /* called from pp1's main. The method should print the
     * identifier name, occurrence count, and first line seen like this:
     * (myVariable seen 5 time(s), first on line 114) */
    void Print();
    
    char *GetName();
    
};
 
#endif
