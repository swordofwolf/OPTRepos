#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Создает новое дерево */
TSymbolTreeNode* CreateTree(const char* rootName)
{
  TSymbolTablesListNode* newSymbolTable = (TSymbolTablesListNode*) malloc(sizeof(TSymbolTablesListNode));
  TSymbolTreeNode* newTree = malloc(sizeof(TSymbolTreeNode));
  strcpy(newTree->name, rootName);
  newTree->left = NULL;
  newTree->right = NULL;
  newSymbolTable->root = newTree;
  newSymbolTable->nextNode = NULL;
  lastSymbolTable->nextNode = (struct TSymbolTablesListNode*) newSymbolTable;
  newSymbolTable->prevNode = (struct TSymbolTablesListNode*) lastSymbolTable;
  lastSymbolTable = newSymbolTable;
  currentSymbolTree = lastSymbolTable->root;
  return newTree;
}

/*очищает таблицу символов*/
int FreeSymbolTable()
{
  while (RecoverTree());
  return 0;
}

/* удаляет верхнее дерево */
TSymbolTreeNode* RecoverTree()
{
  if(!lastSymbolTable->prevNode)
  {
    printf("This is the last tree in the wood. You dont want to cut it.\n");
      FreeSymbolTree(currentSymbolTree);
    currentSymbolTree = NULL;
    free(lastSymbolTable);
    return NULL;
  }
  lastSymbolTable = lastSymbolTable->prevNode;
  free(lastSymbolTable->nextNode);
  lastSymbolTable->nextNode = NULL;
  currentSymbolTree = lastSymbolTable->root;
  return currentSymbolTree;  
}




/* освобождения памяти */
int FreeSymbolTree(TSymbolTreeNode* root)
{
  if (NULL != root->left)
    FreeSymbolTree(root->left);
  
  if (NULL != root->right)
    FreeSymbolTree(root->right);
    
  free(root);
  
  return 0;
}

/* поиск символа в последнем дереве. Если не находит - возвращает NULL */
TSymbolTreeNode* FindSymbol(const char* name)
{

  TSymbolTablesListNode* current = lastSymbolTable;
  while(current)
  {
    TSymbolTreeNode* FindedSymbol = FindSymbolInTree(name, current->root);
    if (NULL != FindedSymbol)
      return FindedSymbol;
    current = current->prevNode;
  }
  return NULL;
}

/* Ищет символ в указанном дереве */
TSymbolTreeNode* FindSymbolInTree(const char* name, TSymbolTreeNode* root)
{
  if (NULL == root)
    return NULL;

  if (0 == strcmp(name, root->name))
  {
    return root;
  }
  
  if (strcmp(name, root->name) > 0)
    return FindSymbolInTree(name, root->left);
  else if (strcmp(name, root->name) < 0)
    return FindSymbolInTree(name, root->right);
    
  return NULL;
}


/* Добавить запись в таблицу */
TSymbolTreeNode* AddRecordToSymbolTable(const char* name, ValueType* type, int isArray)
{
  TSymbolTreeNode* newNode = malloc(sizeof(TSymbolTreeNode));
  strcpy(newNode->name, name);
  newNode->valueType = type;
  newNode->isArray = isArray;
  newNode->left = NULL;
  newNode->right = NULL;

  TSymbolTreeNode* current = currentSymbolTree;
  while (NULL != current)
  {
    if (strcmp(name, current->name) > 0)
    {
      if (0 != current->left)
        current = current->left;
      else
      {
        current->left = newNode;
        return newNode;
      }  
    }
    else
    {
      if (0 != current->right)
        current = current->right;
      else
      {
        current->right = newNode;
        return newNode;
      }
    }
  }
  printf("\ncan not add record to symbol table tree!\n");
  return NULL; 
}






#endif
