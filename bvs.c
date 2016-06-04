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
#include <limits.h>
#include <math.h>
#include "str.h"
#include "bvs.h"

#define SYNTAX_OK 1000
#define LEX_ERROR 1001

//Funkce pro inicializaci tabulky symbolù
//Parametry:
//  T - ukazatel na tabulku symbolù
//Funkce nic nevrací - je typu void
void tableInit(tSymbolTable *T) {
  T->first = NULL;  //Inicializuje tak, ¾e nastaví první prvek tabulky na NULL
}
//KONEC funkce tableInit

//Funkce, která vkládá nový symbol (identifikátor) do tabulky symbolù
//Parametry:
//  T - ukazatel na tabulku symbolù
//  key - klíè symbolu (název identifikátoru)
//  varType - datový typ symbolu (identifikátoru)
//Návratová hodnota funkce:
//  0 - v¹e je OK
//  1 - nepodaøilo se vlo¾it prvek, proto¾e není pamì»
//  2 - prvek s vkládaným klíèem ji¾ v tabulce je - SEM_ERROR!
int tableInsert(tSymbolTable *T, string *key, int varType) {
  int ok=0, myend=0, compare;  //Pomocné promìnné
  tTableItem *fptr;  //Pomocný ukazatel na otcovský uzel
  tTableItem *newItem;  //Ukazatel na novì vkládaný symbol (identifikátor)
  tData *data;  //Pomocný ukazatel na union novì vkládaného symbolu

  data = malloc(sizeof(tData));  //Alokuji pamì» unionu

  switch(varType) {  //Podle datového typu symbolu nastavím union
    case TYPE_INTEGER : {  //Kdy¾ je to integer,
      data->value.integerNumber = 0;  //nastavím v unionu integerNumber na 0
      break;  //Musím ukonèit vìtev!
    }
    case TYPE_DOUBLE : {  //Kdy¾ je to double,
      data->value.decimalNumber = 0;  //nastavím v unionu decimalNumber na 0
      break;  //Musím ukonèit vìtev
    }
    case TYPE_STRING : {  //Kdy¾ je to string,
      strInit(&(data->value.stringValue));  //inicializuji v unionu stringValue
      break;  //Tady vìtev ukonèit nemusím, ale udìlám to :P
    }
  }

  if((newItem = (tTableItem*)malloc(sizeof(struct tableItem)))) {
    //Alokuji pamì» novému prvku tabulky a pokud se alokace podaøila,
    newItem->leftItem  = NULL;  //nastavím levého potomka na NULL,
    newItem->rightItem = NULL;  //nastavím pravého potomka na NULL,
    newItem->data = data;  //nastavím ukazatel na union na union vytvoøený
    strInit(&(newItem->key));  //a inicializuji string prvku pro klíè

    ok = strCopyString(&(newItem->key), key);  //Zkopíruji klíè do klíèe prvku

    newItem->data->varType = varType;  //Nastavím datový typ unionu v prvku
  }
  else {  //Pokud se nepodaøilo alokovat pamì»,
    printf("Nepodaøilo se alokovat pamì»");  //vypí¹i hlá¹ení na obrazovku
    return 1;  //a vracím 0 - nepodaøilo se vlo¾it prvek
  }

  if(T->first == NULL) {  //Pokud je tabulka prázdná (první prvek je NULL),
    T->first = newItem;  //stane se nový prvek automaticky prvním prvkem tabulky
    myend = 1;  //Nastavím pomocnou promìnnou pro konec cyklu na 1
  }
  else {  //Pokud není tabulka prázdná,
    fptr = T->first; //tak musím tabulku procházet pomocí ukazatele fptr

    while(!myend) {  //Procházím tabulku, dokud není myend nastaven na 1
      compare = strCmpString((&fptr->key), key);  //Porovnám,
      //jestli u¾ nejsem na prvku s klíèem, který vkládám

      if(compare == 0) {  //Pokud vkládaná hodnota v tabulce u¾ je,
        return 2;  //nastala sémantická chyba!!! SEM_ERROR <--------------------------->
      }
      else {  //Pokud není vkládaný klíè v tabulce,
        if(compare < 0) {  //tak pokud je vkládaný klíè vìt¹í ne¾ aktuální,
          if(fptr->rightItem == NULL) {  //kdy¾ nemá pravého potomka,
            fptr->rightItem = newItem;  //ulo¾ím nový prvek jako pravého potomka
            myend = 1;  //a nastavím konec cyklu
          }
          else {  //pokud má pravého potomka,
            fptr = fptr->rightItem;   //nastavím ukazatel na pravého potomka
          }
        }
        else {  //jinak,
          if(compare > 0) {  //pokud je vkládaný klíè men¹í ne¾ aktuální,
            if(fptr->leftItem == NULL) { //kdy¾ nemá levého potomka
              fptr->leftItem = newItem;  //ulo¾ím nový prvek jako levého potomka
              myend = 1;  //a nastavím konec cyklu
            }
            else {  //pokud má levého potomka,
              fptr = fptr->leftItem;  //nastavím ukazatel na levého potomka
            }
          }
        }
      }
    } // konec cyklu while
  } // konec else, kdy¾ není klíè v tabulce

  return 0;  //V¹e je OK - vracím 0
}
//KONEC funkce tableInsert

//Funkce, která vyhledává v tabulce symbolù prvek podle klíèe
//Parametry:
//  T - ukazatel na tabulku symbolù
//  key - klíè symbolu (název identifikátoru)
//Návratová hodnota:
//  NULL - prvek s klíèem key v tabulce není
//  tTableItem* - ukazatel na strukturu tData, která obsahuje hodnotu a typ prvku
tTableItem *tableSearch(tSymbolTable *T, string *key) {
// pokud polozku s danym klicem najdu, tak vratim adresu na jeji data
// pokud nenajdu, tak vratim NULL

  tTableItem *ptr;  //Pomocný ukazatel
  int cmp;  //Pomocná promìnná

  ptr = T->first;  //Nastavím ukazatel na první prvek tabulky

  while(ptr) {  //Dokud není ukazatel ptr NULL
    //Funkce strCmpString vrací:
    //  0 - øetìzce jsou stejné
    //  kladné èíslo - kdy¾ s2 < s1
    //  záporné èíslo - kdy¾ s2 > s1
    cmp = (strCmpString(&(ptr->key), key));  //Porovnám aktuální klíè s hledaným

    if(cmp > 0) {  //Pokud je cmp kladné èíslo,
      ptr = ptr->leftItem;  //nastavím ukazatel na levý prvek aktuálního
    }
    else if(cmp < 0) {  //Pokud je cmp záporné èíslo,
      ptr = ptr->rightItem;  //nastavím ukazatel na pravý prvek atuálního
    }
    else {  //Pokud je cmp èíslo 0, na¹el jsem hledaný klíè
      return ptr;  //a vrátím ukazatel na jeho strukturu s daty
    }
  } 
  
  return NULL;  //Pokud jsem prvek s klíèem key nena¹el, vracím NULL
}
//KONEC funkce tableSearch

//Funkce, která pøidává hodnotu k prvku v tabulce podle klíèe
//Parametry:
//  T - tabulka symbolù
//  key - klíè prvku, kterému chci pøidat hodnotu
//  value - hodnota, kterou chci pøidat prvku (je to string, dostanu od LEXu)
int tableInsertValue(tSymbolTable *T, string *key, string *value) {
  
  tTableItem *helper;  //Pomocný ukazatel na strukturu s hodnotou 
  
  if((helper = tableSearch(T, key)) != NULL) {  //Pokud se na¹el klíè v tabulce,
    switch(helper->data->varType) {  //podle datového typu ulo¾ím hodnotu
      case TYPE_INTEGER : {  //Pokud jde o integer
		  if(((helper->data->value.integerNumber = atoi(strGetStr(value))) == INT_MAX) || ((helper->data->value.integerNumber = atoi(strGetStr(value))) == INT_MIN)) { //Nastavím
			  printf("INT_MAX or INT_MIN\n");
			  return LEX_ERROR;
		  }
        break;  //Musím ukonèit vìtev!
      }
      case TYPE_DOUBLE : {  //Pokud jde o double
		  if((helper->data->value.decimalNumber = atof(strGetStr(value))) == HUGE_VAL) { //Nastavím
			  printf("HUGE_VAL\n");
			  return LEX_ERROR;
		  }
        break;  //Musím ukonèit vìtev!
      }
      case TYPE_STRING : {  //Pokud jde o string
        strCopyString(&(helper->data->value.stringValue), value);  //Nastavím
        break;  //Tady ukonèit vìtev nemusím, ale udìlám to :P
      }
    }
  }
  return SYNTAX_OK;
}
//KONEC funkce tableInsertValue


//Funkce, která zru¹í tabulku symblù
//Parametry:
// tableItem - ukazatel na první prvek tabulky (T->first)
//Funkce nic nevrací, je typu void
void tableDispose(tTableItem *tableItem) {

  if(tableItem) {  //Pokud není tableItem NULL,
    tableDispose(tableItem->leftItem);  //volám funkci na levý prvek,
    tableDispose(tableItem->rightItem);  //volám funkci na pravý prvek,
      
    strFree(&(tableItem->key));  //uvolním pamì» po klíèi prvku
    if(tableItem->data->varType == TYPE_STRING) {  //Pokud byl prvek string,
      strFree(&(tableItem->data->value.stringValue));  //uvolním mu pamì»
    }
    free(tableItem->data);  //uvolním pamì» po struktuøe s hodnotou,
    free(tableItem);  //uvolním pamì» po prvku
    tableItem = NULL;  //a nastavím ho na NULL
  }
}
//KONEC funkce tableDispose

//Funkce, která zpracuje (vypí¹e) prvek
//Parametry:
//  item - prvek tabulky, který chci zpracovat
//Funkce nic nevrací, je typu void
void print(tTableItem *item) {

  //printf("Identifikator: %s\n", strGetStr(&(item->key)));  //Výpis názvu prvku

  switch(item->data->varType){  //Podle typu prvku vypí¹u hodnotu
    case TYPE_INTEGER : {  //Kdy¾ je integer,
      printf("Nazev: %s %d\n", strGetStr(&item->key), (int)item);
      printf("Typ: int\n");  //datovy typ
      printf("Hodnota: %d\n\n", item->data->value.integerNumber); // a hodnotu
      break;  //Musím ukonèit vìtev!
    }
    case TYPE_DOUBLE : {  //Kdy¾ je double,
      printf("Nazev: %s %d\n", strGetStr(&item->key), (int)item); // vypisu nazev
      printf("Typ: double\n");  // vypí¹u, ¾e je double
      printf("Hodnota: %f\n\n", item->data->value.decimalNumber); // a hodnotu
      break;  //Musím ukonèit vìtev!
    }
    case TYPE_STRING : {  //Kdy¾ je string,
      printf("Nazev: %s %d\n", strGetStr(&item->key), (int)item); // vypisu nazev
      printf("Typ: string\n");  //vypí¹u, ¾e je string a jeho hodnotu
      printf("Hodnota: %s\n\n", strGetStr(&(item->data->value.stringValue)));
    }
  }

}
//KONEC funkce print

//Funkce, která vypí¹e celou tabulku
//Parametry:
//  item - první prvek tabulky (T->first)
//Funkce nic nevrací, je typu void
void vypis(tTableItem *item) {

  if(item) {  //Pokud není prvek NULL,
    vypis(item->leftItem);  //zavolám funkci vypis na levý prvek,
    print(item);  //zpracuji prvek aktuální
    vypis(item->rightItem);  //a zavolám funkci vypis na pravý prvek
  }

}
//KONEC funkce vypis
