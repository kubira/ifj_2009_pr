/*******************************************************************************
*  
*  Implementace prekladace imperativniho jazyka IFJ 09
*  
*  xkubis03 - Radim Kubis
*  xbrene02 - Brenek Roman
*  xblaho01 - Blaho Vojtech
*  xlukas07 - Lukas Radek
*
*******************************************************************************/

// Hlavicka pro lexikalni analyzator
// Chybove hlasky
#define LEX_ERROR 1001
// Identifikator
#define ID 0

// Operatory
// Nasledne hodnoty byly zvoleny schvalne.
// Jsou nasledne pouzity v tabulce priorit jako indexy
#define PLUS 1      // '+'
#define MINUS 2     // '-'
#define MUL 3       // '*'
#define DIV 4       // '/'

#define LEFT_PARENTHESIS 5  // '('
#define RIGHT_PARENTHESIS 6 // ')'

#define FUNCTION 7

#define STROKE 8 // ','

#define LESSTHAN 9   // '<'
#define GREATTHAN 10 // '>'
#define LTOREQUAL 11 // '<='
#define GTOREQUAL 12 // '>='

#define EQUAL 13     // '='
#define NOTEQUAL 14  // '<>'

#define ASSIGN 15    // ':='

// Ostatni
#define DOLLAR 16 // $


// Klicova slova
#define BEGIN 600
#define KDIV 601
#define DO 602
#define KDOUBLE 603
#define ELSE 604
#define END 605
#define IF 606
#define INTEGER 607
#define STRING 608
#define THEN 609
#define VAR 610
#define WHILE 611

#define FIND 112
#define SORT 113
#define READLN 114
#define WRITE 115

// Jednotlive znaky
#define LEFT_VINCULUM 400     // '{'
#define RIGHT_VINCULUM 401    // '}'
#define SEMICOLON 402         // ';'
#define DOT 403               // '.'
#define EXP_E 404             // 'E'
#define EXP_e 405             // 'e'
#define RET_DDOT 406          // ':'

// Specialni znaky
#define END_OF_FILE 500 // 'EOF'

// Specialni pripady
#define RET_STRING 100   // String
#define RET_INT    101   // Int
#define RET_DOUBLE 102   // Desetinne cislo

//------------------------------------------------------------------------------

// Hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
int getNextToken(string *attr);
int nahrazeni(string *s1);
