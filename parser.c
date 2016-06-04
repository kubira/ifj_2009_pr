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
#include "str.h"
#include "bvs.h"
#include "ilist.h"
#include "scaner.h"
#include "sa_parser.h"
#include "parser.h"


tSymbolTable *table;// globalni promenna uchovavajici tabulku symbolu
tListOfInstr *list; // globalni promenna uchovavajici seznam instrukci
int token;          // globalni promenna, ve ktere bude ulozen aktualni token
int var_flag = 0;  // priznak, jestli prisel var
int debug_message = 0; // nastavenim na 1 se budou vypisovat ladici vypisy
string attr; // globalni promenna attr, ve ktere bude ulozen atribut tokenu



void generateInstruction(int instType, void *addr1, void *addr2, void *addr3)
// vlozi novou instrukci do seznamu instrukci
{
   tInstr I;
   
   I.instType = instType;
   I.addr1    = addr1;
   I.addr2    = addr2;
   I.addr3    = addr3;
   
   listInsertLast(list, I);
}  

int counter_var = 1;

void generate_variable(string *var) {
// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace
 
  int i;

  strClear(var);
  strAddChar(var, '#');
  i = counter_var;
  while (i != 0)
  {
    strAddChar(var, (char)(i % 10 + '0'));
    i = i / 10;
  }
  counter_var ++;
}

int statList(); // tato deklarace je nutna, protoze fci volam drive, nez ji definuji


/////////////////////////////////////////////////////////////////////////////
//                           WRITE stroke or expr or end
/////////////////////////////////////////////////////////////////////////////
int stroke_or_expr() {
  
  if (debug_message == 1) printf("stroke or expr or end...  %d\n", token);

  // pozadame o dalsi token
  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR; 
  
  switch(token) {
    case RIGHT_PARENTHESIS:
		 //pravidlo <stroke_or_expr> )
         return SYNTAX_OK;
   
    case ID:
    case RET_STRING:
    case RET_INT:
    case RET_DOUBLE:
    case FIND:
    case SORT:
    case LEFT_PARENTHESIS:
         // pravidlo <stroke_or_expr> <vyraz> , <stroke_or_expr>

          token = WRITE+token;
          
          // volani vyrazu
          switch(sa_parser(&token, &attr, table, list)) {
			      case SYNTAX_ERROR: return SYNTAX_ERROR;
			      case LEX_ERROR:	   return LEX_ERROR;
			      case SEM_ERROR:	   return SEM_ERROR;
		      }

          // nasledovat za vyrazem muze jen carka nebo prava zavorka
          switch(token) {
            case RIGHT_PARENTHESIS: return SYNTAX_OK;
            case STROKE:            return stroke_or_expr(); 
            default:                return SYNTAX_ERROR;       
          }    
    
    default: return SYNTAX_ERROR;                     
  }
}

/////////////////////////////////////////////////////////////////////////////
//                           READ stroke_or_right_parenthesis
/////////////////////////////////////////////////////////////////////////////
int stroke_or_right_parenthesis() {
  tTableItem *tptr; // pomocna promenna, kam se mne bude ukladat datova slozka ze stromu
  
  if (debug_message == 1) printf("stroke or right...  %d\n", token);
  
  // pozadame o dalsi token
  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR; 
  
  switch(token) {
    case STROKE:  
        //pravidlo <stroke_or_right_parenthesis> , id <stroke_or_right_parenthesis>
        
        // pozadame o dalsi token, kterym musi byt id
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        if (token != ID) return SYNTAX_ERROR;
        
        // nagenerujeme novou instrukci pro interpret
		 tptr = tableSearch(table, &attr);		// nejdrive najdu v tabulce symbolu danou promennou
	     if (tptr == NULL) return SEM_ERROR;	    // pokud nenajdu, promenna nebyla deklarovana
	     generateInstruction(I_READ, tptr, NULL, NULL);
                
        return stroke_or_right_parenthesis();
        
    case RIGHT_PARENTHESIS:  
        //pravidlo <stroke_or_right_parenthesis> ) 
             
        // pozadame o dalsi token, kterym musi byt ;
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        if (token == SEMICOLON || token == ELSE || token == END) {  // && token != ELSE && token != END
          return SYNTAX_OK;
        }
        
        return SYNTAX_ERROR;
  }
  return SYNTAX_ERROR;
}

/////////////////////////////////////////////////////////////////////////////
//                           STAT
/////////////////////////////////////////////////////////////////////////////
int stat() {
     
	tTableItem *tptr;
	tListItem *navesti1, *navesti2, *helpItem, *helpItem2;

    int result; 

    if (debug_message == 1) printf("stat %d\n", token);

    switch(token) {
      //*********************************************************************** 
	    case ID:
        // pravidlo <stat> <vyraz> 

        // volani vyrazu	
		    switch(sa_parser(&token, &attr, table, list)) {
			    case SYNTAX_ERROR: return SYNTAX_ERROR;
			    case LEX_ERROR:	   return LEX_ERROR;
			    case SEM_ERROR:	   return SEM_ERROR;
		    }
        
        result = SYNTAX_OK;

        break;
      
      //***********************************************************************
      case READLN:  
        //pravidlo <stat> readln ( <stroke_or_right_parenthesis>
        
        // pozadame o dalsi token, kterym musi byt (
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        if (token != LEFT_PARENTHESIS) return SYNTAX_ERROR;
        
        // pozadame o dalsi token
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        
        switch (token) {
          case ID:
            // nagenerujeme novou instrukci pro interpret
		        tptr = tableSearch(table, &attr);		// nejdrive najdu v tabulce symbolu danou promennou
		        if (tptr == NULL) return SEM_ERROR;	// pokud nenajdu, promenna nebyla deklarovana
		        generateInstruction(I_READ, tptr, NULL, NULL);
        
            result = stroke_or_right_parenthesis();
            break;
            
          case RIGHT_PARENTHESIS:
              result = SYNTAX_OK;
           
             // pozadame o dalsi token
             if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
             break;
          
          default: return SYNTAX_ERROR;
        }
              
        break;
		  
		  //***********************************************************************
      case WRITE:  
        //pravidlo <stat> write ( <stroke_or_expr> 
        
        // pozadame o dalsi token, kterym musi byt (
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        if (token != LEFT_PARENTHESIS) return SYNTAX_ERROR;
       
        result = stroke_or_expr();
        
		    if(result != SYNTAX_OK) return result;
		    
        // pozadame o dalsi token
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        
        break;
      
      //***********************************************************************
      case WHILE:  
      //pravidlo <stat> while <vyraz> do <stat>
      // nebo
	  //pravidlo <stat> while <vyraz> do begin <statList>
      
        // nagenerujeme nove navesti zacatku while
	      generateInstruction(I_START, NULL, NULL, NULL);	
		    navesti1 = listGetPointerLast(list);
		    navesti1->Instruction.addr3 = navesti1;
      
        // pozadame o dalsi token
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
      
        // volani vyrazu
        switch(sa_parser(&token, &attr, table, list)) {
			    case SYNTAX_ERROR: return SYNTAX_ERROR;
			    case LEX_ERROR:	   return LEX_ERROR;
			    case SEM_ERROR:	   return SEM_ERROR;
		    }  
        
        // token, kterym musi byt DO
        if (token != DO) return SYNTAX_ERROR;
             
        // sa parser mne vygeneroval instrukci, kde na addr3 najdu vysledek vyrazu
        helpItem2 = listGetPointerLast(list);
        
        // nagenerujeme instrukci if_not, jeste do prvni addr1 ulozit vysledek podminky
		// musim si nekam ulozit a zapamatovat tuto instrukci, abych pak mohl ulozit
		// na addr3 konec while, proto ukladam do helpItem
		generateInstruction(I_IF_NOT, NULL, NULL, NULL);
		helpItem = listGetPointerLast(list); 
		    
		// ulozim do instrukce IF_NOT na addr1 vysledek podminky
		helpItem->Instruction.addr1 = helpItem2->Instruction.addr3;
        
        // pozadame o dalsi token a zavolame stat pro while nebo v pripade 
        // begin zavolame statList pro while
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        
        if (token == BEGIN) {
         //pravidlo <stat> while <vyraz> do begin <statList>        
          result = statList(); 
        }
		// pravidlo <stat> while <vyraz> do <stat>
        else result = stat();
        
        if (result != SYNTAX_OK) return result;   // pokud nastala chyba
        
        // nagenerujeme instrukci goto, ktera bude skakat na zacatek while
		generateInstruction(I_GOTO, NULL, NULL, NULL);
	    navesti2 = listGetPointerLast(list);
		navesti2->Instruction.addr3 = navesti1;

		// nagenerujeme nove navesti na konec while
	    generateInstruction(I_START, NULL, NULL, NULL);	
		navesti2 = listGetPointerLast(list);
		navesti2->Instruction.addr3 = navesti2;

		helpItem->Instruction.addr3 = navesti2; 
        
        break;  
      
      //***********************************************************************
      case IF:  
      //pravidla 
	  //<stat> if <vyraz> then <stat> else <stat>
      //<stat> if <vyraz> then begin <statList> else <stat>
      //<stat> if <vyraz> then <stat> else begin <statList>
      //<stat> if <vyraz> then begin <statList> else begin <statList>

      
        // pozadame o dalsi token
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
      
        // volani vyrazu
        switch(sa_parser(&token, &attr, table, list)) {
			    case SYNTAX_ERROR: return SYNTAX_ERROR;
			    case LEX_ERROR:	   return LEX_ERROR;
			    case SEM_ERROR:	   return SEM_ERROR;
		    } 
		    
		// token musi byt then
        if (token != THEN) return SYNTAX_ERROR;
		    
		// sa parser mne vygeneroval instrukci, kde na addr3 najdu vysledek vyrazu
        helpItem2 = listGetPointerLast(list);
        
        // nagenerujeme instrukci if_not, jeste do prvni addr1 ulozit vysledek if
		// musim si nekam ulozit a zapamatovat tuto instrukci, abych pak mohl ulozit
		// na addr3 konec while, proto ukladam do helpItem
		generateInstruction(I_IF_NOT, NULL, NULL, NULL);
		helpItem = listGetPointerLast(list);                  
         
        // ulozim do instrukce IF_NOT na addr1 vysledek podminky
		helpItem->Instruction.addr1 = helpItem2->Instruction.addr3;
        
        // pozadame o dalsi token a zavolame stat pro if nebo v pripade 
        // begin zavolame statList pro if
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        
        if (token == BEGIN) {
          // pravidlo <stat> if <vyraz> then begin <statList> else <stat>
          result = statList(); 
        }
		 // pravidlo <stat> if <vyraz> then <stat> else <stat>
        else result = stat();
        
        if (result != SYNTAX_OK) {
        return result;}   // pokud nastala chyba
        
        // nagenerujeme instrukci goto, ktera bude skakat z konce tela if na konec, tj. ZA else
		// musime si instrukci zapamatovat a adresu skoku priradime az na konci tohoto bloku if
		generateInstruction(I_GOTO, NULL, NULL, NULL);
		navesti2 = listGetPointerLast(list);
        
        //------------------------ BLOK ELSE
                
        // dalsi token musi byt else, uz ho mame nacteny
        //if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;      
        if (token != ELSE) return SYNTAX_ERROR;
        
        // nagenerujeme nove navesti na zacatek else
	    generateInstruction(I_START, NULL, NULL, NULL);	
		navesti1 = listGetPointerLast(list);
		navesti1->Instruction.addr3 = navesti1;
		    
        // ulozim do instrukce IF_NOT skok na zacatek else
		helpItem->Instruction.addr3 = navesti1;
        
        // pozadame o dalsi token a zavolame stat pro else nebo v pripade 
        // begin zavolame statList pro else
        if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
        
        if (token == BEGIN) {
          //pravidlo <stat> if <vyraz> then begin <statList> else begin <statList> 
          result = statList(); 
        }
		// pravidlo <stat> if <vyraz> then <stat> else begin <statList>
        else result = stat();
        
        // nagenerujeme nove navesti za konec else
	    generateInstruction(I_START, NULL, NULL, NULL);	
		navesti1 = listGetPointerLast(list);
		navesti1->Instruction.addr3 = navesti1;
		// ulozim do instrukce GOTO skok za konec else
		navesti2->Instruction.addr3 = navesti1;
        
        break;
      
      //***********************************************************************  
      default: return SYNTAX_ERROR;
		
    } 
	    // pravidlo <stat> ; <stat>
	    // pokud mne prijde strednik, musi za nim nasledovat dalsi prikaz
	    // pokud neprisel strednik, musi prijit END nebo ELSE, nic jineho!
        if (token == SEMICOLON) {
          // pozadame o dalsi token 
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          
          result = stat();
        }  
        else {
          if (token == END || token == ELSE )
          
           return result;
          else return SYNTAX_ERROR;
        }
        
        
        return result;         
}

/////////////////////////////////////////////////////////////////////////////
//                           STAT LIST
/////////////////////////////////////////////////////////////////////////////
/*
Tato fce se ukonci bud chybou nebo kdyz prijde ukoncovaci slozena zavorka. 
Jinak stale cykli. Muze zde prijit bud } (ukoncujici slozena zavorka) nebo nize 
vypsane prikazy. Tady zpracovavam pouze instrukce - celky (jakesi komplexni bloky), 
kdezto ve funkci stat pak jejich soucasti. Napr. token bude CIN, zavolam stat(), 
ktera mne vyresi cely CIN, tj. CIN << id << id; a beh se mne vrati opet do fce
statList, kde po radne ukoncenem prikazu muze nasledovat zase jen zacatek dalsiho
prikazu, z vyctu nize. Proto resim jen tyto "klicova slova", kterymi zacina prikaz.
Je dulezite si uvedomit, ze prikaz else zde v podstate neexistuje, protoze
my pracujeme pouze s pripadem, kdy je else povinnym prvkem if. Proto je zde 
pouze if a ten se cely (i vcetne else) vyresi v stat().
*/
int statList() {
  int result;
  
  if (debug_message == 1) printf("statList %d\n", token);
  
  switch (token) {
    case END:
      // <statList> end

      //pozadame o dalsi token a vratime SYNTAX_OK
      if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
      return SYNTAX_OK;
    
    case BEGIN:
       // pravidlo <statList> begin <statList>

       //pozadame o dalsi token a vratime SYNTAX_OK
       if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
       result = statList();
                  
       break; 
                    
    case ID:
    case READLN:
    case WRITE:
    case IF:
    case WHILE:     
      // pravidlo <statList> -> <stat> <statList>
  
      result = stat(); 
                     
      //******************
      //* ve stat musim nacist dalsi token! ne tady!
      //******************
      
      if (result != SYNTAX_OK) return result;
       
      return statList();
          
    default:
        // pokud aktualni token je jiny nez vyse uvedene, jedna se o syntaktickou chybu 
        return SYNTAX_ERROR;      
  }
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//                           DECLARATION LIST
/////////////////////////////////////////////////////////////////////////////
/*
Funkce cykli tak dlouho, dokud nenacte int main () { nebo chybu.
*/
int declrList() {

  string help_attr; // pomocna pro ulozeni nazvu promenne
  int control; // pomocna pro kontrolu, jestli uz id neexistuje
  
  strInit(&help_attr); // inicializae pomocne promenne
  
 if (debug_message == 1)  printf("declList %d\n", token);

  switch(token) {
    case VAR:
      // pravidlo <declList> var <declList>

      var_flag = 1;

      // pozadame o dalsi token, kterym musi byt id a rekurzivne zavolame 
      // funkci declrList cele pravidlo probehne v poradku, pokud nam tato 
      // funkce vrati uspech
      if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
      if (token != ID) return SYNTAX_ERROR;
      
      return declrList();    
      
    case ID:
      // pravidlo <declList> id : <type_of_id> 
      
      if (var_flag == 0) return SYNTAX_ERROR;
            
      strCopyString(&help_attr, &attr);  // musim si ulozit nazev promenne, protoze v dalsim kroku bych o to prisel, aniz bych znal datovy typ

      // pozadame o dalsi token, kterym musi byt :
      if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
      if (token != RET_DDOT) return SYNTAX_ERROR;
      
      // pozadame o dalsi token a skocime do pravidla sekce <type_of_id>
      if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
      
      // <type_of_id> --------------------------------------------------
      switch(token) {
        case INTEGER:     
          // pravidlo <type_of_id> integer ; <declList>
      
          // pozadame o dalsi token, kterym musi byt ;
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          if (token != SEMICOLON) return SYNTAX_ERROR;
        
          // musime ulozit do tabulky symbolu, ID v tabulce jiz nesmi byt
          control = tableInsert(table, &help_attr, TYPE_INTEGER);
      
          if (control == 2) return SEM_ERROR; // id uz je v tabulce
          else if (control == 1) return MEM_ERROR; // nedostatek pameti 
        
          // pozadame o dalsi token a rekurzivne zavolame funkci declrList
          // cele pravidlo probehne v poradku, pokud nam tato funkce vrati uspech
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          return declrList();
    
        case KDOUBLE:     
          // pravidlo <type_of_id> double ; <declList>
      
          // pozadame o dalsi token, kterym musi byt ;
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          if (token != SEMICOLON) return SYNTAX_ERROR;
        
          // musime ulozit do tabulky symbolu, ID v tabulce jiz nesmi byt
          control = tableInsert(table, &help_attr, TYPE_DOUBLE);
      
          if (control == 2) return SEM_ERROR; // id uz je v tabulce
          else if (control == 1) return MEM_ERROR; // nedostatek pameti 
        
          // pozadame o dalsi token a rekurzivne zavolame funkci declrList
          // cele pravidlo probehne v poradku, pokud nam tato funkce vrati uspech
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          return declrList();
    
        case STRING:     
          // pravidlo <type_of_id> string ; <declList>
      
          // pozadame o dalsi token, kterym musi byt ;
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          if (token != SEMICOLON) return SYNTAX_ERROR;
        
          // musime ulozit do tabulky symbolu, ID v tabulce jiz nesmi byt
          control = tableInsert(table, &help_attr, TYPE_STRING);
      
          if (control == 2) return SEM_ERROR; // id uz je v tabulce
          else if (control == 1) return MEM_ERROR; // nedostatek pameti 
        
          // pozadame o dalsi token a rekurzivne zavolame funkci declrList
          // cele pravidlo probehne v poradku, pokud nam tato funkce vrati uspech
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          return declrList();
          
          default:
            return SYNTAX_ERROR;
      } 
    
    case BEGIN:
      // pravidlo <declList> begin <statList>
      
      // pozadame o dalsi token
      if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;   
        
      return SYNTAX_OK;
      
  } // hlavni switch
  return SYNTAX_ERROR;  
}
  

/////////////////////////////////////////////////////////////////////////////
//                           PARSE
/////////////////////////////////////////////////////////////////////////////
int parse(tSymbolTable *ST, tListOfInstr *instrList) {
  int result;  
  table = ST;  
  list = instrList;
  strInit(&attr);
  
  // nastala chyba jiz pri nacteni prvniho lexemu
  if ((token = getNextToken(&attr)) == LEX_ERROR) {
    strFree(&attr);
    return LEX_ERROR;
  }
  else {
    // pravidlo <parse> <declList> <statList> EOF

    // zavolam deklaracni cast - <declList>
    result = declrList();
    
    // pokud mne deklarace probehne syntakticky v poradku
    if (result == SYNTAX_OK) {
      
      // pokracuji v analyze dal, tj. zavolam statList pro fci main - <statList>
      result = statList();
      if (debug_message == 1) printf("\n Parse %d \n", result-SYNTAX_OK);
      
      //vypis(table->first);    // vypise tabulku symbolu - pouze na odladeni
      //listPrint(list);

      // pokud statList neprobehl v poradku
      if (result != SYNTAX_OK) {
        
        strFree(&attr);
        return result;
      }
      else {
        // vse mne probehlo v poradku, musi jen nasledovat tecka (end.) a konec souboru
        if (token != DOT) return SYNTAX_ERROR;
        else {
          // pozadame o dalsi token, kterym musi byt EOF 
          if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
          
          if (token == END_OF_FILE) return SYNTAX_OK;
          else {
            return SYNTAX_ERROR;
            strFree(&attr);
          } 
        }
        
      
      }
    } 
    else {
      strFree(&attr);
      return result;
     }  
  }  
  
  strFree(&attr);
  return SYNTAX_ERROR;
}
