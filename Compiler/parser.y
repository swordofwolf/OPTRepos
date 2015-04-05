%{
/* Based upon John Levine's book "flex&bison"*/
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symtable.h"
#include "types.h"
#include "labels.h"

CycleStack* cycleStack;
extern int isError;
extern int yylex();
extern FILE* outputFile;
DeclList* tmpTyp = 0;
int inCycle = 0;
%}

%verbose
%debug

%union
{
  nodeType* a;
  int i;
  float f;
  char string[255];
}

/* declare tokens */
%token <i> INTCONST 
%token <string> VARIABLE STRCONST
%token <f> FLOATCONST

%token IF ELSE DO WHILE FOR BREAK CONTINUE STRUCT GOTO FUNC UNION

%nonassoc IFX
%nonassoc ELSE


%nonassoc <string>CMP
%left '+' '-'
%left '*' '/'
%left <string> BOOLOP
%right '!'
%right REVERSE

%type <a> exp cond_stmt stmtlist statement stmtlist_tail do_stmt while_stmt for_stmt block assign declaration refference structrefference arrayrefference


%start prog

%%

prog : globallist
	{		
		CycleStackFree();
		//PrintTypesTable();
		FreeSymbolTable();
		FreeTypesTable();
	}
	;
	
globallist :  global global_tail ;

global_tail : %empty
	| globallist
	;
	
global : 
	struct_declaration
	| function
	| declaration
	;
	
function :
	FUNC VARIABLE VARIABLE marker_openblock '(' paramlist ')' '{' stmtlist '}'marker_closeblock
	{
		if (!isError)
			codegen($9);
		freeTree($9);
	}
	; 
	
paramlist :
	declaration paramlist
	| %empty
	;

struct_declaration :
	STRUCT VARIABLE marker_openblock '{' declaration_list '}' marker_closeblock
	{
		ValueType* newType = malloc(sizeof(ValueType));
		strcpy(newType->name, $2);
		DeclList* current = tmpTyp;
		for (int i = 0; i < MAX_TYPE_SIZE; i++)
		{	
			strcpy(newType->types[i], current->type);
			strcpy(newType->names[i], current->name);
			current = current->next;
			if (NULL == current)
				break;
		}
		
		AddTypeToTypesTable(newType);
		FreeTmpTypesTable(tmpTyp);
		tmpTyp = NULL;
	}
	;

declaration_list : 
	sdeclaration declarationlist_tail 
	;
	
declarationlist_tail : 
	%empty 
	| declaration_list ;
	
	sdeclaration :
	VARIABLE VARIABLE ';'
	{
		ValueType* type = GetType($1);
		if (NULL == type)
		{
			char errormsg[32];
			sprintf(errormsg, "unidentified type %s", $1);
			my_yyerror(errormsg);
			return;
		}
		
		TSymbolTreeNode* record = FindSymbol($2);
		if (NULL != record)
		{
			char errormsg[32];
			sprintf(errormsg, "redeclaration of variable %s", $2);
			my_yyerror(errormsg);
			return;
		}
		record = AddRecordToSymbolTable($2, type, 0);
		if (NULL == tmpTyp)
			tmpTyp = AddToTempType(NULL, $1, $2);
		else
		{
			AddToTempType(tmpTyp, $1, $2);
		}
	}
;


stmtlist : 
	statement stmtlist_tail  
	{ 
		if ($2 == NULL)
      $$ = $1;
    else
			$$ = CreateLNode($1, $2);
  }
	;

stmtlist_tail : %empty {$$ = NULL;}/* пустая строка */
        | stmtlist {$$ = $1;}
        ;

statement : 
		   exp ';' {$$ = $1;}
     | assign ';' {$$ = $1;}
     | declaration ';' {$$ = $1;}
     | cond_stmt {$$ = $1;} 	 
     | for_stmt  {$$ = $1;}	 
     | while_stmt  {$$ = $1;}
     | do_stmt  	 {$$ = $1;}
     | block			 {$$ = $1;}
     | BREAK ';'   
     {	
     	if (0 == inCycle)
     		my_yyerror("break outside cycle");
     	$$ = CreateGotoNode(GetEndLabel());
     }
     | CONTINUE ';' 
     {
     	if (0 == inCycle)
     		my_yyerror("continue outside cycle");
     	$$ = CreateGotoNode(GetBeginLabel());
     }
     | exp error { my_yyerror("; is expected");} 
     | assign error { my_yyerror("; is expected");}
     | VARIABLE ':'
     {
     	if (1 == isLabelDeclared($1))
     	{
				char errormsg[32];
				sprintf(errormsg, "redeclaration of label %s", $1);
				my_yyerror(errormsg);
				return;
     	}
     	char labelName[30];
     	strcpy(labelName, PutLabel($1));
     	$$ = CreateLabelNode(labelName);
     }
     | GOTO VARIABLE ';'
     	{
     		$$ = CreateManualGotoNode($2);
			}
     ;

declaration :
	VARIABLE VARIABLE 
	{
		ValueType* type = GetType($1);
		if (NULL == type)
		{
			char errormsg[32];
			sprintf(errormsg, "unidentified type %s", $1);
			my_yyerror(errormsg);
			return;
		}
		
		TSymbolTreeNode* record = FindSymbol($2);
		if (NULL != record)
		{
			char errormsg[32];
			sprintf(errormsg, "redeclaration of variable %s", $2);
			my_yyerror(errormsg);
			return;
		}

		record = AddRecordToSymbolTable($2, type, 0);
		$$ = CreateDeclarationNode(record);
	}
	| VARIABLE VARIABLE '[' exp ']' 
	{
		ValueType* type = GetType($1);
		if (NULL == type)
		{
			char errormsg[32];
			sprintf(errormsg, "unidentified type %s", $1);
			my_yyerror(errormsg);
			return;
		}
		
		TSymbolTreeNode* record = FindSymbol($2);
		if (NULL != record)
		{
			char errormsg[32];
			sprintf(errormsg, "redeclaration of variable %s", $2);
			my_yyerror(errormsg);
			return;
		}

		record = AddRecordToSymbolTable($2, type, 1);
		$$ = CreateDeclarationNode(record);
	}
;

assign :
	exp '=' exp
	{
		my_yyerror("lvalue required as left operand of assignment");
		$$ = NULL;
		return;
	} 
	| refference '=' exp
	{	
		if (0 != strcmp($1->expType, $3->expType))
		{
			my_yyerror("incompatible types");
			$$ = NULL;
			return;
		}
		$$ = CreateAssignNode($1, $3);
	}
;



refference :
	 structrefference {$$ = $1;}
	 | arrayrefference {$$ = $1;}
 	 | VARIABLE 
	 {
  	 	TSymbolTreeNode* symbol = FindSymbol($1);
  	 	if (NULL == symbol)
  	 	{
  	 		my_yyerror("undeclarated symbol");
  	 		return;
  	 	} 

  	 	$$ = CreateVariableNode(symbol);
	 }
	 ; 
	 
structrefference :
	VARIABLE '.' VARIABLE
	 {
	 	TSymbolTreeNode* record = FindSymbol($1);
		if (NULL == record)
		{
			char errormsg[32];
			sprintf(errormsg, "undeclarated variable %s", $1);
			my_yyerror(errormsg);
			$$ = NULL;
			return;
	 	}
	 	if (0 == isStructMember(record->valueType, $3))
	 	{
	 		char errormsg[32];
			sprintf(errormsg, "%s not a member of struct %s", $3, $1);
			my_yyerror(errormsg);
			$$ = NULL;
			return;
	 	}

		
	 	$$ = CreateStructRefferenceNode($1, $3);
		ValueType* t = record->valueType;
	 	strcpy($$->expType, GetMembersType(t, $3));
	 }
	;
	 
	 
arrayrefference :
	 VARIABLE '[' exp ']'
	 {
	 	TSymbolTreeNode* symbol = FindSymbol($1);
	 	if (NULL == symbol)
	 	{
	 		char errormsg[32];
			sprintf(errormsg, "undeclarated variable %s", $1);
			my_yyerror(errormsg);
			$$ = NULL;
			return;
	 	} 
	 	
	 	if (0 != strcmp ("int",  $3->expType))
	 	{
	 		my_yyerror("integer is expected in []");
	 		$$ = NULL;
	 		return;
	 	}

	 	if (0 == symbol->isArray)
	 	{
	 		char errormsg[32];
			sprintf(errormsg, "%s is not an array", $1);
			my_yyerror(errormsg);
			$$ = NULL;
			return;
	 	}

		$$ = CreateArrayRefferenceNode($3, symbol->valueType->name, $1);	
	 }
	 ;
	 


block : '{' marker_openblock stmtlist marker_closeblock '}' {$$ = $3;}  ;

incycle : %empty 
{
	inCycle++;
	char beginLabel[30];
	char endLabel[30];
	GetNextLabel(beginLabel);
	GetNextLabel(endLabel);
	CycleStackPut(beginLabel, endLabel); 

}
;
outcycle : %empty {inCycle--;}
;

for_stmt : FOR  '(' statement exp ';' statement ')' incycle statement outcycle 
{
	$$ = CreateForNode($3, $4, $6, $9);
	CycleStackGet();
}
;

while_stmt :  WHILE '(' exp ')' incycle statement outcycle  
{
	$$ = CreateWhileNode($3, $6);
	CycleStackGet();
};


do_stmt : DO incycle statement outcycle WHILE '(' exp ')' outcycle 
{
	$$ = CreateDoNode($7, $3);
	CycleStackGet();
};
     
marker_openblock : %empty
	{
		CreateTree(" ");
	}
	;
	
marker_closeblock : %empty
	{
		RecoverTree();
	}
  ;
    			

cond_stmt: 
	IF '(' exp ')' statement %prec IFX
	{
		$$ = CreateIfNode($3, $5);		
	}
  | IF '(' exp ')' statement ELSE statement
  {	
  	$$ = CreateIfElseNode($3, $5, $7);
  }
    ;


exp :	
	 '!' exp
	 {
	 		$$ = CreateUnOperNode("!", "boolean", "not");
	 }
	 | exp BOOLOP exp
	 {	
   		char tmpVar[30];
   		CreateTmpVariable(tmpVar);
			$$ = CreateBinOperNode($1, $3, "int", tmpVar, $2);
   }
	 | exp CMP exp
	 {
   		if (0 != strcmp($1->expType, $3->expType))
   		{
   			my_yyerror("incompatible types");
   			return;
   		}  		
   		char tmpVar[30];
   		CreateTmpVariable(tmpVar);
			$$ = CreateBinOperNode($1, $3, "int", tmpVar, $2);
   }   
   | exp '+' exp
   {
   		if (0 != strcmp($1->expType, $3->expType))
   		{
   			my_yyerror("incompatible types");
   			return;
   		}  		
   		char tmpVar[30];
   		CreateTmpVariable(tmpVar);
			$$ = CreateBinOperNode($1, $3, $1->expType, tmpVar, "+");
   }    
   | exp '-' exp
    {
   		if (0 != strcmp($1->expType, $3->expType))
   		{
   			my_yyerror("incompatible types");
   			return;
   		}  		
   		char tmpVar[30];
   		CreateTmpVariable(tmpVar);
			$$ = CreateBinOperNode($1, $3, $1->expType, tmpVar, "-");

   }   
   | exp '*' exp
   {
   		if (0 != strcmp($1->expType, $3->expType))
   		{
   			my_yyerror("incompatible types");
   			return;
   		}  		
   		char tmpVar[30];
   		CreateTmpVariable(tmpVar);
			$$ = CreateBinOperNode($1, $3, $1->expType, tmpVar, "*");
   }
   | exp '/' exp
   {
   		if (0 != strcmp($1->expType, $3->expType))
   		{
   			my_yyerror("incompatible types");
   			return;
   		}  		
   		char tmpVar[30];
   		CreateTmpVariable(tmpVar);
			$$ = CreateBinOperNode($1, $3, $1->expType, tmpVar, "/");
   }
   | '(' exp ')' {$$ = $2;}
   | INTCONST	{$$ = CreateIntConstantNode($1); }    
   | FLOATCONST {$$ = CreateFloatConstantNode($1); }
   | STRCONST		{$$ = CreateStringConstantNode($1); }
	 | refference {$$ = $1;}
;
	

%%
void printdebug(const char* str)
{
	fprintf(outputFile, "%s\n", str);
}
