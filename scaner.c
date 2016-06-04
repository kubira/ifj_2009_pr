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

// LEXIK�LN� ANALYZ�TOR
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "str.h"
#include "scaner.h"

//STAVY
#define START 0 // V�choz� stav
#define INT 1 // ��slice
#define ID_OR_WORD 2 // Identifik�tor nebo kl��ov� slovo
#define LESS_THAN 3  // Men��tko '<'
#define GREAT_THAN 4  // V�t��tko '>'
#define APOST_1 5  // Apostrof - za��tek nebo konec stringu
#define COMMENT 6  // Koment�� { ... }
#define DDOT 7  // Dvojte�ka ':'
#define UNDER 8  // Podtr��tko '_'
#define TECKA 9  // Te�ka v double
#define EXPONENT 10  // Exponent v double
#define ASCII 11  // Escape sekvence '#....'
#define ZNAMINKO 12  // Znam�nko v double
#define APOST_2 13  // Apostrof - konec stringu nebo za��tek escape sekvence
#define DOUBLE_E 14  // ��slo double s exponentem
#define DOUBLE 15  // ��slo double bez exponentu
#define L_COM1 16
#define L_COM2 17

// Prom�nn� pro ulo�en� vstupn�ho souboru
FILE *source;

// Funkce pro ulo�en� vstupn�ho souboru do prom�nn� source
void setSourceFile(FILE *f)
{
  source = f;
}

// Hlavni funkce lexik�ln�ho analyz�toru
int getNextToken(string *attr)
{
  // P�i prvn�m na�teni se nastav� stav (prom�nn� state) na hodnotu START - v�choz� stav
  int state = START;

  // Prom�nn� c - na��tan� znak
  int c;

  // Vyma�eme obsah atributu a v p��pad� identifik�toru do n�j budeme postupn�
  // vkl�dat jeho n�zev
  strClear(attr);

  // Cyklus, kter� prov�d�me do doby, dokud nena�teme EOF (konec souboru)
  // nebo dokud se nedostaneme do koncov�ho znaku. Ukon�en� (koncov� stav)
  // provedeme p��kazem return s dan�m koncov�m stavem
  while(1)
  {
    c = getc(source);  // Na�ten� znaku ze souboru
    // Dle stavu se rozhoduje, kter� case se bude prov�d�t (v jak�m stavu pr�v� jsme)
    switch(state)
    {
      case START :  // V�choz� stav START
        if(c == '/') {
          state = L_COM1;
        }
        else if(isspace(c)) state = START;  // B�l� znak - ignoruje se - z�st�v� ve stavu START
        else if(isdigit(c)) {  // ��slice
          strAddChar(attr, c);  // Vlo�en� znaku do atributu
          state = INT;  // Zm�na stavu na INT - cel� ��slo
        }
        else if(isalpha(c) || c == '_') // Identifik�tor nebo kl��ov� slovo
        {
          strAddChar(attr, c);  // Vlo�en� znaku do atributu
          state = ID_OR_WORD;  // Zm�na stavu na ID_OR_WORD - identifik�tor nebo kl��ov� slovo
        }
        else if(c == ';') return SEMICOLON;  // St�edn�k
        else if(c == '(') return LEFT_PARENTHESIS;  // Lev� kulat� z�vorka
        else if(c == ')') return RIGHT_PARENTHESIS;  // Prav� kulat� z�vorka
        else if(c == '+') return PLUS;  // Znak plus '+'
        else if(c == '-') return MINUS;  // Znak m�nus '-'
        else if(c == '*') return MUL;  // Znak hv�zdi�ka - n�soben� '*'
        else if(c == '.') return DOT;  // Znak te�ka '.'
        else if(c == ',') return STROKE;  // Znak ��rka ','
        else if(c == '=') return EQUAL;  // Znak rovn�tko '='
        else if(c == '<') state = LESS_THAN;  // Zm�na stavu na LESS_THAN - men�� nebo men�� rovno
        else if(c == '>') state = GREAT_THAN;  // Zm�na stavu na GREAT_THAN - v�t�� nebo v�t�� rovno
        else if(c == 39) {  // Apostrof
          strAddChar(attr, c);  // Vlo�en� znaku do atributu
          state = APOST_1;  // Zm�na stavu na APOST_1 - za��tek stringu
        }
        else if(c == '{') state = COMMENT;  // Zm�na stavu na COMMENT - koment��
        else if(c == ':') state = DDOT;  // Zm�na stavu na DDOT - dvojte�ka nebo p�i�azen�
        else if(c == EOF) return END_OF_FILE;  // Konec souboru
		else  {// Nebyla jin� mo�nost - lexik�ln� chyba
        return LEX_ERROR;
		}
        break;  // Konec v�tve START
      
      case INT :  // Stav INT - cel� ��slo
        if(isdigit(c)) {  // Pokud je znak ��slice
          strAddChar(attr, c);  // P�id�n� znaku do atributu
        }
        else if(c == '.') {  // Pokud je znak te�ka
          strAddChar(attr, c);  // P�id�n� znaku do atributu
          state = TECKA;  // Zm�na stavu na TECKA - te�ka v double
        }
        else if(c == 'e' || c == 'E') {  // Pokud je znak 'e' nebo 'E'
          strAddChar(attr, c);  // P�id�n� znaku do atributu
          state = EXPONENT;  // Zm�na stavu na EXPONENT - exponent v double
        }
        else {  // Pokud je znak n�co jin�ho
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
          return RET_INT;  // �et�zec v attr je cel� ��slo
        }
        break;  // Konec v�tve INT
      
      case ID_OR_WORD :  // Stav ID_OR_WORD - identifik�tor nebo kl��ov� slovo
          if (isalnum(c))  // Pokud je znak p�smeno nebo ��slice
			strAddChar(attr, c);  // Identifik�tor pokra�uje - p�id�me znak do atributu
	      else // Konec identifik�toru - na�ten jin� znak ne� p�smeno nebo ��slice
	      {
       	  ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak

	        // Kontrola, zda-li se nejedn� o kl��ov� slovo
	        if (strCmpConstStr(attr, "begin") == 0) return BEGIN;  // Kl��ov� slovo begin
	        else if (strCmpConstStr(attr, "div") == 0) return DIV;  // Oper�tor d�len�
	        else if (strCmpConstStr(attr, "do") == 0) return DO;  // Kl��ov� slovo do
	        else if (strCmpConstStr(attr, "double") == 0) return KDOUBLE;  // Kl��ov� slovo double
	        else if (strCmpConstStr(attr, "else") == 0) return ELSE;  // Kl��ov� slovo else
	        else if (strCmpConstStr(attr, "end") == 0) return END;  // Kl��ov� slovo end
	        else if (strCmpConstStr(attr, "find") == 0) return FIND;  // Kl��ov� slovo find
	        else if (strCmpConstStr(attr, "if") == 0) return IF;  // Kl��ov� slovo begin
	        else if (strCmpConstStr(attr, "integer") == 0) return INTEGER;  // Kl��ov� slovo integer
	        else if (strCmpConstStr(attr, "readln") == 0) return READLN;  // Kl��ov� slovo readln
	        else if (strCmpConstStr(attr, "sort") == 0) return SORT;  // Kl��ov� slovo sort
	        else if (strCmpConstStr(attr, "string") == 0) return STRING;  // Kl��ov� slovo string
	        else if (strCmpConstStr(attr, "then") == 0) return THEN;  // Kl��ov� slovo then
	        else if (strCmpConstStr(attr, "var") == 0) return VAR;  // Kl��ov� slovo var
	        else if (strCmpConstStr(attr, "while") == 0) return WHILE;  // Kl��ov� slovo while
	        else if (strCmpConstStr(attr, "write") == 0) return WRITE;  // Kl��ov� slovo write
	        else return ID; // Byl to identifik�tor
        }
        break;  // Konec v�tve ID_OR_WORD
      
      case LESS_THAN :  // Stav LESS_THAN - men��tko '<'
		  if(c == '>') return NOTEQUAL;  // Dvojznak nerovno '<>'
        else if(c == '=') return LTOREQUAL;  // Dvojznak men��-rovno '<='
        else  // N�co jin�ho
        {
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
          return LESSTHAN;  // Znak men��tko '<'
        }
        break;  // Konec v�tve LESS_THAN
      
      case GREAT_THAN :  // Stav GREAT_THAN - v�t��tko '>'
        if(c == '=') return GTOREQUAL;  // Dvojznak v�t��-rovno - '>='
        else  // N�co jin�ho
        {
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
          return GREATTHAN;  // Znak v�t��tko '>'
        }
        break;  // Konec v�tve GREAT_THAN
      
      case APOST_1 :  // Stav APOST_1 - za��tek nebo konec stringu

		  if(c < 32) {  // Pokud m� znak ordin�ln� hodnotu men�� ne� 32
         return LEX_ERROR;  // Lexik�ln� chyba
		  }
		  else if(c == EOF) {  // Pokud je znak konec souboru
			return LEX_ERROR;  // Lexik�ln� chyba
		  }
		  else if(c == 39) {  // Pokud je znak apostrof
			  strAddChar(attr, c);  // P�id�m znak do atributu
			  state = APOST_2;  // Zm�n�m stav na APOST_2 - escape sekvence nebo konec stringu
		  }
          else {  // N�co jin�ho
			strAddChar(attr, c);  // P�id�m znak do atributu
          }
          break;  // Konec v�tve APOST_1
      
      case APOST_2 :  // Stav APOST_2 - escape sekvence nebo konec stringu
        if(c == 39) {  // Pokud je znak apostrof
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = APOST_1;  // Zm�n�m stav na APOST_1
        }
        else if(c == '#') {  // Pokud je znak m��ka '#'
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = ASCII;  // Zm�n�m stav na ASCII - escape sekvence
        }
        else {  // N�co jin�ho
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
		  if(nahrazeni(attr) == LEX_ERROR) {  // Nahrad�m escape sekvence, odstran�m aspostrofy a kontrola
			  return LEX_ERROR;  // Lexik�ln� chyba
		  }
          else return RET_STRING;  // V �et�zci attr je string
        }
        break;  // Konec v�tve APOST_2
      
      case ASCII :  // Stav ASCII - escape sekvence
        if(isdigit(c)) {  // Pokud je znak ��slice
          strAddChar(attr, c);  // P�id�m znak do atributu
        }
        else if(c == 39) {  // Pokud je znak apostrof
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = APOST_1;  // Zm�n�m stav na APOST_1
        }
        else  // N�co jin�ho
          return LEX_ERROR;  // Lexik�ln� chyba
        break;  // Konec v�tve ASCII
      
      case COMMENT :  // Stav COMMENT - koment��
		  if(c == EOF) {  // Pokud je znak konec souboru
			return LEX_ERROR;  // Lexik�ln� chyba
		  }
          else if(c == '}')  // Pokud byl znak prav� slo�en� z�vorka
			state = START;  // Zm�n�m stav na START - v�choz� stav
          break;  // Konec v�tve COMMENT
      
      case DDOT :  // Stav DDOT - dvojte�ka
        if(c == '=') return ASSIGN;  // Dvojznak p�i�azen� ':='
        else  // N�co jin�ho
        {
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
          return RET_DDOT;  // Znak dvojte�ka ':'
        }
        break;  // Konec v�tve DDOT
      
      case TECKA :  // Stav TECKA - te�ka v double
        if(isdigit(c))  // Pokud je znak ��slice
        {
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = DOUBLE;  // Zm�n�m stav na DOUBLE
        }
        else  // N�co jin�ho
          return LEX_ERROR;  // Lexik�ln� chyba
        break;  // Konec v�tve TECKA
      
      case EXPONENT :  // Stav EXPONENT - exponent v double
        if(isdigit(c)) {  // Pokud je znak ��slice
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = DOUBLE_E;  // Zm�n�m stav na DOUBLE_E - double s exponentem
        }
        else if(c == '+') {  // Pokud je znak plus '+'
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = ZNAMINKO;  // Zm�n�m stav na ZNAMINKO - double s exponentem, kter� m� znam�nko
        }
        else if(c == '-') {  // Pokud je znak m�nus '-'
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = ZNAMINKO;  // Zm�n�m stav na ZNAMINKO - double s exponentem, kter� m� znam�nko
        }
		else  // N�co jin�ho
			return LEX_ERROR;  // Lexik�ln� chyba
        break;  // Konec v�tve EXPONENT
        
      case ZNAMINKO :  // Stav ZNAMINKO - double s exponentem, kter� m� znam�nko
        if(isdigit(c)) {  // Pokud je znak ��slice
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = DOUBLE_E;  // Zm�n�m stav na DOUBLE_E - double s exponentem
        }
        else  // N�co jin�ho
          return LEX_ERROR;  // Lexik�ln� chyba
        break;  // Konec v�tve ZNAMINKO
      
      case DOUBLE :  // Stav DOUBLE - desetinn� ��slo
        if(isdigit(c)) {  // Pokud je znak ��slice
          strAddChar(attr, c);  // P�id�m znak do atributu
        }
        else if(c == 'e' || c == 'E') {  // Pokud je znak 'e' nebo 'E'
          strAddChar(attr, c);  // P�id�m znak do atributu
          state = EXPONENT;  // Zm�n�m stav na EXPONENT - double s exponentem
        }
        else  // N�co jin�ho
        {
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
          return RET_DOUBLE;  // V atributu je desetinn� ��slo
        }
        break;  // Konec v�tve DOUBLE
        
      case DOUBLE_E :  // Stav DOUBLE_E - double s exponentem
        if(isdigit(c)) {  // Pokud je znak ��slice
          strAddChar(attr, c);  // P�id�m znak do atributu
        }
        else {  // N�co jin�ho
          ungetc(c, source);  // POZOR! Je pot�eba vr�tit posledn� na�ten� znak
          return RET_DOUBLE;  // V atributu je desetinn� ��slo
        }
        break;  // Konec v�tve DOUBLE_E
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
// Funkce pro nahrazen� ASCII hodnoty a apostrof�
{
	string novy;  // �et�zec, do kter�ho ukl�d�m v�sledn� �et�zec
	int delka = s1->length-1;  // Zjist�m si d�lku p�vodn�ho �et�zce pro cyklus
	                           // a ode�tu 1, proto�e je na konci apostrof
	int cislo = 0;  // Prom�nn� pro ASCII hodnotu za m��kou
	int pozice;  // Prom�nn� pro proch�zen� p�vodn�ho �et�zce
	string a;  // �et�zec, do kter�ho ukl�d�m ��sla ASCII hodnoty za m��kou

	if(strInit(&a) == STR_ERROR)  // Inicializace �et�zce a a kontrola
    return STR_ERROR;  // Vrac�m chybu inicializace

	if(strInit(&novy) == STR_ERROR)  // Inicializace �et�zce novy a kontrola
	{
	  strFree(&a);  // Uvoln�m pam� �et�zce a

    return STR_ERROR;  // Vrac�m chybu inicializace
  }

  for(pozice = 1; pozice < delka; pozice++) {// Cyklus proch�zej�c� �et�zec
	  if (s1->str[pozice] < 32) {
		  return LEX_ERROR;
	  }
    
	  if(s1->str[pozice] == '\'') // Pokud je na pozici apostrof
	  {
			pozice++;  // Posuneme se na dal�� pozici
			
			if(s1->str[pozice] == '\'')   // Pokud je na pozici apostrof
      {
				if(strAddChar(&novy, '\'') == STR_ERROR)
        // P�id�m do nov�ho �et�zce apostrof a kontrola
				{
				  strFree(&a);  // Uvoln�m pam� �et�zce a
				  strFree(&novy);  // Uvoln�m pam� �et�zce novy

          return STR_ERROR;  // Vrac�m chybu p�id�n� znaku
        }
			} 
      else if(s1->str[pozice] == '#') // Pokud je na pozici m��ka
      {
				pozice++;  // Posuneme se na dal�� pozici

				while(isdigit(s1->str[pozice]) != 0) // Na��t�me ��slice za m��kou
        {
					if(strAddChar(&a, s1->str[pozice]) == STR_ERROR)
          // P�id�v�me ��slici do �et�zce a a kontrola
					{
            strFree(&a);  // Uvoln�m pam� �et�zce a
				    strFree(&novy);  // Uvoln�m pam� �et�zce novy

            return STR_ERROR;  // Vrac�m chybu p�id�n� znaku
          }
          
					pozice++;  // Posuneme se na dal�� pozici
				}

				cislo = atoi(strGetStr(&a));  // P�evedeme ��sla za m��kou na ��slo

				if(!(cislo <= 31 && cislo >= 1))  // Pokud je ��slo v�t�� ne� 31 -> CHYBA!!!
        {
					strFree(&a);  // Uvoln�me pam� po �et�zci a
					strFree(&novy);  // Uvoln�me pam� po �et�zci novy

					return LEX_ERROR;  // Vrac� se chyba ASCII znaku
				}

				if(strAddChar(&novy, (char)cislo) == STR_ERROR)
        // P�id�me do nov�ho �et�zce ASCII znak a kontrola
				{
          strFree(&a);  // Uvoln�m pam� �et�zce a
				  strFree(&novy);  // Uvoln�m pam� �et�zce novy

          return STR_ERROR;  // Vrac�m chybu p�id�n� znaku
        }
        
				cislo = 0;  // Nastav�me ��slo na 0 pro dal�� pou�it�
				strClear(&a);  // Vyma�eme obsah �et�zce a pro dal�� pou�it�

				if(s1->str[pozice] == '\'') {  // Pokud je na dal�� pozici apostrof
					continue;  // Pokra�ujeme
				}
        else // Jinak je to chyba
        {
					printf("CHYBA2!!!\n");  // Vyp�eme hl�en�
					strFree(&a);  // Uvoln�me pam� po �et�zci a
					strFree(&novy);  // Uvoln�me pam� po �et�zci novy
					
					return STR_ERROR;  // Vrac� se chyba �et�zce
				}
			}
		}
    else // Pokud nen� na pozici apostrof, ulo��me znak do nov�ho �et�zce
    {
			if(strAddChar(&novy, s1->str[pozice]) == STR_ERROR)
      // Ulo��me znak do �et�zce novy a kontrola
			{
        strFree(&a);  // Uvoln�m pam� �et�zce a
				strFree(&novy);  // Uvoln�m pam� �et�zce novy

        return STR_ERROR;  // Vrac�m chybu p�id�n� znaku
      }
		}
	}

  strClear(s1);  // Vyma�eme p�vodn� �et�zec

	if(strCopyString(s1, &novy) == STR_ERROR)
  // Zkop�rujeme nov� �et�zec do p�vodn�ho a kontrola
	{
    strFree(&a);  // Uvoln�m pam� �et�zce a
		strFree(&novy);  // Uvoln�m pam� �et�zce novy

    return STR_ERROR;  // Vrac�m chybu p�id�n� znaku
  }
  
	strFree(&a);  // Uvoln�me pam� po �et�zci a
	strFree(&novy);  // Uvoln�me pam� po �et�zci novy
	
	return STR_SUCCESS;  // �sp�n� nahrazen� ASCII hodnot a apostrof�
}
