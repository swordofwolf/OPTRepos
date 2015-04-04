/* Based upon John Levine's book "flex&bison" */

#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include "subexpression.h"
#include "types.h"

struct TSymbolTreeNode;
struct TSymbolTablesListNode;

/* ��������� �������� ������� �������� */
typedef struct TSymbolTreeNode
{
  char name[255];      /* ��� ����������. */
  int isDeclared; /* ��������� �� ���������� */
  ValueType* valueType; /* ��� ���������� ��� ������������ */
  int isArray; /* �������� �� ������ �������� */
  /* ��������� �� �������� ���� ������ */
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


/* ������� ����� ������ */
TSymbolTreeNode* CreateTree(const char* rootName);
/* ������� ������� ������ */
TSymbolTreeNode* RecoverTree();

TSymbolTreeNode* FindSymbol(const char* name);
TSymbolTreeNode* FindSymbolInTree(const char* name, TSymbolTreeNode* root);
int FreeSymbolTable();

/* ������������ ������ */
int FreeSymbolTree(TSymbolTreeNode* root);

/* �������� ������ � ������� */
TSymbolTreeNode* AddRecordToSymbolTable(const char* name, ValueType* type, int isArray);



#endif
