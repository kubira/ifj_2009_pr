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

// Hlavièkový soubor knihovny pro práci s øetìzci

#define STR_ERROR 1005
#define STR_SUCCESS 1000

// Struktura pro datový typ 'string'
typedef struct
{
  char* str;		// Øetìzec ukonèený nulovým znakem '\0'
  int length;		// Skuteèná délka øetìzce
  int allocSize;	// Alokovaná délka
} string;  // Jméno nového datového typu

// Funkce pro práci s pamìtí
int strInit(string *s);  // Funkce inicializuje string OK
void strFree(string *s);  // Funkce uvolòuje pamì» po string OK

// Funkce pro porovnávání øetìzcù
int strCmpString(string *s1, string *s2);  // Funkce porovná øetìzce s1 a s2 OK
int strCmpConstStr(string *s1, char *s2);  // Funkce porovná øetìzec string a øetìzec OK

// Funkce vracející obsah øetìzce
char *strGetStr(string *s);  // Funkce vrací øetìzec ze stringu OK
int strGetLength(string *s);  // Funkce vrací délku øetìzce OK

// Funkce pro kopírování øetìzcù
int strCopyString(string *s1, string *s2);  // Funkce kopíruje øetìzec s2 do s1 OK
int strCopyChar(string *s1, char retezec[]);  // Funkce zkopíruje øetìzec retezec do stringu OK

// Funkce pro spojení øetìzcù
int concateStrings(string *s1, string *s2, string *s3);  // Funkce spojí dva øetìzce s1 a s2 do øetìzce s3 OK
int strConcate(string *s1, string *s2);  // Funkce pøidá øetìzec s2 do øetìzce s1 OK

// Funkce pro práci s øetìzcem
void strClear(string *s);  // Funkce vyprazdòuje øetìzec OK
int strAddChar(string *s1, char c);  // Funkce pøidá znak c na konec øetìzce OK
int find(string *s1, string *s2);  // Funkce vyhledá podøetìzec s1 v øetìzci s2 OK
void mergessort(string *s1, int levy, int pravy);  // Funkce pro seøazení øetìzce OK
void merge(string *s1, int lev, int piv, int prav);  // Pomocná funkce pro funkci mergessort OK
