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

//sa_parser.c

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "str.h"
#include "bvs.h"
#include "ilist.h"
#include "stack.h"
#include "scaner.h"
#include "sa_parser.h"

char *pole[17];
char *znak[17];
char *pravidla[15];

//Pole s vybranýmy lexémy
void udelejZnak() {
  znak[ID]    = "I";
  znak[PLUS]  = "+";
  znak[MINUS] = "-";
  znak[MUL]   = "*";
  znak[DIV]   = "/";
  znak[LEFT_PARENTHESIS]  = "(";
  znak[RIGHT_PARENTHESIS] = ")";
  znak[7]         = "f";
  znak[STROKE]    = ",";
  znak[LESSTHAN]  = "<";
  znak[GREATTHAN] = ">";
  znak[LTOREQUAL] = "<=";
  znak[GTOREQUAL] = ">=";
  znak[EQUAL]    = "==";
  znak[NOTEQUAL] = "!=";
  znak[ASSIGN]   = "=";
  znak[DOLLAR]   = "$";
}
//Pravidla pro redukci výrazu
void udelejPravidla() {
  pravidla[0] = "E+E";
  pravidla[1] = "E-E";
  pravidla[2] = "E*E";
  pravidla[3] = "E/E";
  pravidla[4] = ")E(";
  pravidla[5] = ")E(f";
  pravidla[6] = ")E,E(f";
  pravidla[7] = "E=E";
  pravidla[8] = "E==E";
  pravidla[9] = "E!=E";
  pravidla[10] = "E>E";
  pravidla[11] = "E<E";
  pravidla[12] = "E>=E";
  pravidla[13] = "E<=E";
  pravidla[14] = "I";
}

//Precedenèní tabulka
void udelejPole() {
  pole[ID]                = " >>>> > >>>>>>>>>";
  pole[PLUS]              = "|>>|||>|>>>>>>>>>";
  pole[MINUS]             = "|>>|||>|>>>>>>>>>";
  pole[MUL]               = "|>>>>|>|>>>>>>>>>";
  pole[DIV]               = "|>>>>|>|>>>>>>>>>";
  pole[LEFT_PARENTHESIS]  = "||||||=|=||||||| ";
  pole[RIGHT_PARENTHESIS] = " >>>> > >>>>>>>>>";
  pole[7]                 = "     =           ";  //Funkce
  pole[STROKE]            = "||||||=| ||||||| ";
  pole[LESSTHAN]          = "||||||>|>>>>>>>>>";
  pole[GREATTHAN]         = "||||||>|>>>>>>>>>";
  pole[LTOREQUAL]         = "||||||>|>>>>>>>>>";
  pole[GTOREQUAL]         = "||||||>|>>>>>>>>>";
  pole[EQUAL]             = "||||||>|>||||>>>>";
  pole[NOTEQUAL]          = "||||||>|>||||>>>>";
  pole[ASSIGN]            = "||||||>|>|||||||>";
  pole[DOLLAR]            = "|||||| | ||||||| ";
}

//Hlavní funkce výrazového analyzátoru
int sa_parser(int *token, string *atrib, tSymbolTable *table, tListOfInstr *list)
{
  //Pomocné promìnné a datové struktury
  stack a;  // Zasobnik
  string pravidlo, pom, pomocna, vysledek;  // Retezce
  int g;  // Index v poli pravidel
  unsigned int u;  // Index v retezci lexemu
  int pravda = TRUE;  // Pomocna promenna - nastavuje se pri hledani pravidla
  char *str;
  tInstr *instrukce;  // Ukazatel na novou instrukci
  tTableItem *loa = NULL;  // Ukazatel na vysledek posledni operace
  stackItemPtr h;  // Pomocny prvek zasobniku
  int funkce;  // Prisla na vstupu funkce?
  tTableItem *ad1 = NULL;  // Ukazatel 1 pro triadresny kod
  tTableItem *ad2 = NULL;  // Ukazatel 2 pro triadresny kod
  int tokenLast = 0;  // Posledni token
  int write = 0;  // Byla volana funkce write?

  // Vytvoreni poli
  udelejPole();
  udelejZnak();
  udelejPravidla();

  //Inicializace datových struktur
  stackInit(&a);
  strInit(&pravidlo);
  strInit(&pomocna);
  strInit(&pom);
  strInit(&vysledek);

/*****************************************************************************/
  do {
	  if(*token == WRITE) {  // Pokud je token WRITE - funkce write s ID
		  write = 1;  // Nastavim priznak
		  *token = *token - WRITE;  // Nastavim token na ID (0)
	  } else if(*token > 200 && *token < 300) {
	    // Pokud prisel WRITE s funkci nebo konstantou
  		write = 1;  // Nastavim priznak
  		*token = *token - WRITE;  // Nastavim token na odpovidajici token ve write
	  } else if(*token == 120) {  // Pokude je token 120 -> prisel WRITE a '('
  		write = 1;  // Nastavim priznak
  		*token = *token - WRITE;  // Nastavim token na odpovidajici token ve write
	  }

/********************************************************************************************/
		  
	  if(*token == END_OF_FILE) {  // Pokud pøisel konec souboru
	    *token = DOLLAR;  // je na vstupu znak dolaru
	  } else if((*token == RET_STRING) || (*token == RET_INT) || (*token == RET_DOUBLE)) {  // Pokud pøijde konstanta
      generateVariable(&pomocna);  // Vygeneruji pro ni jméno
	    tableInsert(table, &pomocna, *token);  // Ulozím jméno do tabulky
		if((ad1 = tableSearch(table, &pomocna)) == NULL) {  // Vyhledam ji
			stackDispose(&a);
	    strFree(&pom);
	    strFree(&pravidlo);
	    strFree(&vysledek);
	    strFree(&pomocna);
			return SEM_ERROR;
		}
		  // Vygeneruji prazdnou instrukci I_START
      instrukce = malloc(sizeof(tInstr));
      instrukce->instType = I_START;
      instrukce->addr1 = ad1;
	    instrukce->addr2 = ad1;
	    instrukce->addr3 = ad1;
	    listInsertLast(list, *instrukce);
	    instrukce = NULL;

		if(tableInsertValue(table, &pomocna, atrib) == LEX_ERROR) {  // Nastavím ji hodnotu
			stackDispose(&a);
	    strFree(&pom);
	    strFree(&pravidlo);
	    strFree(&vysledek);
	    strFree(&pomocna);
			return LEX_ERROR;
		}
	    strCopyString(atrib, &pomocna);  // Název zkopíruji do at
		  *token = ID;  // Nastavím token na ID
	  } else if((*token == FIND) || (*token == SORT)) {  // Pokud je na vstupu funkce
		  funkce = *token;  // Pamatuji si, jaka funkce prisla
		  *token = FUNCTION;  // Nastavím token na funkci
	  } else if(*token == SEMICOLON || *token == THEN || *token == DO || *token == END || *token == ELSE) {
	    // Pokud je na vstupu ukoncujici lexem
      tokenLast = *token;  // Pamatuji si ho
	    *token = DOLLAR;  // Ukonèím cyklus
	  } else if(*token == ID) {  // Pokud je na vstupu ID
  	  if((ad1 = tableSearch(table, atrib)) == NULL) {
  		  stackDispose(&a);
  	    strFree(&pom);
  	    strFree(&pravidlo);
  	    strFree(&vysledek);
  	    strFree(&pomocna);
  		  return SEM_ERROR;
  		}
  
  		instrukce = malloc(sizeof(tInstr));
      instrukce->instType = I_START;
      instrukce->addr1 = ad1;
      instrukce->addr2 = ad1;
      instrukce->addr3 = ad1;
      listInsertLast(list, *instrukce);
  	  instrukce = NULL;
	  } else if(*token > 16) {  // Pokud je token vìt¹í jak 16
		  stackDispose(&a);
	    strFree(&pom);
	    strFree(&pravidlo);
	    strFree(&vysledek);
	    strFree(&pomocna);
		  return SYNTAX_ERROR;
	  }

	  switch(pole[top(&a)->symbol][*token]) {
		  case '=' : {
        // Vlozím do stacku
			  if(push(&a, *token, atrib, table) == SEM_ERROR) {  // Kontrola vlozeni
				  stackDispose(&a);
  		    strFree(&pom);
  		    strFree(&pravidlo);
  		    strFree(&vysledek);
  		    strFree(&pomocna);
				  return SEM_ERROR;
			  }

			  if((*token = getNextToken(atrib)) == LEX_ERROR) {  // Zadam token
				  stackDispose(&a);
  		    strFree(&pom);
  		    strFree(&pravidlo);
  		    strFree(&vysledek);
  		    strFree(&pomocna);
          return LEX_ERROR;
        }
	      break;
		  }

		  case '|' : {
		    if(a.first->symbol != 'E') {  // Kdyz je první symbol rùzný od E
				  if(push(&a, '|', NULL, table) == SEM_ERROR) {  // Vlozím svislítko
  					stackDispose(&a);
    		    strFree(&pom);
    		    strFree(&pravidlo);
    		    strFree(&vysledek);
    		    strFree(&pomocna);
  					return SEM_ERROR;
				  }
        } else {  // Když je první symbol E
          h = malloc(sizeof(struct stackItem));  // Vytvoøím nový prvek zásobníku
          h->symbol = '|';  // Nastavím mu symbol na svislítko
          h->nextItem = a.firstNonTerm->nextItem;  // Nastavím následující na první non-terminál
          a.firstNonTerm->nextItem = h;  // Nastavím svislítko jako následující
        }

  			if(push(&a, *token, atrib, table) == SEM_ERROR) {  // Vložím token do stacku
  				stackDispose(&a);
  		    strFree(&pom);
  		    strFree(&pravidlo);
  		    strFree(&vysledek);
  		    strFree(&pomocna);
  				return SEM_ERROR;
  			}

        if((*token = getNextToken(atrib)) == LEX_ERROR) {  // Pozádám o dalsí token
			    stackDispose(&a);
  		    strFree(&pom);
  		    strFree(&pravidlo);
  		    strFree(&vysledek);
  		    strFree(&pomocna);
          return LEX_ERROR;
        }
        break;  // Konec vìtve '|'
		  }

      case '>' : {
		    strClear(&pravidlo);
        strClear(&pom);

		    switch(top(&a)->symbol) {
          case PLUS :{  //Výraz s plusem
            // Vytvorim instrukci pro PLUS
				    instrukce = malloc(sizeof(tInstr));  // Alokace instrukce
            instrukce->instType = I_EXPR_PLUS;  // Nastavim typ instrukce
				    instrukce->addr1 = a.firstNonTerm->tablePtr;  // Prvni operand instrukce
  			    instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;  // Druhy operand instrukce
	     			generateVariable(&vysledek);  // Promìnná pro výsledek
				    if(a.firstNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokud je první operand INTEGER
				      if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokude je i druhý operand INTEGER
				        tableInsert(table, &vysledek, TYPE_INTEGER);  // Vlozim promennou do tabulky symbolu
					      if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
					        // Pokud se nevlozila, tak koncim
  							  stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
				          return SEM_ERROR;
				        }
				      } else if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {  // Pokud je operand DOUBLE
				        tableInsert(table, &vysledek, TYPE_DOUBLE);  // Musi byt vysledek DOUBLE
					      if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
					        // Pokud se prvek nevlozil, tak koncim
  							  stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
				          return SEM_ERROR;
    				    }
		    		  } else {  // Pokud neni operand INTEGER ani DOUBLE, tak koncim
  						  stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
				        return SEM_ERROR;
				      }
    				} else if(a.firstNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {  // Pokud je operand DOUBLE
    				  if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // a druhy operand INTEGER
    				    tableInsert(table, &vysledek, TYPE_DOUBLE);  // Bude vysledek DOUBLE
      					if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
      					  // Pokud se nevlozil, tak koncim
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
      				    return SEM_ERROR;
      				  }
    				  } else if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {  // Kdyz je i druhy operand DOUBLE
    				    tableInsert(table, &vysledek, TYPE_DOUBLE);  // Vysledek je DOUBLE
    					  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
    					    // Pokud se vysledek nevlozil, tak koncim
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
    				      return SEM_ERROR;
    				    }
    				  } else {
    				    // Spatny typ operandu
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else if(a.firstNonTerm->tablePtr->data->varType == TYPE_STRING) {  // Pokud je operand STRING
    				  if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_STRING) {  // Musi byt i druhy
    				    // KONKATENACE!!!
    				    tableInsert(table, &vysledek, TYPE_STRING);  // Vysledek je STRING
    					  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
    					    // Pokud se nevlozil, tak koncim
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
    				      return SEM_ERROR;
    				    }
    				  } else {  // Pokud byl spatny typ operandu, tak koncim
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {  // Pokud je spatna konstrukce vyrazu, tak koncim
  						stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}
				    loa = instrukce->addr3;  // Vysledek posledni operace ulozim
            listInsertLast(list, *instrukce);  // Vlozim instrukci do seznamu
            instrukce = NULL;  // Vyprazdnim ukazatel pro novou
				    // Konec generování instrukce PLUS

				    // Zjistit pravidlo a vyjmout ze zásobníku a vlozit výsledek na zásobník
				    do {
              if(a.first->symbol != 'E') {  // Pokud je terminal
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {  // Prochazim
                  strAddChar(&pom, znak[a.first->symbol][u]);  // a pridam znak
                }
                strConcate(&pravidlo, &pom);  // Pridam terminal do pravidla
                strClear(&pom);  // Vymazu pomocnou promennou
              } else {
                strAddChar(&pravidlo, 'E');  // Jinak vlozim nonTerminal
              }
              pop(&a, atrib);  // Odeberu prvek ze zasobniku
            } while((top(&a)->symbol) != '|');  // Dokud nenarazim na zarazku

				    for(g = 0; g < 15; g++) {  // Prochazim pravidla
              str = (strGetStr(&pravidlo));  // Ulozim si pravidlo
              if(!(strcmp(str, pravidla[g]))) {  // Porovnam
                pravda = TRUE;  // Kdyz pravidlo najdu, tak nastavim priznak
                break;  // Koncim prochazeni pravidel
              }
              else pravda = FALSE;  // Pokud jsem nenasel pravidlo
            }

				    if(pravda == FALSE) {  // Kdyz nenajdu pravidlo, tak koncim
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
				      return SYNTAX_ERROR;
				    }

		        pop(&a, atrib);  // Odeberu zarazku ze zasobniku

				    if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
				      // Pokud se terminal nevlozil, tak koncim
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
					    return SEM_ERROR;
				    }

            break;
          }
//------------------------------------------------------------------------------
//            PRO OSTATNI VETVE JE ALGORITMUS STEJNY NEBO HODNE PODOBNY
//------------------------------------------------------------------------------
          case MINUS :{  //Výraz s mínusem
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_MINUS;
				    instrukce->addr1 = a.firstNonTerm->tablePtr;
			      instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
				    generateVariable(&vysledek);  // Promìnná pro výsledek
				    if(a.firstNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokud je první operand INTEGER
				      if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokude je i druhý operand INTEGER
				        tableInsert(table, &vysledek, TYPE_INTEGER);
					      if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
  							  stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
				          return SEM_ERROR;
				        }
				      } else if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {
				        tableInsert(table, &vysledek, TYPE_DOUBLE);
					      if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
				          return SEM_ERROR;
				        }
				      } else {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
				        return SEM_ERROR;
				      }
				    } else if(a.firstNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {
				      if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {
				        tableInsert(table, &vysledek, TYPE_DOUBLE);
					      if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
    				      return SEM_ERROR;
    				    }
    				  } else if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {
    				    tableInsert(table, &vysledek, TYPE_DOUBLE);
  					    if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
    				      return SEM_ERROR;
    				    } 
    				  } else {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

				    // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
				    do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }  
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

  				  for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }
  
  		  		if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
  				    return SYNTAX_ERROR;
  				  }

		        pop(&a, atrib);
		        
    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case MUL :{  //Výraz násobení
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_MUL;
				    instrukce->addr1 = a.firstNonTerm->tablePtr;
			      instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
				    generateVariable(&vysledek);  // Promìnná pro výsledek
				    if(a.firstNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokud je první operand INTEGER
				      if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokude je i druhý operand INTEGER
				        tableInsert(table, &vysledek, TYPE_INTEGER);
					      if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
    				      return SEM_ERROR;
    				    }
    				  } else if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {
    				    tableInsert(table, &vysledek, TYPE_DOUBLE);
    				  	if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
  				        return SEM_ERROR;
  				      }
    				  } else {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else if(a.firstNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {
    				  if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {
    				    tableInsert(table, &vysledek, TYPE_DOUBLE);
    					  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
      				    return SEM_ERROR;
      				  }
    				  } else if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_DOUBLE) {
    				    tableInsert(table, &vysledek, TYPE_DOUBLE);
    					  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
							    stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
    				      return SEM_ERROR;
    				    }
    				  } else {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
            
				    loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
				    // Konec generování instrukce PLUS

				    // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
				    do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
  		        pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

				    for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

    				if(pravda == FALSE) {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

				    if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
					    return SEM_ERROR;
				    }
            break;
          }
//------------------------------------------------------------------------------
          case DIV :{  //Výraz dìlení
		        instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_DIV;
		        instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		        if(a.firstNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokud je první operand INTEGER
		          if(a.firstNonTerm->nextNonTerm->tablePtr->data->varType == TYPE_INTEGER) {  // Pokude je i druhý operand INTEGER
		            tableInsert(table, &vysledek, TYPE_INTEGER);
			          if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						      stackDispose(&a);
          		    strFree(&pom);
          		    strFree(&pravidlo);
          		    strFree(&vysledek);
          		    strFree(&pomocna);
		              return SEM_ERROR;
		            }
		          } else {
					      stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
		            return SEM_ERROR;
		          }
		        } else {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
		          return SEM_ERROR;
		        }

    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
		        // Konec generování instrukce PLUS

	        	// Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case LESSTHAN :{  //Výraz men‘ítka
		        instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_LESS;
    				instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		
    				if(a.firstNonTerm->tablePtr->data->varType == a.firstNonTerm->nextNonTerm->tablePtr->data->varType) {  // Pokude je i druhý operand INTEGER
    				  tableInsert(table, &vysledek, TYPE_INTEGER);
    				  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

		        // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case GREATTHAN :{  //Výraz vìt‘ítka
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_ABOVE;
				    instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		
    				if(a.firstNonTerm->tablePtr->data->varType == a.firstNonTerm->nextNonTerm->tablePtr->data->varType) {  // Pokude je i druhý operand INTEGER
    				  tableInsert(table, &vysledek, TYPE_INTEGER);
    				  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

		        // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
				    	stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case LTOREQUAL :{  //Výraz men‘í nebo rovno
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_LESSEQUAL;
            instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		
    				if(a.firstNonTerm->tablePtr->data->varType == a.firstNonTerm->nextNonTerm->tablePtr->data->varType) {  // Pokude je i druhý operand INTEGER
    				  tableInsert(table, &vysledek, TYPE_INTEGER);
    				  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

		        // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case GTOREQUAL :{  //Výraz vìt‘í nebo rovno
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_ABOVEEQUAL;
            instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		
    				if(a.firstNonTerm->tablePtr->data->varType == a.firstNonTerm->nextNonTerm->tablePtr->data->varType) {  // Pokude je i druhý operand INTEGER
    				  tableInsert(table, &vysledek, TYPE_INTEGER);
    				  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

		        // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case EQUAL :{  //Výraz rovno
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_EQUAL;
            instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		
    				if(a.firstNonTerm->tablePtr->data->varType == a.firstNonTerm->nextNonTerm->tablePtr->data->varType) {  // Pokude je i druhý operand INTEGER
    				  tableInsert(table, &vysledek, TYPE_INTEGER);
    				  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

		        // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case NOTEQUAL : {  //Výraz nerovno
				    instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_NOEQUAL;
            instrukce->addr1 = a.firstNonTerm->tablePtr;
	          instrukce->addr2 = a.firstNonTerm->nextNonTerm->tablePtr;
		        generateVariable(&vysledek);  // Promìnná pro výsledek
		
    				if(a.firstNonTerm->tablePtr->data->varType == a.firstNonTerm->nextNonTerm->tablePtr->data->varType) {  // Pokude je i druhý operand INTEGER
    				  tableInsert(table, &vysledek, TYPE_INTEGER);
    				  if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
						    stackDispose(&a);
        		    strFree(&pom);
        		    strFree(&pravidlo);
        		    strFree(&vysledek);
        		    strFree(&pomocna);
    				    return SEM_ERROR;
    				  }
    				} else {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}
    
    				loa = instrukce->addr3;
            listInsertLast(list, *instrukce);
            instrukce = NULL;
    				// Konec generování instrukce PLUS

		        // Zjistit pravidlo a vyjmout ze zásobníku a vložit výsledek na zásobník
		        do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

			      for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

		        if(pravda == FALSE) {
					    stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);

    				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
    					stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    					return SEM_ERROR;
    				}
            break;
          }
//------------------------------------------------------------------------------
          case ASSIGN : {  //Výraz pøiøazení
			    	instrukce = malloc(sizeof(tInstr));
            instrukce->instType = I_EXPR_EXPR;
				    if(loa == NULL) {
    				  instrukce->addr1 = a.firstNonTerm->tablePtr;
    				  loa = a.firstNonTerm->tablePtr;
    				} else {
    				  instrukce->addr1 = loa;
      			}
			    	instrukce->addr2 = NULL;

			    	do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
		          pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

         		for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              } else pravda = FALSE;
            }

    				if(pravda == FALSE) {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
				      return SYNTAX_ERROR;
				    }

		        pop(&a, atrib);

  					if(push(&a, 'E', atrib, table) == SEM_ERROR) {
  						stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
  						return SEM_ERROR;
  					}
				
    				if(loa->data->varType != a.first->tablePtr->data->varType) {
						  stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
    				  return SEM_ERROR;
    				}

    				instrukce->addr3 = a.first->tablePtr;
    				listInsertLast(list, *instrukce);
            instrukce = NULL;
            break;
          }
//------------------------------------------------------------------------------
  			  case ID : {
  				  do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
                strAddChar(&pravidlo, 'E');
              }
              pop(&a, atrib);
            } while((top(&a)->symbol) != '|');
  
            for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
                pravda = TRUE;
                break;
              } else pravda = FALSE;
            }
            if(pravda == FALSE) {
				      stackDispose(&a);
      		    strFree(&pom);
      		    strFree(&pravidlo);
      		    strFree(&vysledek);
      		    strFree(&pomocna);
  			      return SYNTAX_ERROR;
  			    }
		        pop(&a, atrib);
    				if(push(&a, 'E', atrib, table) == SEM_ERROR) {
    					stackDispose(&a);
    					strFree(&pom);
    					strFree(&pravidlo);
    					strFree(&vysledek);
    					strFree(&pomocna);
    					return SEM_ERROR;
    				}
				    break;
			    }  // Konec ID
/******************************************************************************/
			  case RIGHT_PARENTHESIS : {
				  instrukce = malloc(sizeof(tInstr));  // Naalokuji novou instrukci
				  if (a.firstNonTerm != NULL) ad1 = a.firstNonTerm->tablePtr;  // Ulozim si prvni non-terminal
	              if(a.firstNonTerm->nextNonTerm != NULL) ad2 = a.firstNonTerm->nextNonTerm->tablePtr;  // Ulozim si druhy non-terminal

				  do {
              if(a.first->symbol != 'E') {
                for(u = 0; u < strlen(znak[a.first->symbol]); u++) {
                  strAddChar(&pom, znak[a.first->symbol][u]);
                }
                strConcate(&pravidlo, &pom);
                strClear(&pom);
              } else {
				        strCopyString(&vysledek, &(a.firstNonTerm->tablePtr->key));
                strAddChar(&pravidlo, 'E');
              }
  		        pop(&a, atrib);
            } while((top(&a)->symbol) != '|');

				    for(g = 0; g < 15; g++) {
              str = (strGetStr(&pravidlo));
              if(!(strcmp(str, pravidla[g]))) {
				        if(g == 5) {
      					  if(ad1->data->varType != TYPE_STRING) {
      						  stackDispose(&a);
      						  strFree(&pom);
      						  strFree(&pravidlo);
      						  strFree(&vysledek);
      						  strFree(&pomocna);
      						  return SEM_ERROR;
      					  }
      				    generateVariable(&vysledek);  // Promìnná pro výsledek operace
      					  tableInsert(table, &vysledek, TYPE_STRING);
      				    instrukce->instType = I_SORT;
        					if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
        						stackDispose(&a);
      							strFree(&pom);
      							strFree(&pravidlo);
      							strFree(&vysledek);
      							strFree(&pomocna);
            				return SEM_ERROR;
        					} else {
        					  instrukce->addr1 = ad1;
        					  instrukce->addr2 = NULL;
        					  loa = instrukce->addr3;
        					  listInsertLast(list, *instrukce);
        					  instrukce = NULL;
        					}
      				  } else if(g == 6) {
      				    if(ad1->data->varType != TYPE_STRING) {
      						  stackDispose(&a);
      						  strFree(&pom);
      						  strFree(&pravidlo);
      						  strFree(&vysledek);
      						  strFree(&pomocna);
      						  return SEM_ERROR;
        					}
        					if(ad2->data->varType != TYPE_STRING) {
        						  stackDispose(&a);
        						  strFree(&pom);
        						  strFree(&pravidlo);
        						  strFree(&vysledek);
        						  strFree(&pomocna);
        						  return SEM_ERROR;
        					}
      				    generateVariable(&vysledek);  // Promìnná pro výsledek operace
      					  tableInsert(table, &vysledek, TYPE_INTEGER);
      					  instrukce->instType = I_FIND;
        					if((instrukce->addr3 = tableSearch(table, &vysledek)) == NULL) {
        								stackDispose(&a);
        								strFree(&pom);
        								strFree(&pravidlo);
        								strFree(&vysledek);
        								strFree(&pomocna);
            							return SEM_ERROR;
        					} else {
        					  instrukce->addr1 = ad1;
        					  instrukce->addr2 = ad2;
        					  loa = instrukce->addr3;
        					  listInsertLast(list, *instrukce);
        					  instrukce = NULL;
        					}
      				  }
                pravda = TRUE;
                break;
              }
              else pravda = FALSE;
            }

    				if(pravda == FALSE) {
						stackDispose(&a);
						strFree(&pom);
						strFree(&pravidlo);
						strFree(&vysledek);
						strFree(&pomocna);
    				  return SYNTAX_ERROR;
    				}

		        pop(&a, atrib);
				if(push(&a, 'E', &vysledek, table) == SEM_ERROR) {
					stackDispose(&a);
					strFree(&pom);
					strFree(&pravidlo);
					strFree(&vysledek);
					strFree(&pomocna);
					return SEM_ERROR;
				} 
				  break;
				  }
        }
		    break;  // Konec vìtve '>'
		  }
		  case ' ' : {  // Pokud je v precedencni tabulce mezera, tak koncim
		    stackDispose(&a);
		    strFree(&pom);
		    strFree(&pravidlo);
		    strFree(&vysledek);
		    strFree(&pomocna);
		    return SYNTAX_ERROR;
		    break;
		  }
	  }
  } while((!((*token == DOLLAR || (*token == RIGHT_PARENTHESIS) || (write == 1 && *token == STROKE)) && (top(&a)->symbol == DOLLAR))) && pravda);
  // Podminka pro ukonceni redukce vyrazu
  /*****************************************************************************/
  
  if(write == 1) {  // Pokud byla funkce WRITE, tak nageneruji WRITE instrukci
	  instrukce = malloc(sizeof(tInstr));
    instrukce->instType = I_WRITE;
    instrukce->addr1 = a.firstNonTerm->tablePtr;
	  instrukce->addr2 = NULL;
	  instrukce->addr3 = NULL;
	  listInsertLast(list, *instrukce);
  }

  stackDispose(&a);  // Uvonim zasobnik

  // Uvolnim pomocne retezce
  strFree(&pom);
  strFree(&pravidlo);
  strFree(&vysledek);
  strFree(&pomocna);

  // Pokud jsem si pamatoval posledni token, tak ho nastavim
  if(tokenLast != 0) {
	  *token = tokenLast;
  }

  return SYNTAX_OK;  // Vracim SYNTAX_OK
}
