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
#include "bvs.h"
#include "ilist.h"

#define INTER_ERROR 1004
#define SEM_ERROR 1003
#define SYNTAX_OK 1000

char c;
int debug_mess=0;
int pomocna = 0;

int inter(tSymbolTable *ST, tListOfInstr *instrList)
{
  tInstr *I;
  listFirst(instrList);

  //listPrint(instrList);

  while (1)
  {
    I = listGetData(instrList); //ziskani nove instrukce

  	if(I != NULL) {
      //podle nazvu instrukce se switch rozhodne, co za insttukci se ma provest
      switch (I->instType) {
        case I_READ:
          // instrukce pro nacteni hodnoty ze std. vstupu
          switch((((tTableItem*)I->addr1)->data)->varType){
            case TYPE_INTEGER:   //nactu Int
              if (debug_mess == 1) {
                printf("instrukce: I_READ\n");
                printf("int: ");
              }
              //otestovani, zda-li vstup neskoncil predcasne EOF
              pomocna = scanf("%d", &((((tTableItem*)I->addr1)->data)->value.integerNumber));
              if (pomocna <= 0 || pomocna == EOF) return INTER_ERROR;
    
              break;
    
            case TYPE_DOUBLE:   //nactu Double
              if (debug_mess == 1) {
                printf("instrukce: I_READ\n");
                printf("double: ");
              }
              //otestovani, zda-li vstup neskoncil predcasne EOF
              pomocna = scanf("%le", &((((tTableItem*)I->addr1)->data)->value.decimalNumber));
              if (pomocna <= 0 || pomocna == EOF) return INTER_ERROR;
              break;
    
            case TYPE_STRING:   //nactu String
              if (debug_mess == 1) {
                printf("instrukce: I_READ\n");
                printf("string: ");
              }
              //inicializace noveho retezce
              strInit(&((((tTableItem*)I->addr1)->data)->value.stringValue));
              //cyklus pro nacitani znaku do noveho retezce
              while((c = getchar()) != '\n') {
                if (c == EOF) return INTER_ERROR;
                strAddChar(&((((tTableItem*)I->addr1)->data)->value.stringValue), c);
              }
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_WRITE:
          // instrukce pro vypis hodnoty na std. vystup
          if (debug_mess == 1) {
            printf("instrukce: I_WRITE\n");
          }
    
          switch((((tTableItem*)I->addr1)->data)->varType){
            case TYPE_INTEGER:   //vypisu Int
              fprintf(stdout, "%d", (((tTableItem*)I->addr1)->data)->value.integerNumber);
              break;
    
            case TYPE_DOUBLE:   //vypisu Double
              fprintf(stdout, "%g", (((tTableItem*)I->addr1)->data)->value.decimalNumber);
              break;
    
            case TYPE_STRING:   //vypisu String
              fprintf(stdout, "%s", strGetStr(&((((tTableItem*)I->addr1)->data)->value.stringValue)));
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_FIND:
          // instrukce pro vyhledani podretezce v retezci
          // instrukce ulozi na treti adresu pozici na ktere nalezla hledany
          // hledany podretezec, nebo 0 v pripade neuspechu
            (((tTableItem*)I->addr3)->data)->value.integerNumber = find(&((((tTableItem*)I->addr1)->data)->value.stringValue), &((((tTableItem*)I->addr2)->data)->value.stringValue));
          break;
//------------------------------------------------------------------------------
        case I_SORT:
          // instrukce pro serazeni znaku v retezci
          // pred vlastnim razeni si instrukce vytvori novy retezec, ktery bude
          // radit a puvodni necha nezmeneny
          strCopyString(&((((tTableItem*)I->addr3)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue));
          mergessort(&((((tTableItem*)I->addr3)->data)->value.stringValue), 0, (strGetLength(&((((tTableItem*)I->addr3)->data)->value.stringValue)))-1);
          break;
//------------------------------------------------------------------------------
        case I_GOTO:
          // instrukce skoku
          listGoto(instrList, I->addr3);
          break;
//------------------------------------------------------------------------------
        case I_IF_NOT:
          // rozhodovaci instrukce, pomoci ktere se provadi podminka IF a cyklus WHILE
          if (debug_mess == 1) {
            printf("IFNOT: %d\n", (((tTableItem*)I->addr1)->data)->value.integerNumber);
          }
          //pokud se hodnota na prvni adrese nerovna nule, nikam se neskace 
          if ((((tTableItem*)I->addr1)->data)->value.integerNumber != 0) {
            if (debug_mess == 1) {
              printf("Skacu na dalsi instrukci\n");
            }
          }
          else {
            if (debug_mess == 1) {
              printf("Skacu na instrukci %d\n", (int)I->addr3);
            }
            listGoto(instrList, I->addr3);
            continue;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_PLUS:
          // instrukce scitani +
          // podle datoveho typu se rozhodnu, co budu scitat
          switch((((tTableItem*)I->addr3)->data)->varType){
            case TYPE_INTEGER:   // sectu Int
              // pokud ma byt vysledek INT musi se scitat dva integery
              ((((tTableItem*)I->addr3)->data)->value.integerNumber) = ((((tTableItem*)I->addr2)->data)->value.integerNumber) + ((((tTableItem*)I->addr1)->data)->value.integerNumber);
    
              if (debug_mess == 1) {
                 printf("VYSLEDEK PLUS: %d\n", (((tTableItem*)I->addr3)->data)->value.integerNumber);
              }
              break;
    
            case TYPE_DOUBLE:   // sectu Double
              // vysledek double muze nastat pokud se scitaji dva doubly, nebo
              // pokud se scita jeden double a jeden integer
              if(((((tTableItem*)I->addr2)->data)->varType == TYPE_DOUBLE) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_DOUBLE))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.decimalNumber) + ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              }
              else if(((((tTableItem*)I->addr2)->data)->varType == TYPE_INTEGER) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_DOUBLE))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.integerNumber) + ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              }
              else if(((((tTableItem*)I->addr2)->data)->varType == TYPE_DOUBLE) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_INTEGER))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.decimalNumber) + ((((tTableItem*)I->addr1)->data)->value.integerNumber);
              }
              break;
    
            case TYPE_STRING:   // provedu konkatenaci
              // instrukce provede spojeni dvou retezcu
              if(((((tTableItem*)I->addr2)->data)->varType == TYPE_STRING) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_STRING))) {
                concateStrings(&((((tTableItem*)I->addr2)->data)->value.stringValue), &(((((tTableItem*)I->addr1)->data)->value.stringValue)), &((((tTableItem*)I->addr3)->data)->value.stringValue));
              }
              break;
    
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_MINUS:
          //instrukce odcitani -
          switch((((tTableItem*)I->addr3)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:    // odectu Int
              // pokud ma byt vysledek INT musi se odecitat dva integery
              ((((tTableItem*)I->addr3)->data)->value.integerNumber) = ((((tTableItem*)I->addr2)->data)->value.integerNumber) - ((((tTableItem*)I->addr1)->data)->value.integerNumber);
              if (debug_mess == 1) {
                 printf("\nVYSLEDEK MINUS: %d\n", (((tTableItem*)I->addr3)->data)->value.integerNumber);
              }
              break;
    
            case TYPE_DOUBLE: //sectu Double
              // vysledek double muze nastat pokud se odecitaji dva doubly, nebo
              // pokud se odecita jeden double a jeden integer
              if(((((tTableItem*)I->addr2)->data)->varType == TYPE_DOUBLE) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_DOUBLE))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.decimalNumber) - ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              }
              else if(((((tTableItem*)I->addr2)->data)->varType == TYPE_INTEGER) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_DOUBLE))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = (double)((((tTableItem*)I->addr2)->data)->value.integerNumber) - ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              }
              else if(((((tTableItem*)I->addr2)->data)->varType == TYPE_DOUBLE) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_INTEGER))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.decimalNumber) - (double)((((tTableItem*)I->addr1)->data)->value.integerNumber);
              }
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_MUL:
          //instrukce nasobeni *
          switch((((tTableItem*)I->addr3)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // vynasobim Int
              // pokud ma byt vysledek INT musi se nasobit dva integery
              ((((tTableItem*)I->addr3)->data)->value.integerNumber) = ((((tTableItem*)I->addr2)->data)->value.integerNumber) * ((((tTableItem*)I->addr1)->data)->value.integerNumber);
    
              break;
    
            case TYPE_DOUBLE:   // sectu Double
              // vysledek double muze nastat pokud se budou nasobit dva doubly, nebo
              // pokud se nasobi jeden double a jeden integer
              if(((((tTableItem*)I->addr2)->data)->varType == TYPE_DOUBLE) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_DOUBLE))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.decimalNumber) * ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              }
              else if(((((tTableItem*)I->addr2)->data)->varType == TYPE_INTEGER) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_DOUBLE))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = (double)((((tTableItem*)I->addr2)->data)->value.integerNumber) * ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              }
              else if(((((tTableItem*)I->addr2)->data)->varType == TYPE_DOUBLE) && (((((tTableItem*)I->addr1)->data)->varType == TYPE_INTEGER))) {
                ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr2)->data)->value.decimalNumber) * (double)((((tTableItem*)I->addr1)->data)->value.integerNumber);
              }
    
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_DIV:
          //instrukce deleni div
          //delit se muzou pouze dva integery - jina moznost neni pristupna
          if (((((tTableItem*)I->addr1)->data)->varType) == TYPE_INTEGER && ((((tTableItem*)I->addr2)->data)->varType) == TYPE_INTEGER && ((((tTableItem*)I->addr3)->data)->varType) == TYPE_INTEGER)
            {if (((((tTableItem*)I->addr1)->data)->value.integerNumber) == 0) return INTER_ERROR;
              ((((tTableItem*)I->addr3)->data)->value.integerNumber) = ((((tTableItem*)I->addr2)->data)->value.integerNumber) / ((((tTableItem*)I->addr1)->data)->value.integerNumber);
            }
          else return SEM_ERROR;
    
          break;
//------------------------------------------------------------------------------
        case I_EXPR_EXPR:
          //instrukce prirazeni :=
          //vzdy muzu prirazovat jen stejny typ
          switch((((tTableItem*)I->addr3)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // priradim Int
              ((((tTableItem*)I->addr3)->data)->value.integerNumber) = ((((tTableItem*)I->addr1)->data)->value.integerNumber);
              break;
    
            case TYPE_DOUBLE:   // priradim Double
              ((((tTableItem*)I->addr3)->data)->value.decimalNumber) = ((((tTableItem*)I->addr1)->data)->value.decimalNumber);
              break;
    	      case TYPE_STRING:   // priradim Double
              strCopyString(&((((tTableItem*)I->addr3)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue));
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_LESS:
          //instrukce mensi <
          //vzdy muzu porovnavat jen stejny typ
          switch((((tTableItem*)I->addr1)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   //  porovnam Int
              if ((((tTableItem*)I->addr2)->data)->value.integerNumber < (((tTableItem*)I->addr1)->data)->value.integerNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_DOUBLE:   // porovnam Double
              if ((((tTableItem*)I->addr2)->data)->value.decimalNumber < (((tTableItem*)I->addr1)->data)->value.decimalNumber) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
    
            case TYPE_STRING:   // porovnam String
              if (strCmpString(&((((tTableItem*)I->addr2)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue)) < 0) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
          }
    	  //listPrint(instrList);
    	  //vypis(ST->first);
          break;
//------------------------------------------------------------------------------
        case I_EXPR_ABOVE:
          //instrukce vetsi >
          //vzdy muzu porovnavat jen stejny typ
          switch((((tTableItem*)I->addr1)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // porovnam Int
              if ((((tTableItem*)I->addr2)->data)->value.integerNumber > (((tTableItem*)I->addr1)->data)->value.integerNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_DOUBLE:   // porovnam Double
              if ((((tTableItem*)I->addr2)->data)->value.decimalNumber > (((tTableItem*)I->addr1)->data)->value.decimalNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_STRING:   // porovnam String
              if (strCmpString(&((((tTableItem*)I->addr2)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue)) > 0) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
    
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_LESSEQUAL:
          //instrukce mensi-rovno <=
          //vzdy muzu porovnavat jen stejny typ
          switch((((tTableItem*)I->addr1)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // porovnam Int
              if ((((tTableItem*)I->addr2)->data)->value.integerNumber <= (((tTableItem*)I->addr1)->data)->value.integerNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_DOUBLE:   // porovnam Double
              if ((((tTableItem*)I->addr2)->data)->value.decimalNumber <= (((tTableItem*)I->addr1)->data)->value.decimalNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_STRING:   // porovnam String
              if (strCmpString(&((((tTableItem*)I->addr2)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue)) <= 0) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_ABOVEEQUAL:
          //instrukce vetsi-rovno >=
          //vzdy muzu porovnavat jen stejny typ
          switch((((tTableItem*)I->addr1)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // porovnam Int
              if ((((tTableItem*)I->addr2)->data)->value.integerNumber >= (((tTableItem*)I->addr1)->data)->value.integerNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_DOUBLE:   // porovnam Double
              if ((((tTableItem*)I->addr2)->data)->value.decimalNumber >= (((tTableItem*)I->addr1)->data)->value.decimalNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_STRING:   // porovnam String
              if (strCmpString(&((((tTableItem*)I->addr2)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue)) >= 0) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_EQUAL:
          //instrukce rovno =
          //vzdy muzu porovnavat jen stejny typ
          switch((((tTableItem*)I->addr1)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // porovnam Int
              if ((((tTableItem*)I->addr2)->data)->value.integerNumber == (((tTableItem*)I->addr1)->data)->value.integerNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_DOUBLE:   // porovnam Double
              if ((((tTableItem*)I->addr2)->data)->value.decimalNumber == (((tTableItem*)I->addr1)->data)->value.decimalNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_STRING:   // porovnam String
              if (strCmpString(&((((tTableItem*)I->addr2)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue)) == 0) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_EXPR_NOEQUAL:
          //instrukce nerovno <>
          //vzdy muzu porovnavat jen stejny typ
          switch((((tTableItem*)I->addr1)->data)->varType){ //podle datoveho typu
            case TYPE_INTEGER:   // porovnam Int
              if ((((tTableItem*)I->addr2)->data)->value.integerNumber != (((tTableItem*)I->addr1)->data)->value.integerNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
    
            case TYPE_DOUBLE:   // porovnam Double
              if ((((tTableItem*)I->addr2)->data)->value.decimalNumber != (((tTableItem*)I->addr1)->data)->value.decimalNumber)
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              else
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              break;
  
            case TYPE_STRING:   // porovnam String
              if (strCmpString(&((((tTableItem*)I->addr2)->data)->value.stringValue), &((((tTableItem*)I->addr1)->data)->value.stringValue)) != 0) {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = TRUE;
              }
              else {
                (((tTableItem*)I->addr3)->data)->value.integerNumber = FALSE;
              }
              break;
          }
          break;
//------------------------------------------------------------------------------
        case I_START:
          // instrukce konce programu
          break;
//------------------------------------------------------------------------------
        case I_STOP:
          // instrukce konce programu
          break;
      }
  
    }	else break;
    listNext(instrList);
  }
  //pokud vse probehlo v poradku, vrati se o tom spravne hlaseni
  return SYNTAX_OK;
}
