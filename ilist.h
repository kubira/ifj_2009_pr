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

#define TRUE    1  //TRUE
#define FALSE   0  //FALSE

//tabulka instrukci
#define I_READ             2  //nacteni hodnoty
#define I_LENGTH           3  //vrati delku retezce
#define I_INTTODOUBLE      4  //pretypuje int to double
#define I_WRITE            5  //vypis hodnoty
#define I_FIND             6  //vyhleda hledany retezec a vrati jeho pozici
#define I_SORT             7  //seradi znaky v retezci
#define I_CONCAT           8  //provede konkatenaci (zretezeni) retezcu
#define I_GOTO             9  //skoc na adresu
#define I_IF_NOT          10  //podminka
#define I_RETURN          12  //ukonceni programu (vraci int)
#define I_STOP            13  //ukonceni programu
#define I_EXPR_PLUS       14  //vyraz - scitani +
#define I_EXPR_MINUS      15  //vyraz - odcitani -
#define I_EXPR_MUL        16  //vyraz - nasobeni *
#define I_EXPR_DIV        17  //vyraz - deleni /
#define I_EXPR_EXPR       18  //vyraz - prirazeni =
#define I_EXPR_LESS       19  //vyraz - mensi <
#define I_EXPR_ABOVE      20  //vyraz - vetsi >
#define I_EXPR_LESSEQUAL  21  //vyraz - mensi-rovno <=
#define I_EXPR_ABOVEEQUAL 22 //vyraz - vetsi-rovno >=
#define I_EXPR_EQUAL      23  //vyraz - rovno ==
#define I_EXPR_NOEQUAL    24  //vyraz - nerovno !=
#define I_START			      100 //navesti

//struktura pro instrukci
typedef struct
{
  int instType; //typ instrukce
  void *addr1;  //1.adresa
  void *addr2;  //2.adresa
  void *addr3;  //3.adresa
} tInstr;

//polozka seznamu
typedef struct listItem
{
  tInstr Instruction;
  struct listItem *nextItem;
} tListItem;

//struktura seznamu    
typedef struct
{
  struct listItem *first;  //ukazatel na prvni prvek
  struct listItem *last;   //ukazatel na posledni prvek
  struct listItem *active; //ukazatel na aktivni prvek
} tListOfInstr;

//funkce pro praci se seznamem
void listInit(tListOfInstr *L);                   //inicializace seznamu instrukci
void listFree(tListOfInstr *L);                   //uvolneni seznamu
void listInsertLast(tListOfInstr *L, tInstr I);   //vlozeni instrukce jako posledni prvek seznamu
void listFirst(tListOfInstr *L);                  //nastaveni seznamu na prvni prvek
void listNext(tListOfInstr *L);                   //aktivni instrukci se stane nasledujici instrukce
void listGoto(tListOfInstr *L, void *gotoInstr);  //nastavime aktivni instrukci podle zadaneho ukazatele
void *listGetPointerLast(tListOfInstr *L);        //vrati ukazatel na posledni instrukci
tInstr *listGetData(tListOfInstr *L);             //vrati aktivni instrukci
void listPrint(tListOfInstr *L);                  // vypise cely seznam
