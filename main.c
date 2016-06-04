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
   string at;  // Struktura string pro øetìzec
   int ok;
   tSymbolTable table;
   tListOfInstr instrList;

   if (argc == 1)  // Pokud je poèet pøedaných parametrù roven jedné,
   {               // nebyl zadán parametr jména souboru pro analýzu
      return FILE_ERROR-SYNTAX_OK;  // Návratová hodnota programu je 5
   }
   if ((f = fopen(argv[1], "r")) == NULL)  // Kontrola otevøení souboru
   {  // Pokud se soubor nepodaøilo otevøít
      return FILE_ERROR-SYNTAX_OK;  // a návratová hodnota programu je 5
   }

   strInit(&at);  // Inicializace øetìzce at
   tableInit(&table);  // Inicializace tabulky symbolu
   listInit(&instrList); // Inicializace seznamu instrukci

   setSourceFile(f);  // Nastavení zdrojového souboru pro lexikální analýzu                                           

   ok = parse(&table, &instrList);
   
   //printf(" Main: %d\n", ok-SYNTAX_OK);

   //listPrint(&instrList);

   if (ok == SYNTAX_OK) ok = inter(&table, &instrList);

   //printf(" Inter: %d\n\n", ok-SYNTAX_OK);

   fclose(f);  // Uzavøení analyzovaného souboru
   
   strFree(&at);  // Uvolnìní pamìti po struktuøe s øetìzcem
   listFree(&instrList);
   tableDispose(table.first);

   return ok-SYNTAX_OK;  // Návratová hodnota 0 - program skonèil bez chyby
}
