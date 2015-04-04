#ifndef _TYPES_H
#define _TYPES_H

#define MAX_TYPE_SIZE 250
#define MAX_TYPENAME_SIZE 250

struct TypesTable;

/*структура для представления пользовательских типов. Хранит в себе имя типа, имена и типы составных частей типа */
typedef struct ValueType
{
	char types[MAX_TYPE_SIZE][MAX_TYPENAME_SIZE];
	char names[MAX_TYPE_SIZE][MAX_TYPENAME_SIZE];
	char name[MAX_TYPENAME_SIZE];
} ValueType;

typedef struct TypesTable
{
	ValueType* type;
	struct TypesTable* next;

} TypesTable;

TypesTable* typesTable;

void InitTypesTable();
void AddTypeToTypesTable(ValueType* newType);
void FreeTypesTable();
ValueType* GetType(const char* name);
TypesTable* GetLastType();
void PrintTypesTable();
const char* GetMembersType(ValueType* type, const char* memName);
int isStructMember(ValueType* type, const char* memName);

typedef struct DeclList
{
	char type[MAX_TYPENAME_SIZE];
	char name[MAX_TYPENAME_SIZE];
	struct DeclList* next;
} DeclList;

DeclList* AddToTempType(DeclList* table, const char* typeName, const char* memberName);
void FreeTmpTypesTable(DeclList* table);

#endif
