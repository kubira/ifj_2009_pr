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

#define TRUE 1
#define FALSE 0

tSymbolTable table;

typedef struct stackItem {
  int term;
  int symbol;
  int function;
  tTableItem *tablePtr;
  struct stackItem *nextItem;
  struct stackItem *nextNonTerm;
} *stackItemPtr;

typedef struct {
  stackItemPtr first;
  stackItemPtr top;
  stackItemPtr firstNonTerm;
} stack;

void generateVariable(string *var);
void stackInit(stack *s);
int push(stack *s, int symb, string *name, tSymbolTable *table);
int isEmpty(stack *s);
stackItemPtr top(stack *s);
void pop(stack *s, string *name);
void stackVypis(stack *s);
void stackDispose(stack *s);
