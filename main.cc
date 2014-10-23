/* File: main.cc
 * -------------
 * This file defines the main() routine for the program and not much else.
 * You should not need to modify this file.
 */
 
#include "scanner.h"
#include "utility.h"
#include "declaration.h"
#include <stdio.h>


static void ParseCommandLine(int argc, char *argv[]);



/*
 * Function: PrintOneToken()
 * Usage: PrintOneToken(T_Double, "3.5", val, loc);
 * -----------------------------------------------
 * We supply this function to print information about the tokens returned
 * by the lexer as part of pp1.  Do not modifiy it.
 */
static void PrintOneToken(TokenType token, char *text, YYSTYPE value, yyltype loc)
{
  char buffer[] = {'\'', token, '\'', '\0'};
  const char *name = token >= T_Void ? gTokenNames[token - T_Void] : buffer;
  
  printf("%-12s line %d cols %d-%d is %s ", text,
	   loc.first_line, loc.first_column, loc.last_column, name);
  
  switch(token) {
    case T_IntConstant:     
      printf("(value = %d)\n", value.integerConstant); break;
    case T_DoubleConstant:   
      printf("(value = %g)\n", value.doubleConstant); break;
    case T_StringConstant:  
      printf("(value = %s)\n", value.stringConstant); break;
    case T_BoolConstant:    
      printf("(value = %s)\n", value.boolConstant ? "true" : "false"); break;
    case T_Identifier: 
      value.decl->Print(); break;
    default:
      printf("\n"); break;
  }
}


/*
 * Function: main()
 * ----------------
 * Entry point to the entire program.  We parse the command line and turn
 * on any debugging flags requested by the user when invoking the program.
 * Call Inityylex() to set up the scanner, and then loop, using yylex()
 * to get each token, print out its information, and continue until the
 * entire input has been scanned.
 */
int main(int argc, char *argv[])
{
  TokenType token;

  ParseCommandLine(argc, argv);

  Inityylex();
  while ((token = (TokenType)yylex()) != 0) 
      PrintOneToken(token, yytext, yylval, yylloc);
  return 0;
}




/*
 * Function: ParseCommandLine
 * --------------------------
 * Turn on the debugging flags from the command line.  Verifies that
 * first argument is -d, and then interpret all the arguments that follow
 * as being flags to turn on.
 */
static void ParseCommandLine(int argc, char *argv[])
{
  int i;
  
  if (argc == 1) 
    return;
  
  if (strcmp(argv[1], "-d") != 0) { // first arg is not -d
    printf("Usage:   -d <debug-key-1> <debug-key-2> ... \n");
    exit(2);
  }

  for (i = 2; i < argc; i++) 
    DebugOn(argv[i]);
}
