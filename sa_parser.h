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

#define SYNTAX_OK 1000
#define SYNTAX_ERROR 1002
#define SEM_ERROR 1003
#define FILE_ERROR 1005

//Funkce syntaktické analýzy výrazù
void udelejZnak();
void udelejPravidla();
void udelejPole();
int sa_parser(int *token, string *atrib, tSymbolTable *table, tListOfInstr *list);
