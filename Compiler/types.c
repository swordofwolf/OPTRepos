#include "types.h"


#define NULL 0


void InitTypesTable()
{
	typesTable = malloc(sizeof(TypesTable));
	TypesTable* currentRecord = typesTable;
	
	ValueType* inttype = malloc(sizeof(ValueType));
	strcpy(inttype->name, "int");
	typesTable->type = inttype;
	currentRecord->next = malloc(sizeof(TypesTable));
	currentRecord = currentRecord->next;
	
	ValueType* stringtype = malloc(sizeof(ValueType));
	strcpy(stringtype->name, "string");
	currentRecord->type = stringtype;
	currentRecord->next = malloc(sizeof(TypesTable));
	currentRecord = currentRecord->next;
	
	ValueType* floattype = malloc(sizeof(ValueType));
	strcpy(floattype->name, "float");
	currentRecord->type = floattype;
	currentRecord->next = NULL;
}

TypesTable* GetLastType()
{
	TypesTable* current = typesTable;
	while(NULL != current->next)
	{
		current = current->next;
	}
	return current;
}

void AddTypeToTypesTable(ValueType* newType)
{
	TypesTable* lastRecord = GetLastType();
	lastRecord->next = malloc(sizeof(TypesTable));
	lastRecord = lastRecord->next;
	lastRecord->next = NULL;
	for (int i = 0; i < MAX_TYPE_SIZE; i++)
	{
		lastRecord->type = newType;
	}

	strcpy(lastRecord->type->name, newType->name);
}


void FreeTypesTable()
{
	TypesTable* next;
	while (NULL != typesTable)
	{
		next = typesTable->next;
		free(typesTable->type);
		free(typesTable);
		typesTable = next;
	}
}

int isStructMember(ValueType* type, const char* memName)
{
	for (int i = 0; i < MAX_TYPE_SIZE; i++)
	{
		if (0 == strcmp(type->names[i], memName))
		{
			return i + 1;
		}
	}
	return 0;
}

const char* GetMembersType(ValueType* type, const char* memName)
{
	for (int i = 0; i < MAX_TYPE_SIZE; i++)
	{
		if (0 == strcmp(type->names[i], memName))
		{
			return type->types[i];
		}
	}
	return NULL;
}

/* */
ValueType* GetType(const char* name)
{
	TypesTable* current = typesTable;
	while (NULL != current)
	{
		if (0 == strcmp(name, current->type->name))
		{
			return current->type;
			break;
		}
		current = current->next;
	}
	return NULL;
}


/* выводит таблицу символов, можно использовать для отладки */ 
void PrintTypesTable()
{
	TypesTable* current = typesTable;
	while (NULL != current)
	{
		printf("\nname: %s\n", current->type->name);
		for(int i = 0; i < 3; i++)
		{
			printf("%s %s ;\n", current->type->types[i], current->type->names[i]);
		}
		current = current->next;
	}
}


DeclList* AddToTempType(DeclList* table, const char* typeName, const char* memberName)
{
	if (NULL == table)
	{
		DeclList* newTable = malloc(sizeof(DeclList));
		strcpy(newTable->type, typeName);
		strcpy(newTable->name, memberName);
		newTable->next = NULL;
		return newTable;
	}
	DeclList* last = table;
	while(NULL != last->next)
	{
			last = last->next;
	}
		
	last->next = malloc(sizeof(DeclList));
	strcpy(last->next->type, typeName);
	strcpy(last->next->name, memberName);
	last->next->next = NULL;
	return last->next;
}

void FreeTmpTypesTable(DeclList* table)
{
	DeclList* next;
	while (NULL != table)
	{
		next = table->next;
		free(table);
		table = next;
	}
}
