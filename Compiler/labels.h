#ifndef QWE_LABELS_H
#define QWE_LABELS_H

#define NULL 0

typedef struct
{
	char originalName[30];
	char codeName[30];
} Label;

typedef struct CycleStack
{
	char begin[30];
	char end[30];
	struct CycleStack* next;
	struct CycleStack* prev;
} CycleStack;

char* PutLabel(const char* originalName);
int isLabelDeclared(const char* labelName);
void GetNextLabel(char* dest);
const char* FindLabelCodeName(const char* originalName);

void CycleStackInit();
void CycleStackFree();
void CycleStackPut(const char* begin, const char* end);
void CycleStackGet();
char* GetBeginLabel();
char* GetEndLabel();




#endif
