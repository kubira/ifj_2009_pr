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
#include "str.h"
#include "scaner.h"
#include "bvs.h"
#include "ilist.h"
#include "parser.h"
#include "interpret.h"

#define FILE_ERROR 1005

int main(int argc, char** argv)
{
   FILE *f;  // Ukazatel na soubor
   string at;  // Struktura string pro �et�zec
   int ok;
   tSymbolTable table;
   tListOfInstr instrList;

   if (argc == 1)  // Pokud je po�et p�edan�ch parametr� roven jedn�,
   {               // nebyl zad�n parametr jm�na souboru pro anal�zu
      return FILE_ERROR-SYNTAX_OK;  // N�vratov� hodnota programu je 5
   }
   if ((f = fopen(argv[1], "r")) == NULL)  // Kontrola otev�en� souboru
   {  // Pokud se soubor nepoda�ilo otev��t
      return FILE_ERROR-SYNTAX_OK;  // a n�vratov� hodnota programu je 5
   }

   strInit(&at);  // Inicializace �et�zce at
   tableInit(&table);  // Inicializace tabulky symbolu
   listInit(&instrList); // Inicializace seznamu instrukci

   setSourceFile(f);  // Nastaven� zdrojov�ho souboru pro lexik�ln� anal�zu                                           

   ok = parse(&table, &instrList);
   
   //printf(" Main: %d\n", ok-SYNTAX_OK);

   //listPrint(&instrList);

   if (ok == SYNTAX_OK) ok = inter(&table, &instrList);

   //printf(" Inter: %d\n\n", ok-SYNTAX_OK);

   fclose(f);  // Uzav�en� analyzovan�ho souboru
   
   strFree(&at);  // Uvoln�n� pam�ti po struktu�e s �et�zcem
   listFree(&instrList);
   tableDispose(table.first);

   return ok-SYNTAX_OK;  // N�vratov� hodnota 0 - program skon�il bez chyby
}
