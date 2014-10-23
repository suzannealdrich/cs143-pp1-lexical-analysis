/*
 * File:  scanner.l
 * ----------------
 * Lex input file to generate the scanner for the compiler.
 * By Suzanne Aldrich for CS143
 *
 * Symbol Table is not freed because there is no 
 * ending wrapper that corresponds to Inityylex()
 */

%{

/* The text within this first region delimited by %{ and %} is assumed to
 * be C/C++ code and will be copied verbatim to the lex.yy.c file ahead
 * of the definitions of the yylex() function. Add other header file inclusions
 * or C++ variable declarations/prototypes that are needed by your code here.
 */
#include "scanner.h"
#include "utility.h" // for PrintDebug()
#include "declaration.h"

#include "hash.h"
HashTable	SymTab;
#define NUM_BUCKETS	25

int commentDepth = 0;  		/* depth of comment nesting */

/*
 * Global variable: yylval
 * -----------------------
 * This global variable is how we get attribute information about the token
 * just scanned to the client. The scanner sets the global variable
 * appropriately and since it's global the client can just read it.  In the
 * future, this variable will be declared for us in the y.tab.c file
 * produced by Yacc, but for now, we declare it manually.
 */
YYSTYPE yylval;

/*
 * Global variable: yylloc
 * -----------------------
 * This global variable is how we get position information about the token
 * just scanned to the client. (Operates similarly to yylval above)
 */
struct yyltype yylloc;


/* Macro: YY_USER_ACTION 
 * ---------------------
 * This flex built-in macro can be defined to provide an action which is
 * always executed prior to any matched rule's action. Basically, it is
 * a way of having a piece of code common to all actions factored out to
 * this routine.  We already defined it for you and left the empty
 * function DoBeforeEachAction ready for your use as needed. It will
 * be called once for each pattern scanned from the file, before
 * executing its action.
 */
static void DoBeforeEachAction(); 
#define YY_USER_ACTION DoBeforeEachAction();


%}

 /*
  * The section before the first %% is the Definitions section of the lex
  * input file. Here is where you set options for the scanner, define lex
  * states, and can set up definitions to give names to regular expressions
  * as a simple substitution mechanism that allows for more readable
  * entries in the Rules section later. 
  */

NEWLINE ("\n")
WHITESPACE ([ \t]+)

LETTER	[a-zA-Z]
DIGIT 	[0-9]
UNDERSCORE ("_")
IDENTIFIER ({LETTER}({LETTER}|{DIGIT}|{UNDERSCORE})*)

DECIMALINT ({DIGIT}+)
HEXDIGIT 	({DIGIT}|[a-fA-F])
HEXINT (0("X"|"x"){HEXDIGIT}+)

DECIMAL (".")
EXPONENT (("E"|"e")("+"|"-"){DIGIT}+)
DOUBLECONSTANT (({DIGIT}+{DECIMAL}{DIGIT}*){EXPONENT}?)

BADSTRINGCONSTANT (["][^"\n]*)
STRINGCONSTANT (["][^"\n]*["])

ONECHAR ([+\-\*/%\\<>=;,.![\](){}])

SINGLELINECOMMENT ("//".*)$

%x COMMENT

%%
 /*
  * All patterns and actions should be placed between the start and stop
  * %% markers which delimit the Rules section. Be careful to avoid
  * unintended whitespace in your patterns. Enclose each action in curly
  * braces (although not technically needed for single-line action, better
  * safe than sorry!). Comments are allowed with the braces delimiting
  * an action or on lines by themselves if the line begins with whitespace.
  */ 


{NEWLINE}|{WHITESPACE} { /* eat my space */; }
{SINGLELINECOMMENT} { /* eat my space */; }

"/*" {
		BEGIN(COMMENT);
		commentDepth = 1;
}

<COMMENT>"*/"		{ if (!--commentDepth) BEGIN(INITIAL); }
<COMMENT>(.|\n)		{ /* eat my comment */; }
<COMMENT><<EOF>>	{ ReportError(&yylloc, "Input ends with unterminated comment"); 
					  return 0; }
<COMMENT>"/*"		{ commentDepth++; }

"void" { return T_Void;}
"int" { return T_Int;}
"double" { return T_Double;}
"bool" { return T_Bool;}
"string" { return T_String;}
"class" { return T_Class;}
"extends" { return T_Extends;}
"this" { return T_This;}
"null" { return T_Null;}
"while" { return T_While;}
"if" { return T_If;}
"else" { return T_Else;}
"return" { return T_Return;}
"public" { return T_Public;}
"private" { return T_Private;}
"New" { return T_New;}
"NewArray" { return T_NewArray;}
"Print" { return T_Print;}
"ReadInteger" { return T_ReadInteger;}
"ReadLine" { return T_ReadLine;}

{ONECHAR} { return yytext[0]; }

"&&" { return T_And;}
"||" { return T_Or;}
"<=" { return T_LessEqual;}
">=" { return T_GreaterEqual;}
"==" { return T_Equal;}
"!=" { return T_NotEqual;}

{DECIMALINT} { yylval.integerConstant = atol(yytext); return T_IntConstant; }
{HEXINT} { yylval.integerConstant = strtol(yytext, NULL, 16); return T_IntConstant; }

{DOUBLECONSTANT} { yylval.doubleConstant = atof(yytext); return T_DoubleConstant; }

"true" { yylval.boolConstant = true; return T_BoolConstant;}
"false" { yylval.boolConstant = false; return T_BoolConstant;}

{BADSTRINGCONSTANT} { ReportError(&yylloc, "Illegal newline in string constant %s", yytext); }

{STRINGCONSTANT} { 
	yylval.stringConstant = CopyString(yytext); 
	return T_StringConstant; 
}

{IDENTIFIER} {  
	Declaration	*identifier = new Declaration(yytext, yylloc.first_line);
	Declaration *match = (Declaration *)TableLookup(SymTab, identifier);
	if (match != NULL) {
		match->IncrementOccurrences();
		yylval.decl = match;
	} else {
 		TableEnter(SymTab, identifier);
 		yylval.decl = (Declaration *)TableLookup(SymTab, identifier);
	}
	return T_Identifier;
}

. { ReportError(&yylloc, "Unrecognized char: '%c'", yytext[0]); }

%%
/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated lex.yy.c file.
 * This section is where you put definitions of helper functions.
 */

static int DeclHash(const void *elem, int numBuckets)
{
    Declaration   *d = (Declaration *)elem;
 
    return StringHash(d->GetName(), numBuckets);
}

static int DeclCompare(const void *entry1, const void *entry2)
{
    Declaration   *d1 = (Declaration *)entry1, *d2 = (Declaration *)entry2;
    
    return strcmp(d1->GetName(), d2->GetName());
}

static void DeclFree(void *elem)
{
    Declaration   *d = (Declaration *)elem;
    
    if (d != NULL) {
        delete d;
    }
}

/*
 * Function: Inityylex()
 * --------------------
 * This function will be called before any calls to yylex().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the scanner (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yy_flex_debug that controls whether flex prints debugging information
 * about each token and what rule was matched. If set to false, no information
 * is printed. Setting it to true will give you a running trail that might
 * be helpful when debugging your scanner. Please be sure the variable is
 * set to false when submitting your final version.
 */
void Inityylex()
{
    PrintDebug("lex", "Initializing scanner");
    yy_flex_debug = false;

    yylloc.first_line = 1;
    yylloc.first_column = 0;
    yylloc.last_column = 0;
    SymTab = TableNew(sizeof(Declaration), NUM_BUCKETS, DeclHash, DeclCompare, DeclFree);
}



/*
 * Function: DoBeforeEachAction()
 * ------------------------------
 * This function is installed as the YY_USER_ACTION. This is a place
 * to group code common to all actions.
 */
static void DoBeforeEachAction()
{
	if (yytext[0] == '\n') {
		yylloc.first_line = yylloc.first_line + 1;
		yylloc.first_column = yylloc.last_column = 0;
	} else {
		yylloc.first_column = yylloc.last_column + 1;
		yylloc.last_column += strlen(yytext);
	}
}



