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

// Hlavi�kov� soubor knihovny pro pr�ci s �et�zci

#define STR_ERROR 1005
#define STR_SUCCESS 1000

// Struktura pro datov� typ 'string'
typedef struct
{
  char* str;		// �et�zec ukon�en� nulov�m znakem '\0'
  int length;		// Skute�n� d�lka �et�zce
  int allocSize;	// Alokovan� d�lka
} string;  // Jm�no nov�ho datov�ho typu

// Funkce pro pr�ci s pam�t�
int strInit(string *s);  // Funkce inicializuje string OK
void strFree(string *s);  // Funkce uvol�uje pam� po string OK

// Funkce pro porovn�v�n� �et�zc�
int strCmpString(string *s1, string *s2);  // Funkce porovn� �et�zce s1 a s2 OK
int strCmpConstStr(string *s1, char *s2);  // Funkce porovn� �et�zec string a �et�zec OK

// Funkce vracej�c� obsah �et�zce
char *strGetStr(string *s);  // Funkce vrac� �et�zec ze stringu OK
int strGetLength(string *s);  // Funkce vrac� d�lku �et�zce OK

// Funkce pro kop�rov�n� �et�zc�
int strCopyString(string *s1, string *s2);  // Funkce kop�ruje �et�zec s2 do s1 OK
int strCopyChar(string *s1, char retezec[]);  // Funkce zkop�ruje �et�zec retezec do stringu OK

// Funkce pro spojen� �et�zc�
int concateStrings(string *s1, string *s2, string *s3);  // Funkce spoj� dva �et�zce s1 a s2 do �et�zce s3 OK
int strConcate(string *s1, string *s2);  // Funkce p�id� �et�zec s2 do �et�zce s1 OK

// Funkce pro pr�ci s �et�zcem
void strClear(string *s);  // Funkce vyprazd�uje �et�zec OK
int strAddChar(string *s1, char c);  // Funkce p�id� znak c na konec �et�zce OK
int find(string *s1, string *s2);  // Funkce vyhled� pod�et�zec s1 v �et�zci s2 OK
void mergessort(string *s1, int levy, int pravy);  // Funkce pro se�azen� �et�zce OK
void merge(string *s1, int lev, int piv, int prav);  // Pomocn� funkce pro funkci mergessort OK
