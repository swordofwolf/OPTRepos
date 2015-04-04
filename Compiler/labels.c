#include "labels.h"

Label Labels[255];
int iiLabels = 0;
int mlabelCount = 0;
extern CycleStack* cycleStack;

void GetNextLabel(char* dest)
{
	sprintf(dest, "L%d", mlabelCount++);
}

const char* FindLabelCodeName(const char* originalName)
{
	for(int i = 0; i < 255; i++)
	{
		if (0 == strcmp(originalName, Labels[i].originalName))
			return Labels[i].codeName;
	}
	return 0;
}


void CycleStackInit()
{
		cycleStack = malloc(sizeof(CycleStack));
		cycleStack->next = NULL;
		cycleStack->prev = NULL;
}

void CycleStackFree()
{
	free(cycleStack);
}

void CycleStackPut(const char* begin, const char* end)
{
	cycleStack->next = malloc(sizeof(CycleStack));
	cycleStack->next->prev = cycleStack;
	cycleStack = cycleStack->next;
	cycleStack->next = NULL;
	strcpy(cycleStack->begin, begin);
	strcpy(cycleStack->end, end);
}
void CycleStackGet()
{
	cycleStack = cycleStack->prev;
	free(cycleStack->next);
	cycleStack->next = NULL;
}


char* GetBeginLabel()
{
	return cycleStack->begin;
}

char* GetEndLabel()
{
	return cycleStack->end;
}

char* PutLabel(const char* originalName)
{
	char newLabelName[30];
	GetNextLabel(newLabelName);
	strcpy(Labels[iiLabels].originalName, originalName);
	strcpy(Labels[iiLabels].codeName, newLabelName);
	iiLabels++;
	return Labels[iiLabels - 1].codeName;
}

int isLabelDeclared(const char* labelName)
{
	for(int i = 0; i < 255; i++)
	{
		if (0 == strcmp(labelName, Labels[i].originalName))
			return 1;
	}
	return 0;
}











