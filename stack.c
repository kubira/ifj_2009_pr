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

//stack.c
#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "bvs.h"
#include "stack.h"
#include "scaner.h"

#define SYNTAX_ERROR 1002
#define SEM_ERROR 1003

int counterVar = 1;

void generateVariable(string *var)
// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace

{
  int i;

  strClear(var);
  strAddChar(var, '$');
  i = counterVar;
  while (i != 0)
  {
    strAddChar(var, (char)(i % 10 + '0'));
    i = i / 10;
  }
  counterVar ++;
}

char *znaky[17];

void udelejZnaky() {
  znaky[ID]    = "I";
  znaky[PLUS]  = "+";
  znaky[MINUS] = "-";
  znaky[MUL]   = "*";
  znaky[DIV]   = "/";
  znaky[LEFT_PARENTHESIS]  = "(";
  znaky[RIGHT_PARENTHESIS] = ")";
  znaky[7]         = "f";
  znaky[STROKE]    = ",";
  znaky[LESSTHAN]  = "<";
  znaky[GREATTHAN] = ">";
  znaky[LTOREQUAL] = "<=";
  znaky[GTOREQUAL] = ">=";
  znaky[EQUAL]    = "==";
  znaky[NOTEQUAL] = "!=";
  znaky[ASSIGN]   = "=";
  znaky[DOLLAR]   = "$";
}

void stackInit(stack *s) {
  stackItemPtr helper;
  
  helper = malloc(sizeof(struct stackItem));
  
  helper->symbol = DOLLAR;
  helper->term = TRUE;
  helper->nextItem = NULL;
  helper->tablePtr = NULL;
  helper->nextNonTerm = NULL;

  s->first = helper;
  s->top = helper;
  s->firstNonTerm = NULL;
  
  udelejZnaky();
}

int push(stack *s, int symb, string *name, tSymbolTable *table) {
  stackItemPtr helper;  // Pomocná promìnné pro vložení nového prvku
  helper = malloc(sizeof(struct stackItem));  // Alokace nového prvku

  if(symb == FUNCTION) {
	  if(strCmpConstStr(name, "sort") == 0) {
		  helper->function = SORT;
	  } else if(strCmpConstStr(name, "find") == 0) {
		  helper->function = FIND;
	  } else {
		  return SYNTAX_ERROR;
	  }
  }

  helper->symbol = symb;  // Symbol na zásobníku - index ve znacích
  helper->nextItem = s->first;  // Nastavení následujícího prvku
  s->first = helper;  // Nastavení prvního prvku na aktuální

  //if(symb == 'E' || symb == ID) printf("\t\t\tPUSH: %s\n", strGetStr(name));

  if(symb != 'E') {
    helper->term = TRUE;
    s->top = helper;
	helper->nextNonTerm = NULL;
  }
  else {
    helper->term = FALSE;
    helper->nextNonTerm = s->firstNonTerm;
    s->firstNonTerm = helper;
  }
  
  if((symb == 0) || (symb == 'E')) {

	  if((helper->tablePtr = tableSearch(table, name)) == NULL) { // nenasel v tabulce, treba vygenerovat promennou
		  return SEM_ERROR;
	  }

  }

  return 0;
}

int isEmpty(stack *s) {
  if(!s->top) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

stackItemPtr top(stack *s) {
  return s->top;
}

void pop(stack *s, string *name) {
  stackItemPtr helper;
  
  helper = s->first;

  if(helper->symbol == 'E' || helper->symbol == ID) {
	  //printf("POP: %s\n", s->first->tablePtr->key.str);
	  strCopyChar(name, helper->tablePtr->key.str);
  }

  if (helper->symbol == 'E')
    if(s != NULL)
	    if(s->firstNonTerm != NULL)
		    if(s->firstNonTerm->nextNonTerm != NULL)  // CHYBA!!!
			    s->firstNonTerm = s->firstNonTerm->nextNonTerm;
  
  s->first = helper->nextItem;
  
  if(helper == s->top) {
    s->top = s->first;
  }

  free(helper);
}

void stackVypis(stack *s)
{
  stackItemPtr helper;
  
  helper = s->first;

  while(helper) {
    if(helper->symbol == '|') {
      //printf("|");
    }
    else if(helper->symbol != 'E') {
      //printf("%s", znaky[helper->symbol]);
    }
    //else printf("%c", helper->symbol);
    
    helper = helper->nextItem;
  }
  //printf("\n");
}

void stackDispose(stack *s)
{
  stackItemPtr helper;
  
  while(s->first != NULL) {
    helper = s->first;
    s->first = helper->nextItem;
    free(helper);
  }
}
