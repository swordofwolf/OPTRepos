#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "symtable.h"

// тип узла синтаксического дерева
typedef enum
{
		// константа
    typeIntConst,         
    typeFloatConst,
    typeStringConst,
    typeIdentifier,    // идентификатор
    typeTmpvar,         // временная переменная
    typeBinOp,
    typeAssign,
    typeL,
    typeStructRefference,
    typeArrayRefference,
    typeIf,
    typeIfElse,
    typeDo,
    typeFor,
    typeWhile,
    typeGoto,
    typeLabel,
    typeDeclaration,
    typeUnNode,
    typeManualGoto
} nodeEnum;


// константы
typedef struct
{
    double fvalue;       // значение константы
    int ivalue;
    char* svalue;
} constNodeType;

typedef struct nodeTypeTag
{
    struct nodeTypeTag* left;
    struct nodeTypeTag* right;
    nodeEnum     type;   // тип узла дерева
    char expType[255];
    char place[30];
    char addInfo[10];

} nodeType;

typedef struct
{
	struct nodeTypeTag* first;
  struct nodeTypeTag* second;
  nodeEnum     type;   // тип узла дерева
  char expType[255];
  char place[30];
  char addInfo[10];
  struct nodeTypeTag* third;
  struct nodeTypeTag* fourth;
  char beginLabel[30];
  char endLabel[30];
} nodeTypeEx;


nodeType* CreateIntConstantNode(int value);
nodeType* CreateStringConstantNode(const char* value);
nodeType* CreateFloatConstantNode(float value);
nodeType* CreateBinOperNode(nodeType* left, nodeType* right, const char* type, const char* tmpVar, const char* op);
nodeType* CreateAssignNode(nodeType* left, nodeType* right);
nodeType* CreateLNode(nodeType* left, nodeType* right);
nodeType* CreateStructRefferenceNode(const char* left, const char* right);
nodeType* CreateArrayRefferenceNode (nodeType* exp, const char* type, const char* varName);
nodeType* CreateDeclarationNode(TSymbolTreeNode* record);
nodeType* CreateIfElseNode(nodeType* cond, nodeType* ifstmt, nodeType* elsestmt);
nodeType* CreateIfNode(nodeType* cond, nodeType* stmt);
nodeType* CreateIfElseNode(nodeType* cond, nodeType* thenstmt, nodeType* elsestmt);
nodeType* CreateForNode(nodeType* first, nodeType* second, nodeType* third, nodeType* fourth);
nodeType* CreateWhileNode(nodeType* cond, nodeType* stmt);
nodeType* CreateDoNode(nodeType* stmt, nodeType* cond);
nodeType* CreateGotoNode(const char* label);
nodeType* CreateManualGotoNode(const char* label);
nodeType* CreateLabelNode(const char* labelName);
nodeType* CreateUnOperNode(nodeType* expNode, const char* typeName, const char* opName);

int codegenUnOp(nodeType* src);
int codegenBinOp(nodeType* left, nodeType* right, const char* tmpVar, const char* op);
int codegenLabel(const char* l);
int codegenGoto(const char* labelName);
int codegenFor(nodeType* src);
int codegenWhile(nodeType* src);
int codegenDo(nodeType* src);
int codegenIfElse(nodeType* src);
int codegenAssign(nodeType* src);
int codegenStructRef(nodeType* left, nodeType* right, const char* tmpVar);
int codegenArrayRef(nodeType* var, nodeType* count, const char* tmpVar);
int codegen(nodeType* src);
int my_yyerror(char* error_message);
nodeType* CreateVariableNode(TSymbolTreeNode* symbol);
void CreateTmpVariable(char* dest);
int freeTree(nodeType* src);

#endif
