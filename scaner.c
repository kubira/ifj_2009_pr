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

// LEXIKÁLNÍ ANALYZÁTOR
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "str.h"
#include "scaner.h"

//STAVY
#define START 0 // Výchozí stav
#define INT 1 // Èíslice
#define ID_OR_WORD 2 // Identifikátor nebo klíèové slovo
#define LESS_THAN 3  // Men‘ítko '<'
#define GREAT_THAN 4  // Vìt‘ítko '>'
#define APOST_1 5  // Apostrof - zaèátek nebo konec stringu
#define COMMENT 6  // Komentáø { ... }
#define DDOT 7  // Dvojteèka ':'
#define UNDER 8  // Podtržítko '_'
#define TECKA 9  // Teèka v double
#define EXPONENT 10  // Exponent v double
#define ASCII 11  // Escape sekvence '#....'
#define ZNAMINKO 12  // Znaménko v double
#define APOST_2 13  // Apostrof - konec stringu nebo zaèátek escape sekvence
#define DOUBLE_E 14  // Èíslo double s exponentem
#define DOUBLE 15  // Èíslo double bez exponentu
#define L_COM1 16
#define L_COM2 17

// Promìnná pro uložení vstupního souboru
FILE *source;

// Funkce pro uložení vstupního souboru do promìnné source
void setSourceFile(FILE *f)
{
  source = f;
}

// Hlavni funkce lexikálního analyzátoru
int getNextToken(string *attr)
{
  // Pøi prvním naèteni se nastaví stav (promìnná state) na hodnotu START - výchozí stav
  int state = START;

  // Promìnná c - naèítaný znak
  int c;

  // Vymažeme obsah atributu a v pøípadì identifikátoru do nìj budeme postupnì
  // vkládat jeho název
  strClear(attr);

  // Cyklus, který provádíme do doby, dokud nenaèteme EOF (konec souboru)
  // nebo dokud se nedostaneme do koncového znaku. Ukonèení (koncový stav)
  // provedeme pøíkazem return s daným koncovým stavem
  while(1)
  {
    c = getc(source);  // Naètení znaku ze souboru
    // Dle stavu se rozhoduje, který case se bude provádìt (v jakém stavu právì jsme)
    switch(state)
    {
      case START :  // Výchozí stav START
        if(c == '/') {
          state = L_COM1;
        }
        else if(isspace(c)) state = START;  // Bílý znak - ignoruje se - zùstává ve stavu START
        else if(isdigit(c)) {  // Èíslice
          strAddChar(attr, c);  // Vložení znaku do atributu
          state = INT;  // Zmìna stavu na INT - celé èíslo
        }
        else if(isalpha(c) || c == '_') // Identifikátor nebo klíèové slovo
        {
          strAddChar(attr, c);  // Vložení znaku do atributu
          state = ID_OR_WORD;  // Zmìna stavu na ID_OR_WORD - identifikátor nebo klíèové slovo
        }
        else if(c == ';') return SEMICOLON;  // Støedník
        else if(c == '(') return LEFT_PARENTHESIS;  // Levá kulatá závorka
        else if(c == ')') return RIGHT_PARENTHESIS;  // Pravá kulatá závorka
        else if(c == '+') return PLUS;  // Znak plus '+'
        else if(c == '-') return MINUS;  // Znak mínus '-'
        else if(c == '*') return MUL;  // Znak hvìzdièka - násobení '*'
        else if(c == '.') return DOT;  // Znak teèka '.'
        else if(c == ',') return STROKE;  // Znak èárka ','
        else if(c == '=') return EQUAL;  // Znak rovnítko '='
        else if(c == '<') state = LESS_THAN;  // Zmìna stavu na LESS_THAN - men‘í nebo men‘í rovno
        else if(c == '>') state = GREAT_THAN;  // Zmìna stavu na GREAT_THAN - vìt‘í nebo vìt‘í rovno
        else if(c == 39) {  // Apostrof
          strAddChar(attr, c);  // Vložení znaku do atributu
          state = APOST_1;  // Zmìna stavu na APOST_1 - zaèátek stringu
        }
        else if(c == '{') state = COMMENT;  // Zmìna stavu na COMMENT - komentáø
        else if(c == ':') state = DDOT;  // Zmìna stavu na DDOT - dvojteèka nebo pøiøazení
        else if(c == EOF) return END_OF_FILE;  // Konec souboru
		else  {// Nebyla jiná možnost - lexikální chyba
        return LEX_ERROR;
		}
        break;  // Konec vìtve START
      
      case INT :  // Stav INT - celé èíslo
        if(isdigit(c)) {  // Pokud je znak èíslice
          strAddChar(attr, c);  // Pøidání znaku do atributu
        }
        else if(c == '.') {  // Pokud je znak teèka
          strAddChar(attr, c);  // Pøidání znaku do atributu
          state = TECKA;  // Zmìna stavu na TECKA - teèka v double
        }
        else if(c == 'e' || c == 'E') {  // Pokud je znak 'e' nebo 'E'
          strAddChar(attr, c);  // Pøidání znaku do atributu
          state = EXPONENT;  // Zmìna stavu na EXPONENT - exponent v double
        }
        else {  // Pokud je znak nìco jiného
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
          return RET_INT;  // Øetìzec v attr je celé èíslo
        }
        break;  // Konec vìtve INT
      
      case ID_OR_WORD :  // Stav ID_OR_WORD - identifikátor nebo klíèové slovo
          if (isalnum(c))  // Pokud je znak písmeno nebo èíslice
			strAddChar(attr, c);  // Identifikátor pokraèuje - pøidáme znak do atributu
	      else // Konec identifikátoru - naèten jiný znak než písmeno nebo èíslice
	      {
       	  ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak

	        // Kontrola, zda-li se nejedná o klíèové slovo
	        if (strCmpConstStr(attr, "begin") == 0) return BEGIN;  // Klíèové slovo begin
	        else if (strCmpConstStr(attr, "div") == 0) return DIV;  // Operátor dìlení
	        else if (strCmpConstStr(attr, "do") == 0) return DO;  // Klíèové slovo do
	        else if (strCmpConstStr(attr, "double") == 0) return KDOUBLE;  // Klíèové slovo double
	        else if (strCmpConstStr(attr, "else") == 0) return ELSE;  // Klíèové slovo else
	        else if (strCmpConstStr(attr, "end") == 0) return END;  // Klíèové slovo end
	        else if (strCmpConstStr(attr, "find") == 0) return FIND;  // Klíèové slovo find
	        else if (strCmpConstStr(attr, "if") == 0) return IF;  // Klíèové slovo begin
	        else if (strCmpConstStr(attr, "integer") == 0) return INTEGER;  // Klíèové slovo integer
	        else if (strCmpConstStr(attr, "readln") == 0) return READLN;  // Klíèové slovo readln
	        else if (strCmpConstStr(attr, "sort") == 0) return SORT;  // Klíèové slovo sort
	        else if (strCmpConstStr(attr, "string") == 0) return STRING;  // Klíèové slovo string
	        else if (strCmpConstStr(attr, "then") == 0) return THEN;  // Klíèové slovo then
	        else if (strCmpConstStr(attr, "var") == 0) return VAR;  // Klíèové slovo var
	        else if (strCmpConstStr(attr, "while") == 0) return WHILE;  // Klíèové slovo while
	        else if (strCmpConstStr(attr, "write") == 0) return WRITE;  // Klíèové slovo write
	        else return ID; // Byl to identifikátor
        }
        break;  // Konec vìtve ID_OR_WORD
      
      case LESS_THAN :  // Stav LESS_THAN - men‘ítko '<'
		  if(c == '>') return NOTEQUAL;  // Dvojznak nerovno '<>'
        else if(c == '=') return LTOREQUAL;  // Dvojznak men‘í-rovno '<='
        else  // Nìco jiného
        {
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
          return LESSTHAN;  // Znak men‘ítko '<'
        }
        break;  // Konec vìtve LESS_THAN
      
      case GREAT_THAN :  // Stav GREAT_THAN - vìt‘ítko '>'
        if(c == '=') return GTOREQUAL;  // Dvojznak vìt‘í-rovno - '>='
        else  // Nìco jiného
        {
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
          return GREATTHAN;  // Znak vìt‘ítko '>'
        }
        break;  // Konec vìtve GREAT_THAN
      
      case APOST_1 :  // Stav APOST_1 - zaèátek nebo konec stringu

		  if(c < 32) {  // Pokud má znak ordinální hodnotu men‘í než 32
         return LEX_ERROR;  // Lexikální chyba
		  }
		  else if(c == EOF) {  // Pokud je znak konec souboru
			return LEX_ERROR;  // Lexikální chyba
		  }
		  else if(c == 39) {  // Pokud je znak apostrof
			  strAddChar(attr, c);  // Pøidám znak do atributu
			  state = APOST_2;  // Zmìním stav na APOST_2 - escape sekvence nebo konec stringu
		  }
          else {  // Nìco jiného
			strAddChar(attr, c);  // Pøidám znak do atributu
          }
          break;  // Konec vìtve APOST_1
      
      case APOST_2 :  // Stav APOST_2 - escape sekvence nebo konec stringu
        if(c == 39) {  // Pokud je znak apostrof
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = APOST_1;  // Zmìním stav na APOST_1
        }
        else if(c == '#') {  // Pokud je znak møížka '#'
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = ASCII;  // Zmìním stav na ASCII - escape sekvence
        }
        else {  // Nìco jiného
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
		  if(nahrazeni(attr) == LEX_ERROR) {  // Nahradím escape sekvence, odstraním aspostrofy a kontrola
			  return LEX_ERROR;  // Lexikální chyba
		  }
          else return RET_STRING;  // V øetìzci attr je string
        }
        break;  // Konec vìtve APOST_2
      
      case ASCII :  // Stav ASCII - escape sekvence
        if(isdigit(c)) {  // Pokud je znak èíslice
          strAddChar(attr, c);  // Pøidám znak do atributu
        }
        else if(c == 39) {  // Pokud je znak apostrof
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = APOST_1;  // Zmìním stav na APOST_1
        }
        else  // Nìco jiného
          return LEX_ERROR;  // Lexikální chyba
        break;  // Konec vìtve ASCII
      
      case COMMENT :  // Stav COMMENT - komentáø
		  if(c == EOF) {  // Pokud je znak konec souboru
			return LEX_ERROR;  // Lexikální chyba
		  }
          else if(c == '}')  // Pokud byl znak pravá složená závorka
			state = START;  // Zmìním stav na START - výchozí stav
          break;  // Konec vìtve COMMENT
      
      case DDOT :  // Stav DDOT - dvojteèka
        if(c == '=') return ASSIGN;  // Dvojznak pøiøazení ':='
        else  // Nìco jiného
        {
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
          return RET_DDOT;  // Znak dvojteèka ':'
        }
        break;  // Konec vìtve DDOT
      
      case TECKA :  // Stav TECKA - teèka v double
        if(isdigit(c))  // Pokud je znak èíslice
        {
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = DOUBLE;  // Zmìním stav na DOUBLE
        }
        else  // Nìco jiného
          return LEX_ERROR;  // Lexikální chyba
        break;  // Konec vìtve TECKA
      
      case EXPONENT :  // Stav EXPONENT - exponent v double
        if(isdigit(c)) {  // Pokud je znak èíslice
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = DOUBLE_E;  // Zmìním stav na DOUBLE_E - double s exponentem
        }
        else if(c == '+') {  // Pokud je znak plus '+'
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = ZNAMINKO;  // Zmìním stav na ZNAMINKO - double s exponentem, který má znaménko
        }
        else if(c == '-') {  // Pokud je znak mínus '-'
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = ZNAMINKO;  // Zmìním stav na ZNAMINKO - double s exponentem, který má znaménko
        }
		else  // Nìco jiného
			return LEX_ERROR;  // Lexikální chyba
        break;  // Konec vìtve EXPONENT
        
      case ZNAMINKO :  // Stav ZNAMINKO - double s exponentem, který má znaménko
        if(isdigit(c)) {  // Pokud je znak èíslice
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = DOUBLE_E;  // Zmìním stav na DOUBLE_E - double s exponentem
        }
        else  // Nìco jiného
          return LEX_ERROR;  // Lexikální chyba
        break;  // Konec vìtve ZNAMINKO
      
      case DOUBLE :  // Stav DOUBLE - desetinné èíslo
        if(isdigit(c)) {  // Pokud je znak èíslice
          strAddChar(attr, c);  // Pøidám znak do atributu
        }
        else if(c == 'e' || c == 'E') {  // Pokud je znak 'e' nebo 'E'
          strAddChar(attr, c);  // Pøidám znak do atributu
          state = EXPONENT;  // Zmìním stav na EXPONENT - double s exponentem
        }
        else  // Nìco jiného
        {
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
          return RET_DOUBLE;  // V atributu je desetinné èíslo
        }
        break;  // Konec vìtve DOUBLE
        
      case DOUBLE_E :  // Stav DOUBLE_E - double s exponentem
        if(isdigit(c)) {  // Pokud je znak èíslice
          strAddChar(attr, c);  // Pøidám znak do atributu
        }
        else {  // Nìco jiného
          ungetc(c, source);  // POZOR! Je potøeba vrátit poslední naètený znak
          return RET_DOUBLE;  // V atributu je desetinné èíslo
        }
        break;  // Konec vìtve DOUBLE_E
      case L_COM1 :
        if(c == '/') {
          state = L_COM2;
        }
        else return LEX_ERROR;
        break;
      case L_COM2 :
        if(c == '\n') {
          state = START;
        } else if(c == EOF) {
          return LEX_ERROR;
        }
        break;
    } // Konec switche
  } // Konec whilu
} // Konec funkce getNextToken

int nahrazeni(string *s1)
// Funkce pro nahrazení ASCII hodnoty a apostrofù
{
	string novy;  // Øetìzec, do kterého ukládám výsledný øetìzec
	int delka = s1->length-1;  // Zjistím si délku pùvodního øetìzce pro cyklus
	                           // a odeètu 1, proto¾e je na konci apostrof
	int cislo = 0;  // Promìnná pro ASCII hodnotu za møí¾kou
	int pozice;  // Promìnná pro procházení pùvodního øetìzce
	string a;  // Øetìzec, do kterého ukládám èísla ASCII hodnoty za møí¾kou

	if(strInit(&a) == STR_ERROR)  // Inicializace øetìzce a a kontrola
    return STR_ERROR;  // Vracím chybu inicializace

	if(strInit(&novy) == STR_ERROR)  // Inicializace øetìzce novy a kontrola
	{
	  strFree(&a);  // Uvolním pamì» øetìzce a

    return STR_ERROR;  // Vracím chybu inicializace
  }

  for(pozice = 1; pozice < delka; pozice++) {// Cyklus procházející øetìzec
	  if (s1->str[pozice] < 32) {
		  return LEX_ERROR;
	  }
    
	  if(s1->str[pozice] == '\'') // Pokud je na pozici apostrof
	  {
			pozice++;  // Posuneme se na dal¹í pozici
			
			if(s1->str[pozice] == '\'')   // Pokud je na pozici apostrof
      {
				if(strAddChar(&novy, '\'') == STR_ERROR)
        // Pøidám do nového øetìzce apostrof a kontrola
				{
				  strFree(&a);  // Uvolním pamì» øetìzce a
				  strFree(&novy);  // Uvolním pamì» øetìzce novy

          return STR_ERROR;  // Vracím chybu pøidání znaku
        }
			} 
      else if(s1->str[pozice] == '#') // Pokud je na pozici møí¾ka
      {
				pozice++;  // Posuneme se na dal¹í pozici

				while(isdigit(s1->str[pozice]) != 0) // Naèítáme èíslice za møí¾kou
        {
					if(strAddChar(&a, s1->str[pozice]) == STR_ERROR)
          // Pøidáváme èíslici do øetìzce a a kontrola
					{
            strFree(&a);  // Uvolním pamì» øetìzce a
				    strFree(&novy);  // Uvolním pamì» øetìzce novy

            return STR_ERROR;  // Vracím chybu pøidání znaku
          }
          
					pozice++;  // Posuneme se na dal¹í pozici
				}

				cislo = atoi(strGetStr(&a));  // Pøevedeme èísla za møí¾kou na èíslo

				if(!(cislo <= 31 && cislo >= 1))  // Pokud je èíslo vìt¹í ne¾ 31 -> CHYBA!!!
        {
					strFree(&a);  // Uvolníme pamì» po øetìzci a
					strFree(&novy);  // Uvolníme pamì» po øetìzci novy

					return LEX_ERROR;  // Vrací se chyba ASCII znaku
				}

				if(strAddChar(&novy, (char)cislo) == STR_ERROR)
        // Pøidáme do nového øetìzce ASCII znak a kontrola
				{
          strFree(&a);  // Uvolním pamì» øetìzce a
				  strFree(&novy);  // Uvolním pamì» øetìzce novy

          return STR_ERROR;  // Vracím chybu pøidání znaku
        }
        
				cislo = 0;  // Nastavíme èíslo na 0 pro dal¹í pou¾ití
				strClear(&a);  // Vyma¾eme obsah øetìzce a pro dal¹í pou¾ití

				if(s1->str[pozice] == '\'') {  // Pokud je na dal¹í pozici apostrof
					continue;  // Pokraèujeme
				}
        else // Jinak je to chyba
        {
					printf("CHYBA2!!!\n");  // Vypí¹eme hlá¹ení
					strFree(&a);  // Uvolníme pamì» po øetìzci a
					strFree(&novy);  // Uvolníme pamì» po øetìzci novy
					
					return STR_ERROR;  // Vrací se chyba øetìzce
				}
			}
		}
    else // Pokud není na pozici apostrof, ulo¾íme znak do nového øetìzce
    {
			if(strAddChar(&novy, s1->str[pozice]) == STR_ERROR)
      // Ulo¾íme znak do øetìzce novy a kontrola
			{
        strFree(&a);  // Uvolním pamì» øetìzce a
				strFree(&novy);  // Uvolním pamì» øetìzce novy

        return STR_ERROR;  // Vracím chybu pøidání znaku
      }
		}
	}

  strClear(s1);  // Vyma¾eme pùvodní øetìzec

	if(strCopyString(s1, &novy) == STR_ERROR)
  // Zkopírujeme nový øetìzec do pùvodního a kontrola
	{
    strFree(&a);  // Uvolním pamì» øetìzce a
		strFree(&novy);  // Uvolním pamì» øetìzce novy

    return STR_ERROR;  // Vracím chybu pøidání znaku
  }
  
	strFree(&a);  // Uvolníme pamì» po øetìzci a
	strFree(&novy);  // Uvolníme pamì» po øetìzci novy
	
	return STR_SUCCESS;  // Úspì¹né nahrazení ASCII hodnot a apostrofù
}
