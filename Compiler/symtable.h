/* Based upon John Levine's book "flex&bison" */

#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include "subexpression.h"
#include "types.h"

struct TSymbolTreeNode;
struct TSymbolTablesListNode;

/* Структура элемента таблицы символов */
typedef struct TSymbolTreeNode
{
  char name[255];      /* Имя переменной. */
  int isDeclared; /* Объявлена ли переменная */
  ValueType* valueType; /* Тип переменной или подвыражения */
  int isArray; /* является ли символ массивом */
  /* указатели на дочерние узлы дерева */
  struct TSymbolTreeNode* left;
  struct TSymbolTreeNode* right;
} TSymbolTreeNode;


typedef struct 
{
  TSymbolTreeNode* root;
  struct TSymbolTablesListNode* nextNode;
  struct TSymbolTablesListNode* prevNode;
  
} TSymbolTablesListNode;



TSymbolTablesListNode* lastSymbolTable; 
TSymbolTreeNode* currentSymbolTree;


/* Создает новое дерево */
TSymbolTreeNode* CreateTree(const char* rootName);
/* удаляет верхнее дерево */
TSymbolTreeNode* RecoverTree();

TSymbolTreeNode* FindSymbol(const char* name);
TSymbolTreeNode* FindSymbolInTree(const char* name, TSymbolTreeNode* root);
int FreeSymbolTable();

/* освобождения памяти */
int FreeSymbolTree(TSymbolTreeNode* root);

/* Добавить запись в таблицу */
TSymbolTreeNode* AddRecordToSymbolTable(const char* name, ValueType* type, int isArray);



#endif
