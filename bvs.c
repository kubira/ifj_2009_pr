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

//Funkce pro inicializaci tabulky symbol�
//Parametry:
//  T - ukazatel na tabulku symbol�
//Funkce nic nevrac� - je typu void
void tableInit(tSymbolTable *T) {
  T->first = NULL;  //Inicializuje tak, �e nastav� prvn� prvek tabulky na NULL
}
//KONEC funkce tableInit

//Funkce, kter� vkl�d� nov� symbol (identifik�tor) do tabulky symbol�
//Parametry:
//  T - ukazatel na tabulku symbol�
//  key - kl�� symbolu (n�zev identifik�toru)
//  varType - datov� typ symbolu (identifik�toru)
//N�vratov� hodnota funkce:
//  0 - v�e je OK
//  1 - nepoda�ilo se vlo�it prvek, proto�e nen� pam�
//  2 - prvek s vkl�dan�m kl��em ji� v tabulce je - SEM_ERROR!
int tableInsert(tSymbolTable *T, string *key, int varType) {
  int ok=0, myend=0, compare;  //Pomocn� prom�nn�
  tTableItem *fptr;  //Pomocn� ukazatel na otcovsk� uzel
  tTableItem *newItem;  //Ukazatel na nov� vkl�dan� symbol (identifik�tor)
  tData *data;  //Pomocn� ukazatel na union nov� vkl�dan�ho symbolu

  data = malloc(sizeof(tData));  //Alokuji pam� unionu

  switch(varType) {  //Podle datov�ho typu symbolu nastav�m union
    case TYPE_INTEGER : {  //Kdy� je to integer,
      data->value.integerNumber = 0;  //nastav�m v unionu integerNumber na 0
      break;  //Mus�m ukon�it v�tev!
    }
    case TYPE_DOUBLE : {  //Kdy� je to double,
      data->value.decimalNumber = 0;  //nastav�m v unionu decimalNumber na 0
      break;  //Mus�m ukon�it v�tev
    }
    case TYPE_STRING : {  //Kdy� je to string,
      strInit(&(data->value.stringValue));  //inicializuji v unionu stringValue
      break;  //Tady v�tev ukon�it nemus�m, ale ud�l�m to :P
    }
  }

  if((newItem = (tTableItem*)malloc(sizeof(struct tableItem)))) {
    //Alokuji pam� nov�mu prvku tabulky a pokud se alokace poda�ila,
    newItem->leftItem  = NULL;  //nastav�m lev�ho potomka na NULL,
    newItem->rightItem = NULL;  //nastav�m prav�ho potomka na NULL,
    newItem->data = data;  //nastav�m ukazatel na union na union vytvo�en�
    strInit(&(newItem->key));  //a inicializuji string prvku pro kl��

    ok = strCopyString(&(newItem->key), key);  //Zkop�ruji kl�� do kl��e prvku

    newItem->data->varType = varType;  //Nastav�m datov� typ unionu v prvku
  }
  else {  //Pokud se nepoda�ilo alokovat pam�,
    printf("Nepoda�ilo se alokovat pam�");  //vyp�i hl�en� na obrazovku
    return 1;  //a vrac�m 0 - nepoda�ilo se vlo�it prvek
  }

  if(T->first == NULL) {  //Pokud je tabulka pr�zdn� (prvn� prvek je NULL),
    T->first = newItem;  //stane se nov� prvek automaticky prvn�m prvkem tabulky
    myend = 1;  //Nastav�m pomocnou prom�nnou pro konec cyklu na 1
  }
  else {  //Pokud nen� tabulka pr�zdn�,
    fptr = T->first; //tak mus�m tabulku proch�zet pomoc� ukazatele fptr

    while(!myend) {  //Proch�z�m tabulku, dokud nen� myend nastaven na 1
      compare = strCmpString((&fptr->key), key);  //Porovn�m,
      //jestli u� nejsem na prvku s kl��em, kter� vkl�d�m

      if(compare == 0) {  //Pokud vkl�dan� hodnota v tabulce u� je,
        return 2;  //nastala s�mantick� chyba!!! SEM_ERROR <--------------------------->
      }
      else {  //Pokud nen� vkl�dan� kl�� v tabulce,
        if(compare < 0) {  //tak pokud je vkl�dan� kl�� v�t�� ne� aktu�ln�,
          if(fptr->rightItem == NULL) {  //kdy� nem� prav�ho potomka,
            fptr->rightItem = newItem;  //ulo��m nov� prvek jako prav�ho potomka
            myend = 1;  //a nastav�m konec cyklu
          }
          else {  //pokud m� prav�ho potomka,
            fptr = fptr->rightItem;   //nastav�m ukazatel na prav�ho potomka
          }
        }
        else {  //jinak,
          if(compare > 0) {  //pokud je vkl�dan� kl�� men�� ne� aktu�ln�,
            if(fptr->leftItem == NULL) { //kdy� nem� lev�ho potomka
              fptr->leftItem = newItem;  //ulo��m nov� prvek jako lev�ho potomka
              myend = 1;  //a nastav�m konec cyklu
            }
            else {  //pokud m� lev�ho potomka,
              fptr = fptr->leftItem;  //nastav�m ukazatel na lev�ho potomka
            }
          }
        }
      }
    } // konec cyklu while
  } // konec else, kdy� nen� kl�� v tabulce

  return 0;  //V�e je OK - vrac�m 0
}
//KONEC funkce tableInsert

//Funkce, kter� vyhled�v� v tabulce symbol� prvek podle kl��e
//Parametry:
//  T - ukazatel na tabulku symbol�
//  key - kl�� symbolu (n�zev identifik�toru)
//N�vratov� hodnota:
//  NULL - prvek s kl��em key v tabulce nen�
//  tTableItem* - ukazatel na strukturu tData, kter� obsahuje hodnotu a typ prvku
tTableItem *tableSearch(tSymbolTable *T, string *key) {
// pokud polozku s danym klicem najdu, tak vratim adresu na jeji data
// pokud nenajdu, tak vratim NULL

  tTableItem *ptr;  //Pomocn� ukazatel
  int cmp;  //Pomocn� prom�nn�

  ptr = T->first;  //Nastav�m ukazatel na prvn� prvek tabulky

  while(ptr) {  //Dokud nen� ukazatel ptr NULL
    //Funkce strCmpString vrac�:
    //  0 - �et�zce jsou stejn�
    //  kladn� ��slo - kdy� s2 < s1
    //  z�porn� ��slo - kdy� s2 > s1
    cmp = (strCmpString(&(ptr->key), key));  //Porovn�m aktu�ln� kl�� s hledan�m

    if(cmp > 0) {  //Pokud je cmp kladn� ��slo,
      ptr = ptr->leftItem;  //nastav�m ukazatel na lev� prvek aktu�ln�ho
    }
    else if(cmp < 0) {  //Pokud je cmp z�porn� ��slo,
      ptr = ptr->rightItem;  //nastav�m ukazatel na prav� prvek atu�ln�ho
    }
    else {  //Pokud je cmp ��slo 0, na�el jsem hledan� kl��
      return ptr;  //a vr�t�m ukazatel na jeho strukturu s daty
    }
  } 
  
  return NULL;  //Pokud jsem prvek s kl��em key nena�el, vrac�m NULL
}
//KONEC funkce tableSearch

//Funkce, kter� p�id�v� hodnotu k prvku v tabulce podle kl��e
//Parametry:
//  T - tabulka symbol�
//  key - kl�� prvku, kter�mu chci p�idat hodnotu
//  value - hodnota, kterou chci p�idat prvku (je to string, dostanu od LEXu)
int tableInsertValue(tSymbolTable *T, string *key, string *value) {
  
  tTableItem *helper;  //Pomocn� ukazatel na strukturu s hodnotou 
  
  if((helper = tableSearch(T, key)) != NULL) {  //Pokud se na�el kl�� v tabulce,
    switch(helper->data->varType) {  //podle datov�ho typu ulo��m hodnotu
      case TYPE_INTEGER : {  //Pokud jde o integer
		  if(((helper->data->value.integerNumber = atoi(strGetStr(value))) == INT_MAX) || ((helper->data->value.integerNumber = atoi(strGetStr(value))) == INT_MIN)) { //Nastav�m
			  printf("INT_MAX or INT_MIN\n");
			  return LEX_ERROR;
		  }
        break;  //Mus�m ukon�it v�tev!
      }
      case TYPE_DOUBLE : {  //Pokud jde o double
		  if((helper->data->value.decimalNumber = atof(strGetStr(value))) == HUGE_VAL) { //Nastav�m
			  printf("HUGE_VAL\n");
			  return LEX_ERROR;
		  }
        break;  //Mus�m ukon�it v�tev!
      }
      case TYPE_STRING : {  //Pokud jde o string
        strCopyString(&(helper->data->value.stringValue), value);  //Nastav�m
        break;  //Tady ukon�it v�tev nemus�m, ale ud�l�m to :P
      }
    }
  }
  return SYNTAX_OK;
}
//KONEC funkce tableInsertValue


//Funkce, kter� zru�� tabulku symbl�
//Parametry:
// tableItem - ukazatel na prvn� prvek tabulky (T->first)
//Funkce nic nevrac�, je typu void
void tableDispose(tTableItem *tableItem) {

  if(tableItem) {  //Pokud nen� tableItem NULL,
    tableDispose(tableItem->leftItem);  //vol�m funkci na lev� prvek,
    tableDispose(tableItem->rightItem);  //vol�m funkci na prav� prvek,
      
    strFree(&(tableItem->key));  //uvoln�m pam� po kl��i prvku
    if(tableItem->data->varType == TYPE_STRING) {  //Pokud byl prvek string,
      strFree(&(tableItem->data->value.stringValue));  //uvoln�m mu pam�
    }
    free(tableItem->data);  //uvoln�m pam� po struktu�e s hodnotou,
    free(tableItem);  //uvoln�m pam� po prvku
    tableItem = NULL;  //a nastav�m ho na NULL
  }
}
//KONEC funkce tableDispose

//Funkce, kter� zpracuje (vyp�e) prvek
//Parametry:
//  item - prvek tabulky, kter� chci zpracovat
//Funkce nic nevrac�, je typu void
void print(tTableItem *item) {

  //printf("Identifikator: %s\n", strGetStr(&(item->key)));  //V�pis n�zvu prvku

  switch(item->data->varType){  //Podle typu prvku vyp�u hodnotu
    case TYPE_INTEGER : {  //Kdy� je integer,
      printf("Nazev: %s %d\n", strGetStr(&item->key), (int)item);
      printf("Typ: int\n");  //datovy typ
      printf("Hodnota: %d\n\n", item->data->value.integerNumber); // a hodnotu
      break;  //Mus�m ukon�it v�tev!
    }
    case TYPE_DOUBLE : {  //Kdy� je double,
      printf("Nazev: %s %d\n", strGetStr(&item->key), (int)item); // vypisu nazev
      printf("Typ: double\n");  // vyp�u, �e je double
      printf("Hodnota: %f\n\n", item->data->value.decimalNumber); // a hodnotu
      break;  //Mus�m ukon�it v�tev!
    }
    case TYPE_STRING : {  //Kdy� je string,
      printf("Nazev: %s %d\n", strGetStr(&item->key), (int)item); // vypisu nazev
      printf("Typ: string\n");  //vyp�u, �e je string a jeho hodnotu
      printf("Hodnota: %s\n\n", strGetStr(&(item->data->value.stringValue)));
    }
  }

}
//KONEC funkce print

//Funkce, kter� vyp�e celou tabulku
//Parametry:
//  item - prvn� prvek tabulky (T->first)
//Funkce nic nevrac�, je typu void
void vypis(tTableItem *item) {

  if(item) {  //Pokud nen� prvek NULL,
    vypis(item->leftItem);  //zavol�m funkci vypis na lev� prvek,
    print(item);  //zpracuji prvek aktu�ln�
    vypis(item->rightItem);  //a zavol�m funkci vypis na prav� prvek
  }

}
//KONEC funkce vypis
