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

#include <stdio.h>
#include <stdlib.h>
#include "ilist.h"

//inicializace seznamu instrukci
void listInit(tListOfInstr *L)
{
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
}

//funkce dealokuje seznam instrukci  
void listFree(tListOfInstr *L)
{
  tListItem *ptr;
  while (L->first != NULL)
  {
    ptr = L->first;
    L->first = L->first->nextItem;
    //uvolnime celou polozku
    free(ptr);
  }
}

//vlozi novou instruci na konec seznamu
void listInsertLast(tListOfInstr *L, tInstr I)
{
  tListItem *newItem;
  newItem = malloc(sizeof (tListItem));
  newItem->Instruction = I;
  newItem->nextItem = NULL;
  if (L->first == NULL)
     L->first = newItem;
  else
     L->last->nextItem=newItem;
  L->last=newItem;
}

//zaktivuje prvni instrukci
void listFirst(tListOfInstr *L)
{
  L->active = L->first;
}

//aktivni instrukci se stane nasledujici instrukce
void listNext(tListOfInstr *L)
{
  if (L->active != NULL)
  L->active = L->active->nextItem;
}

//nastavime aktivni instrukci podle zadaneho ukazatele
void listGoto(tListOfInstr *L, void *gotoInstr)
{
  L->active = (tListItem*) gotoInstr;
}

// vrati ukazatel na posledni instrukci
void *listGetPointerLast(tListOfInstr *L)
{
  return (void*) L->last;
}

//vrati aktivni instrukci
tInstr *listGetData(tListOfInstr *L)
{
  if (L->active == NULL)
  {
    return NULL;
  }
  else return &(L->active->Instruction);
}

//Výpis seznamu instrukcí
void listPrint(tListOfInstr *L) {
  tListItem *item;   
    
  if (L->first != NULL) { 
    item = L->first;
  
    while (item != NULL) {
      printf("\nAdresa instrukce: %d\n", (int)item);
      printf("Instrukce: %d\n", item->Instruction.instType);
      printf("Adresa: %d\n", (int)item->Instruction.addr1);
      printf("Adresa: %d\n", (int)item->Instruction.addr2);
      printf("Adresa: %d\n", (int)item->Instruction.addr3);
      
      item = item->nextItem;
    }
  }
}
