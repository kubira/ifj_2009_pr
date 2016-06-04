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

#define SYNTAX_OK     1000
#define SYNTAX_ERROR 1002
#define SEM_ERROR    1003
#define MEM_ERROR    1005

// hlavicka pro syntakticky analyzator
int parse(tSymbolTable *ST, tListOfInstr *instrList);
int id_or_main();
int statL();    
int stat(); 
int declrList();
