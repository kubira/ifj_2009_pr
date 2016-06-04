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

// Knihovna pro pr�ci s nekone�n� dlouh�mi �et�zci
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"

#define STR_LEN_INC 8
// Konstanta STR_LEN_INC ud�v�, na kolik byt� provedeme po�ate�n� alokaci pam�ti
// Pokud na��t�me �et�zec znak po znaku, pam� se postupn� bude alokovat na
// n�sobky tohoto ��sla

int strInit(string *s)
// Funkce vytvo�� nov� �et�zec
{
   if((s->str = (char*)malloc(STR_LEN_INC)) == NULL)  // Alokace a jej� kontrola
      return STR_ERROR;  // P�i ne�sp�n� alokaci vrac�m chybu

   s->str[0] = '\0';  // Nastav�m prvn� znak na nulov� -> pr�zdn� �et�zec
   s->length = 0;  // Nastav�m d�lku �et�zce na 0
   s->allocSize = STR_LEN_INC;  // Nastav�m alokovanou velikost na STR_LEN_INC
   
   return STR_SUCCESS;  // �sp�n� vytvo�en� nov�ho �et�zce
}

void strFree(string *s)
// Funkce uvoln� �et�zec z pam�ti
{
   free(s->str);  // Uvoln�n� �et�zce z pam�ti
}

void strClear(string *s)
// Funkce vyma�e obsah �et�zce
{
   s->str[0] = '\0';  // Nastav�m prvn� znak na nulov�
   s->length = 0;  // Nastav�m d�lku �et�zce na 0
}

int strAddChar(string *s1, char c)
// P�id� na konec �et�zce jeden znak
{
   if(s1->length + 1 >= s1->allocSize)  // Pokud alokovan� pam� nebude sta�it
   {
      // Realokuji pam� �et�zce
      if((s1->str = (char*)realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;  // P�i ne�sp�n� realokaci vrac�m chybu

      s1->allocSize = s1->length + STR_LEN_INC;  // P�enastav�m alokovanou pam�
   }
   
   s1->str[s1->length] = c;  // P�id�m znak c na konec �et�zce
   s1->length++;  // Zv�t��m d�lku �et�zce
   s1->str[s1->length] = '\0';  // Posunu nulov� znak o 1 pozici
   
   return STR_SUCCESS;  // �sp�n� p�id�n� znaku do �et�zce
}

int strCopyString(string *s1, string *s2)
// Funkce p�ekop�ruje �et�zec s2 do s1
{
   int newLength = s2->length;  // Zjist�m d�lku kop�rovan�ho �et�zce
   
   if(newLength >= s1->allocSize)  // Pokud alokovan� pam� nebude sta�it
   {
      // Realokuji pam� �et�zce
      if((s1->str = (char*)realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;  // P�i ne�sp�n� realokaci vrac�m chybu

      s1->allocSize = newLength;  // P�enastav�m alokovanou pam�
   }

   strcpy(s1->str, s2->str);  // Zkop�ruji �et�zec s2 do �et�zce s1
   s1->length = newLength;  // Nastav�m d�lku �et�zce

   return STR_SUCCESS;  // �sp�n� zkop�rov�n� �et�zce s2 do s1
}

int strCmpString(string *s1, string *s2)
// Funkce porovn� oba �et�zce a vr�t� v�sledek
{
   return strcmp(s1->str, s2->str);  // Vrac� v�sledek porovn�n�
}

int strCmpConstStr(string *s1, char* s2)
// Funkce porovn� n� �et�zec s konstantn�m �et�zcem
{
   return strcmp(s1->str, s2);  // Vrac� v�sledek porovn�n�
}

char *strGetStr(string *s)
// Funkce vr�t� textovou ��st �et�zce
{
   return s->str;  // Vrac� obsah �et�zce
}

int strGetLength(string *s)
// Funkce vr�t� d�lku dan�ho �et�zce
{
   return s->length;  // Vrac� d�lku �et�zce
}

int strConcate(string *s1, string *s2)
// Funkce p�id� �et�zec s2 do �et�zce s1
{
    int delka1, delka2, newLength, index;  // Prom�nn� pro p�id�n�
    
    delka1 = s1->length;  // Zjist�m d�lku prvn�ho
    
    delka2 = s2->length;  // Zjist�m d�lku druh�ho
    
    newLength = delka1 + delka2;  // Nov� d�lka

    if(newLength >= s1->allocSize)  // Pokud alokovan� pam� nebude sta�it
    {
      // Realokuji pam� �et�zce
      if((s1->str = (char*)realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;  // P�i ne�sp�n� realokaci vrac�m chybu

      s1->allocSize = newLength;  // P�enastav�m alokovanou pam�
    }
    
    // Zkop�ruji po znac�ch druh� �et�zec do prvn�ho �et�zce
    for(index = 0; index < delka2; index++)
    {
      if(strAddChar(s1, s2->str[index]) == STR_ERROR)
      // P�id�m znak na konec �et�zce s1 a kontrola
        return STR_ERROR;  // Vrac�m chybu p�id�n� znaku
    }
    
    s1->length = newLength;  // Zm�n�m d�lku v prvn�m �et�zci

    return STR_SUCCESS;  // �sp�n� p�id�n� �et�zce s2 do s1
}

int concateStrings(string *s1, string *s2, string *s3)
// Funkce zkop�ruje �et�zec s1 do s3 a p�id� za n�j �et�zec s2
{   
    int delka, index;  // Prom�nn� pro p�id�v�n�
    
    if(strCopyString(s3,s1) == STR_ERROR)  // Zkop�ruji s1 do s3 a kontrola
      return STR_ERROR;
    
    delka = strGetLength(s2); // Ulo��m si d�lku druh�ho

    for(index = 0; index < delka; index++)  // Kop�ruji do s3 znaky z s2
    {
      if(strAddChar(s3, s2->str[index]) == STR_ERROR)
      // P�id�m znak na konec �et�zce a kontrola
        return STR_ERROR;
    }
    
    return STR_SUCCESS;  // �sp�n� slou�en� �et�zc� s1 a s2 do �et�zce s3
}

int strCopyChar(string *s1, char retezec[])
// Funkce zkop�ruje �et�zec do �et�zce s1
{
  int delka, x;  // Prom�nn� pro kop�rov�n�

  strClear(s1);  // Vypr�zdn�n� prvn�ho �et�zce
  
  delka = strlen(retezec);  // Zjist�m d�lku �et�zce
  
  for(x = 0; x < delka; x++)  // Proch�z�m �et�zec po znac�ch
  {
    if(strAddChar(s1, retezec[x]) == STR_ERROR)
    // P�id�v�m znak na konec �et�zce s1
      return STR_ERROR;
  }
  
  return STR_SUCCESS;  // �sp�n� zkop�rov�n� �et�zce do �et�zce s1
}

/**************************************************************************************************/
/*                                       TODO                                                     */
/**************************************************************************************************/

int find(string *s1, string *s2) {

  int i = -1, j, citac=0;
  int delka_vzorku = s1->length; //delka vzorku
  char *vektor = (char*)malloc((delka_vzorku+1)*sizeof(char));  //vektor, ktery bude udrzovat cisla o kolik se ma text pri vyhledavani posunout

   vektor[0] = -1;

//VYTVO�EN� VEKTORU
  //vyvoreni vektoru, pomoci ktereho se nebudeme vracet pri vyhledavani v textu zpet
   for (j = 0; j < delka_vzorku; j++) {
      while ((i > -1) && (s1->str[j] != s1->str[i]))
         i = vektor[i];
      i++;
      if (s1->str[j+1] == s1->str[i])
         vektor[j+1] = vektor[i];
      else
         vektor[j+1] = i;
   }

  //zvetseni cisel ve vektoru o 1
  for(i =0; i < delka_vzorku; i++) {
    vektor[i] = vektor[i]+1;
  }

//VLASTNI HLEDANI PODRETEZCE
  i=0;
  j=0;
  //cyklus prochazejici hlavni retezec dokud nenarazi na konec
  while (s2->str[i] != '\0') {

  //pokud se hledany znak retezce shoduje s 1.znakem podretezce
    if(s2->str[i] == s1->str[0]) {
      //prochazej podretezec dokud nedojdes na konec
      while(s1->str[j] != '\0') {
        //pokud se dalsi znaky podretezce a retezce rovnaji
        if(s2->str[i+j] == s1->str[j]) {
          citac++;
        }
        else {
          //vzorek se nenasel - vymazou se pomocne promenne a skoci se o predem vypocitany vektor
          if(citac != delka_vzorku) {
            s2->str[i]=s2->str[i]+vektor[j];
            j=0;
            citac=0;
            break;
          }
        }

        j++;
        //v pripade ze se citac rovna velikosti pole - retezce se rovnaji a vraci se pozice hledaneho podretezce v retezci
        if(citac == delka_vzorku) {
		  free(vektor);
          return (i + 1);
        }
      }

    }
    i++;
  }

  free(vektor);
  return 0;

}

void mergessort(string *s1, int levy, int pravy){
    int pivot;

    if(levy<pravy){ //dokud nemame jedno cislo
        pivot=(levy+pravy)/2;
		mergessort(s1, levy, pivot); //rozdelime na levou
		mergessort(s1, pivot+1, pravy); //a pravou cast
		merge(s1, levy, pivot, pravy); //zatridime je do sebe
    }
}

void merge(string *s1, int lev, int piv, int prav){
    char *pomocne_pole; //pomuzeme si pomocnym polem
    int i, j, k; //pomocne promenna cyklu
    int levy=lev;
    int pravy=prav;
    int pivot=piv;

    pomocne_pole=(char *) malloc((pravy+1)*sizeof(char)); //naalokujeme ono pole

    for(i=levy; i<=pravy; i++){ //do pomocneho pole si prekopirujeme cele pole
		pomocne_pole[i]=s1->str[i];
    }

    i=levy;
    j=pivot+1;
    k=levy;

    while(i<=pivot && j<=pravy){ //nejvetsi prvek vzdycky nakopiruju do pole cisel
      if(pomocne_pole[i]<=pomocne_pole[j]){ //pokud je prvek z leve poloviny mensi, ulozim ho do vyslednyho pole
		  s1->str[k]=pomocne_pole[i];
          i++;
          k++;
      }
      else{ //jinak tam ulozim prvek z prave poloviny
		  s1->str[k]=pomocne_pole[j];
          j++;
          k++;
      }
     }

    while (i<=pivot){ //pokud existuje nejaky zbytek prvni pulky, dokopiruj ho do pole_cisel
		s1->str[k]=pomocne_pole[i];
        i++;
        k++;
    }

    free(pomocne_pole);
}
