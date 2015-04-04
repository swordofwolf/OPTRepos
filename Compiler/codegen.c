#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "codegen.h"
#include "symtable.h"
#include "labels.h"

extern CycleStack* cycleStack;
extern int yylineno;
extern int yydebug; 
FILE* outputFile;
int isError;
int tmpVariableCounter = 0;

int codegenLabel(const char* l)
{
	fprintf(outputFile, "%s:\n", l);
	return 0;
}


int my_yyerror(char* error_message)
{
    fprintf(stderr, "Line %d: %s.\n", yylineno, error_message);
    isError = !0;
    return !0;
}

void CreateTmpVariable(char* dest)
{
	sprintf(dest, "$T%d", tmpVariableCounter++);
}

int codegenUnOp(nodeType* src)
{
	fprintf(outputFile, "\t%s\t:=\t!:s\n", src->place, src->left->place);
}

int codegenBinOp(nodeType* left, nodeType* right, const char* tmpVar, const char* op)
{
	fprintf(outputFile, "\t%s\t:=\t%s\t%s\t%s\n", tmpVar, left->place, op, right->place);
	return 0;	
}

int freeTree(nodeType* src)
{
	switch(src->type)
	{
	case typeIfElse:
		if (NULL != ((nodeTypeEx*) src)->first)
			freeTree((nodeType*) ((nodeTypeEx*) src)->first);
		if (NULL != ((nodeTypeEx*) src)->second)
			freeTree((nodeType*) ((nodeTypeEx*) src)->second);
		if (NULL != ((nodeTypeEx*) src)->third)
			freeTree((nodeType*) ((nodeTypeEx*) src)->third);
			break;
	case typeFor:
		if (NULL != ((nodeTypeEx*) src)->first)
			freeTree((nodeType*) ((nodeTypeEx*) src)->first);
		if (NULL != ((nodeTypeEx*) src)->second)
			freeTree((nodeType*) ((nodeTypeEx*) src)->second);
		if (NULL != ((nodeTypeEx*) src)->third)
			freeTree((nodeType*) ((nodeTypeEx*) src)->third);
		if (NULL != ((nodeTypeEx*) src)->fourth)
			freeTree((nodeType*) ((nodeTypeEx*) src)->fourth);
		break;
	case typeWhile:
	case typeDo:
	case typeL:
	case typeIf:
	case typeBinOp:
		if (NULL != src->left)
			freeTree(src->left);
		if (NULL != src->right)
			freeTree(src->right);
		break;
	case typeAssign:
		if (NULL != src->right)
			freeTree(src->right);
		break;
	case typeUnNode:
	case typeArrayRefference:
		if (NULL != src->left)
			freeTree(src->left);
		break;
	case typeStructRefference:
	case typeIntConst:
	case typeLabel:
	case typeFloatConst:
	case typeStringConst:
	case typeIdentifier:
	case typeDeclaration:
	case typeTmpvar:
		break;
	default:
			printf("\n unknown type\n");
		break;
	}
	free(src);
}

int codegen(nodeType* src)
{

	switch(src->type)
	{
	case typeFor:
		codegenFor(src);
		break;
	case typeWhile:
		codegenWhile(src);
		break;
	case typeUnNode:
		codegenUnOp(src);
		break;
	case typeDo:
		codegenDo(src);
		break;
	case typeIfElse:
		codegenIfElse(src);
	break;
	case typeBinOp:
		codegen(src->left);
		codegen(src->right);
		fprintf(outputFile, "\t%s\t:=\t%s\t%s\t%s\n", src->place, src->left->place, src->addInfo, src->right->place);
		break;
	case typeAssign:
		codegen(src->left);
		codegen(src->right);
		codegenAssign(src);
		break;
	case typeGoto:
		codegenGoto(src->place);
		break;
	case typeManualGoto:
		if (0 == FindLabelCodeName(src->place))
		{
			char errormsg[32];
			sprintf(errormsg, "dont found label %s", src->place);
			my_yyerror(errormsg);
			fprintf(outputFile, "LUndefined:\n");
			break;
		}
		codegenGoto(FindLabelCodeName(src->place));
		break;
		case typeIf:
		codegenIf(src);
		break;
	case typeArrayRefference:
		codegen(src->left);
		break;
	case typeL:
		codegen(src->left);
		codegen(src->right);
		break;
	case typeLabel:
		codegenLabel(src->place);
		break;
	case typeStructRefference:
	case typeDeclaration:
	case typeIntConst:
	case typeFloatConst:
	case typeStringConst:
	case typeIdentifier:
	case typeTmpvar:
	break;
	default:
		printf("\n unknown type\n");
		break;
	}
}

int codegenAssign(nodeType* src)
{
	fprintf(outputFile, "\t%s\t:=\t%s\n", src->left->place, src->right->place);
}

int codegenGoto(const char* labelName)
{
	fprintf(outputFile, "\tgoto\t%s\n", labelName);
	return 0;
}
int codegenWhile(nodeType* src)
{
	nodeTypeEx* n = (nodeTypeEx*) src;
	codegenLabel(n->beginLabel);
	codegen(n->first);
	fprintf(outputFile, "\tiffalse\t%s\tgoto\t%s\n", n->first->place, n->endLabel);
	codegen(n->second);
	codegenGoto(n->beginLabel);
	codegenLabel(n->endLabel);
}
int codegenDo(nodeType* src)
{
	nodeTypeEx* n = (nodeTypeEx*) src;
	codegenLabel(n->beginLabel);
	codegen(n->second);
	codegen(n->first);
	fprintf(outputFile, "\tiffalse\t%s\tgoto\t%s\n", n->second->place, n->endLabel);
	codegenGoto(n->beginLabel);
	codegenLabel(n->endLabel);
}


int codegenFor(nodeType* src)
{
	nodeTypeEx* n = (nodeTypeEx*) src;
	codegen(n->first);
	codegenLabel(n->beginLabel);
	codegen(n->second);
	fprintf(outputFile, "\tiffalse\t%s\tgoto\t%s\n", n->second->place, n->endLabel);
	codegen(n->third);
	codegen(n->fourth);
	codegenGoto(n->beginLabel);
	codegenLabel(n->endLabel);
	
}

int codegenIfElse(nodeType* src)
{
	char labelName[30];
	GetNextLabel(labelName);
	char labelNameEnd[30];
	GetNextLabel(labelNameEnd);
	
	codegen(src->left);
	fprintf(outputFile, "\tiffalse\t%s\tgoto\t%s\n", src->left->place, labelName);
	codegen(src->right);
	codegenGoto(labelNameEnd);
	codegenLabel(labelName);
	codegen(((nodeTypeEx*)src)->third);
	codegenLabel(labelNameEnd);
}

int codegenIf(nodeType* src)
{
	char labelName[30];
	GetNextLabel(labelName);
	codegen(src->left);
	fprintf(outputFile, "\tiffalse\t%s\tgoto\t%s\n", src->left->place, labelName);
	codegen(src->right);
	codegenLabel(labelName);
}

nodeType* CreateManualGotoNode(const char* label)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeManualGoto;
 	strcpy(p->place, label);
  
  return (nodeType*) p; 
}


nodeType* CreateGotoNode(const char* label)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeGoto;
 	strcpy(p->place, label);
  
  return (nodeType*) p; 
}

nodeType* CreateLabelNode(const char* labelName)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeLabel;
 	strcpy(p->place, labelName);
  
  return (nodeType*) p; 
}

nodeType* CreateWhileNode(nodeType* cond, nodeType* stmt)
{
	nodeTypeEx* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeTypeEx);
  if (NULL == (p = (nodeTypeEx*)malloc(nodeSize)))
     my_yyerror("out of memory");
     
/* установить значени€ полей */
  p->type = typeWhile;
  p->first = cond;
  p->second = stmt;
  strcpy(p->beginLabel, GetBeginLabel());
  strcpy(p->endLabel, GetEndLabel());
  
  return (nodeType*) p; 
}
nodeType* CreateDoNode(nodeType* stmt, nodeType* cond)
{
	nodeTypeEx* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeTypeEx);
  if (NULL == (p = (nodeTypeEx*)malloc(nodeSize)))
     my_yyerror("out of memory");
     
/* установить значени€ полей */
  p->type = typeDo;
  p->first = cond;
  p->second = stmt;
  strcpy(p->beginLabel, GetBeginLabel());
  strcpy(p->endLabel, GetEndLabel());
  
  return (nodeType*) p; 
}


nodeType* CreateForNode(nodeType* first, nodeType* second, nodeType* third, nodeType* fourth)
{
	nodeTypeEx* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeTypeEx);
  if (NULL == (p = (nodeTypeEx*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeFor;
  p->first = first;
  p->second = second;
  p->third = third;
  p->fourth = fourth;
  strcpy(p->beginLabel, GetBeginLabel());
  strcpy(p->endLabel, GetEndLabel());
  
  return (nodeType*) p; 
}

nodeType* CreateStructRefferenceNode(const char* left, const char* right)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeStructRefference;
  sprintf(p->place, "%s.%s", left, right);

  p->right = NULL;
  p->left = NULL;

  return p;
}

nodeType* CreateDeclarationNode(TSymbolTreeNode* record)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeDeclaration;
  strcpy(p->place, record->name);
  strcpy(p->expType, record->valueType->name);
  p->right = NULL;
  p->left = NULL;

  return p;
}

nodeType* CreateLNode(nodeType* left, nodeType *right)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeL;

  p->right = right;
  p->left = left;

  return p;
}

nodeType* CreateAssignNode(nodeType* left, nodeType* right)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeAssign;

  p->right = right;
  p->left = left;

  return p;
}

nodeType* CreateBinOperNode(nodeType* left, nodeType* right, const char* type, const char* tmpVar, const char* op)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeBinOp;
  strcpy(p->expType, type);
  snprintf(p->place, 20, "%s", tmpVar);
	strcpy(p->addInfo, op);
  p->right = right;
  p->left = left;

  return p;
}

nodeType* CreateUnOperNode(nodeType* expNode, const char* typeName, const char* opName)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeUnNode;
  strcpy(p->expType, typeName);
  char tmpVar[30];
	CreateTmpVariable(tmpVar);
  snprintf(p->place, 20, "%s", tmpVar);
	strcpy(p->addInfo, opName);
  p->right = NULL;
  p->left = expNode;

  return p;
}


nodeType* CreateIntConstantNode(int value)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeIntConst;
  strcpy(p->expType, "int");
  snprintf(p->place, 20, "%d", value);

  p->right = p->left = NULL;

  return p;
}

nodeType* CreateStringConstantNode(const char* value)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  
  p->type = typeStringConst;  
  strcpy(p->expType, "string");
  sprintf(p->place, "\"%s\"", value);

  p->right = p->left = NULL;

  return p;
}

nodeType* CreateFloatConstantNode(float value)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeFloatConst;
  strcpy(p->expType, "float");
  snprintf(p->place, 20, "%f", value);

  p->right = p->left = NULL;

  return p;
}

nodeType* CreateIfElseNode(nodeType* cond, nodeType* thenstmt, nodeType* elsestmt)
{
	nodeTypeEx* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeTypeEx);
  if (NULL == (p = (nodeTypeEx*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeIfElse;
  p->first = cond;
  p->second = thenstmt;
  p->third = elsestmt; 


  return (nodeType*) p;
}

nodeType* CreateIfNode(nodeType* cond, nodeType* stmt)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeIf;
  p->left = cond;
  p->right = stmt;


  return p;
}

nodeType* CreateArrayRefferenceNode (nodeType* exp, const char* type, const char* varName)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeArrayRefference;
  strcpy(p->expType, type);
  sprintf(p->place, "%s[%s]", varName, exp->place);
  p->left = exp;
  p->right = NULL;


  return p;
}

nodeType* CreateVariableNode(TSymbolTreeNode* symbol)
{
	nodeType* p;
  size_t    nodeSize;

  /* выделить пам€ть дл€ узла */
  nodeSize = sizeof(nodeType);
  if (NULL == (p = (nodeType*)malloc(nodeSize)))
     my_yyerror("out of memory");

  /* установить значени€ полей */
  p->type = typeIdentifier;
  if (1 == symbol->isArray)
  {
  	strcpy(p->expType, "*");
  	strcat(p->expType, symbol->valueType->name);
  }
  else
  	strcpy(p->expType, symbol->valueType->name);
  strcpy(p->place, symbol->name);

  p->right = p->left = NULL;

  return p;

}


int codegenStructRef(nodeType* left, nodeType* right, const char* tmpVar)
{
	fprintf(outputFile, "\t%s\t:=\t%s.%s\n", tmpVar, left->place, right->place);
	 
	 return 0;
}

int codegenArrayRef(nodeType* var, nodeType* count, const char* tmpVar)
{
	fprintf(outputFile, "\t%s\t:=\t%s[%s]\n", tmpVar, var->place, count->place);
	 
	 return 0;
}

int main (int argc, char* argv[])
{
  //yydebug = 1;
  int yyparse();
  if (argc < 2)
  {
      printf("Too few parameters.\n");
      return EXIT_FAILURE;
  }
  if (NULL == freopen (argv[1], "r", stdin))
  {
      printf("Cannot open input file %s.\n", argv[1]);
      return EXIT_FAILURE;
  }
	
	outputFile = fopen("output", "w");
  lastSymbolTable = malloc(sizeof(TSymbolTablesListNode));
  lastSymbolTable->prevNode = NULL;
  lastSymbolTable->nextNode = NULL;
  CreateTree(" ");
  InitTypesTable();
  CycleStackInit();
  
  return yyparse();
}

int yyerror(char* errormessage)
{
    // ѕодавл€ем встроенное сообщение об ошибке
    // ¬место нее используетс€ my_yyerror (см.выше)
    return !0;
}

