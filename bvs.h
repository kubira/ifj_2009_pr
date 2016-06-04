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

// definice datovych typu

#define TYPE_STRING 100
#define TYPE_INTEGER 101
#define TYPE_DOUBLE 102

typedef union {
      int integerNumber;      // cele cislo, pristupuj tData.value.integerNumber
      double decimalNumber;   // desetinne cislo, tData.value.decimalNumber
      string stringValue;     // retezec, novy_ret strInit(&(tData.value.stringValue))
                              // vypis strGetStr(&(tData.value.stringValue)) - vraci char
} typeValue;

typedef struct
{
  int varType;                // typ dane promenne, (int, double, string)
  typeValue value;            // union
} tData;
  
typedef struct tableItem
{
  string key;                   // klic, podle ktereho se bude vyhledavat = nazev identifikatoru
  tData *data;                   // data, ktera jsou ke klici pridruzena
  struct tableItem *leftItem;   // ukazatel na leveho potomka
  struct tableItem *rightItem;  // ukazatel na praveho potomka
} tTableItem;

typedef struct                  // koren stromu
{
  struct tableItem *first;
} tSymbolTable;


//Funkce pro práci s tabulkou symbolù - stromem
void tableInit(tSymbolTable *T);
int tableInsert(tSymbolTable *T, string *key, int varType);
tTableItem *tableSearch(tSymbolTable *T, string *key);
void tableDispose(tTableItem *tableItem);
void print(tTableItem *item);
void vypis(tTableItem *item);
int tableInsertValue(tSymbolTable *T, string *key, string *value);
