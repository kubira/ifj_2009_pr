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

// Knihovna pro práci s nekoneènì dlouhými øetìzci
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"

#define STR_LEN_INC 8
// Konstanta STR_LEN_INC udává, na kolik bytù provedeme poèateèní alokaci pamìti
// Pokud naèítáme øetìzec znak po znaku, pamì» se postupnì bude alokovat na
// násobky tohoto èísla

int strInit(string *s)
// Funkce vytvoøí nový øetìzec
{
   if((s->str = (char*)malloc(STR_LEN_INC)) == NULL)  // Alokace a její kontrola
      return STR_ERROR;  // Pøi neúspì¹né alokaci vracím chybu

   s->str[0] = '\0';  // Nastavím první znak na nulový -> prázdný øetìzec
   s->length = 0;  // Nastavím délku øetìzce na 0
   s->allocSize = STR_LEN_INC;  // Nastavím alokovanou velikost na STR_LEN_INC
   
   return STR_SUCCESS;  // Úspì¹né vytvoøení nového øetìzce
}

void strFree(string *s)
// Funkce uvolní øetìzec z pamìti
{
   free(s->str);  // Uvolnìní øetìzce z pamìti
}

void strClear(string *s)
// Funkce vyma¾e obsah øetìzce
{
   s->str[0] = '\0';  // Nastavím první znak na nulový
   s->length = 0;  // Nastavím délku øetìzce na 0
}

int strAddChar(string *s1, char c)
// Pøidá na konec øetìzce jeden znak
{
   if(s1->length + 1 >= s1->allocSize)  // Pokud alokovaná pamì» nebude staèit
   {
      // Realokuji pamì» øetìzce
      if((s1->str = (char*)realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;  // Pøi neúspì¹né realokaci vracím chybu

      s1->allocSize = s1->length + STR_LEN_INC;  // Pøenastavím alokovanou pamì»
   }
   
   s1->str[s1->length] = c;  // Pøidám znak c na konec øetìzce
   s1->length++;  // Zvìt¹ím délku øetìzce
   s1->str[s1->length] = '\0';  // Posunu nulový znak o 1 pozici
   
   return STR_SUCCESS;  // Úspì¹né pøidání znaku do øetìzce
}

int strCopyString(string *s1, string *s2)
// Funkce pøekopíruje øetìzec s2 do s1
{
   int newLength = s2->length;  // Zjistím délku kopírovaného øetìzce
   
   if(newLength >= s1->allocSize)  // Pokud alokovaná pamì» nebude staèit
   {
      // Realokuji pamì» øetìzce
      if((s1->str = (char*)realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;  // Pøi neúspì¹né realokaci vracím chybu

      s1->allocSize = newLength;  // Pøenastavím alokovanou pamì»
   }

   strcpy(s1->str, s2->str);  // Zkopíruji øetìzec s2 do øetìzce s1
   s1->length = newLength;  // Nastavím délku øetìzce

   return STR_SUCCESS;  // Úspì¹né zkopírování øetìzce s2 do s1
}

int strCmpString(string *s1, string *s2)
// Funkce porovná oba øetìzce a vrátí výsledek
{
   return strcmp(s1->str, s2->str);  // Vrací výsledek porovnání
}

int strCmpConstStr(string *s1, char* s2)
// Funkce porovná ná¹ øetìzec s konstantním øetìzcem
{
   return strcmp(s1->str, s2);  // Vrací výsledek porovnání
}

char *strGetStr(string *s)
// Funkce vrátí textovou èást øetìzce
{
   return s->str;  // Vrací obsah øetìzce
}

int strGetLength(string *s)
// Funkce vrátí délku daného øetìzce
{
   return s->length;  // Vrací délku øetìzce
}

int strConcate(string *s1, string *s2)
// Funkce pøidá øetìzec s2 do øetìzce s1
{
    int delka1, delka2, newLength, index;  // Promìnné pro pøidání
    
    delka1 = s1->length;  // Zjistím délku prvního
    
    delka2 = s2->length;  // Zjistím délku druhého
    
    newLength = delka1 + delka2;  // Nová délka

    if(newLength >= s1->allocSize)  // Pokud alokovaná pamì» nebude staèit
    {
      // Realokuji pamì» øetìzce
      if((s1->str = (char*)realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;  // Pøi neúspì¹né realokaci vracím chybu

      s1->allocSize = newLength;  // Pøenastavím alokovanou pamì»
    }
    
    // Zkopíruji po znacích druhý øetìzec do prvního øetìzce
    for(index = 0; index < delka2; index++)
    {
      if(strAddChar(s1, s2->str[index]) == STR_ERROR)
      // Pøidám znak na konec øetìzce s1 a kontrola
        return STR_ERROR;  // Vracím chybu pøidání znaku
    }
    
    s1->length = newLength;  // Zmìním délku v prvním øetìzci

    return STR_SUCCESS;  // Úspì¹né pøidání øetìzce s2 do s1
}

int concateStrings(string *s1, string *s2, string *s3)
// Funkce zkopíruje øetìzec s1 do s3 a pøidá za nìj øetìzec s2
{   
    int delka, index;  // Promìnné pro pøidávání
    
    if(strCopyString(s3,s1) == STR_ERROR)  // Zkopíruji s1 do s3 a kontrola
      return STR_ERROR;
    
    delka = strGetLength(s2); // Ulo¾ím si délku druhého

    for(index = 0; index < delka; index++)  // Kopíruji do s3 znaky z s2
    {
      if(strAddChar(s3, s2->str[index]) == STR_ERROR)
      // Pøidám znak na konec øetìzce a kontrola
        return STR_ERROR;
    }
    
    return STR_SUCCESS;  // Úspì¹né slouèení øetìzcù s1 a s2 do øetìzce s3
}

int strCopyChar(string *s1, char retezec[])
// Funkce zkopíruje øetìzec do øetìzce s1
{
  int delka, x;  // Promìnné pro kopírování

  strClear(s1);  // Vyprázdnìní prvního øetìzce
  
  delka = strlen(retezec);  // Zjistím délku øetìzce
  
  for(x = 0; x < delka; x++)  // Procházím øetìzec po znacích
  {
    if(strAddChar(s1, retezec[x]) == STR_ERROR)
    // Pøidávám znak na konec øetìzce s1
      return STR_ERROR;
  }
  
  return STR_SUCCESS;  // Úspì¹né zkopírování øetìzce do øetìzce s1
}

/**************************************************************************************************/
/*                                       TODO                                                     */
/**************************************************************************************************/

int find(string *s1, string *s2) {

  int i = -1, j, citac=0;
  int delka_vzorku = s1->length; //delka vzorku
  char *vektor = (char*)malloc((delka_vzorku+1)*sizeof(char));  //vektor, ktery bude udrzovat cisla o kolik se ma text pri vyhledavani posunout

   vektor[0] = -1;

//VYTVOØENÍ VEKTORU
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
