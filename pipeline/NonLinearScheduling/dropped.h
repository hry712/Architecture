#ifndef _NONLINEAR_H_
#define _NONLINEAR_H_
#include <string.h>

int pathcount;
unsigned int Paths[20][20];

typedef struct State{
	unsigned int ConflictVector;
	unsigned int timeticks;
} State;

void PopVectorStack(State* Stack, unsigned int* StackCursor);
void PushVectorStack(State * Stack, unsigned int * StackTop, State NewNode);
void PrintStack(State * Stack, unsigned int StackTop);
void PrintAvailConflictList(State * CfltList, int Len);
void GenerateConflictVectorList(State * Stack, unsigned int StackTop, const unsigned int TimeTickLimit, const unsigned int InitCfltVector, State * CfltVctrList, int* VctrNum);

int IsInVectorStack(State* Stack, int Len, unsigned int Vector){
	int i = 0;
	if (Stack[0].timeticks == 0)
		return -1;
	for (; i<Len; i++){
		if (Stack[i].ConflictVector == Vector)
			return i;
	}
	if (i == Len)
		return -1;
}

void SaveCurrentPath(State* Stack, unsigned int StackCursor, unsigned int Paths[][15], unsigned int PathCount){
	int i = 0;
	for (; i <= StackCursor; i++ ){
		Paths[PathCount][i] = Stack[i].timeticks;
	}
}

void PopVectorStack(State* Stack, unsigned int* StackCursor){
	if (*StackCursor == 0)
		return;
	else {
		Stack[*StackCursor].ConflictVector = 0;
		Stack[*StackCursor].timeticks = 0;
		(*StackCursor) -= 1;
	}
}

void PrintAvailConflictList(State * CfltList, int Len){
	int i = 0;
	printf("Print avaliable conflict list:\n");
	for (; i<=Len; i++)
		printf("(%d, %d) ", CfltList[i].ConflictVector, CfltList[i].timeticks);
	printf("\n");
}

void SavePath(State* Stack, unsigned int* StackTop, unsigned int BeginAt, unsigned int PathsBuffer[][20], int* Count){
	int i = 0;
	int begin = BeginAt;
	(*Count) += 1;
	for (; begin<=*StackTop; i++, begin++){
		PathsBuffer[*Count][i] = Stack[begin].timeticks;
	}
}

void DFSforDirectedGraph(State* Stack, unsigned int* StackTop, const unsigned int InitCfltVector, const unsigned int TimeTickLimit){
	State AvailableConflictVectorList[10];
	State TmpVector;
	memset(AvailableConflictVectorList, 0, sizeof(State)*10);
	int VectorNum = -1;
	int ConflictCycleBeginAt = -1;
	int i = 0;
	int j = 0;
#ifdef DEBUG
	printf("The Stack Top Conflict Vector value is: %d\n", Stack[*StackTop]);
#endif
	GenerateConflictVectorList(Stack, *StackTop, TimeTickLimit, InitCfltVector, AvailableConflictVectorList, &VectorNum);
#ifdef DEBUG
	printf("The generated vector number is: %d\n", VectorNum);
	PrintAvailConflictList(AvailableConflictVectorList, VectorNum);	
#endif
	if (-1 == VectorNum){
#ifdef DEBUG			
				printf("We find a cycle here:\n");
				for (i = 0; i<=*StackTop; i++)
					printf("(%d, %d) ", Stack[i].ConflictVector, Stack[i].timeticks);
				printf("\n");
#endif
		SavePath(Stack, StackTop, 0, Paths, &pathcount);
		PopVectorStack(Stack, StackTop);
	} else {
		for (i = 0; i<=VectorNum; i++){
			ConflictCycleBeginAt = IsInVectorStack(Stack, *StackTop, AvailableConflictVectorList[i].ConflictVector);
			if (-1 != ConflictCycleBeginAt){

				// make a loop and search ror all possible paths
#ifdef DEBUG			
				printf("We find a cycle here:\n");
				for (j = ConflictCycleBeginAt; j<=*StackTop; j++)
					printf("(%d, %d) ", Stack[j].ConflictVector, Stack[j].timeticks);
				printf("\n");
#endif
				SavePath(Stack, StackTop, ConflictCycleBeginAt, Paths, &pathcount);
				j = *StackTop - ConflictCycleBeginAt + 1;
				while (j--)
					PopVectorStack(Stack, StackTop);

			} else {
				TmpVector.ConflictVector = AvailableConflictVectorList[i].ConflictVector | InitCfltVector;
				TmpVector.timeticks = AvailableConflictVectorList[i].timeticks;
				PushVectorStack(Stack, StackTop, TmpVector);
				PrintStack(Stack, *StackTop);
				DFSforDirectedGraph(Stack, StackTop, InitCfltVector, TimeTickLimit);
				PopVectorStack(Stack, StackTop);
			}
		}
	}
	//PrintStack(Stack, StackTop);
}

void PrintStack(State * Stack, unsigned int StackTop){
	int i = 0;
	printf("Print stack data below: (the top site is %d)\n", StackTop);
	for (; i<=StackTop; i++)
		printf("(%d, %d) ", Stack[i].ConflictVector, Stack[i].timeticks);
	printf("\n");
}

void PushVectorStack(State * Stack, unsigned int * StackTop, State NewNode){
	(*StackTop) += 1;
	Stack[(*StackTop)] = NewNode;
}

void GenerateConflictVectorList(State * Stack, unsigned int StackTop, const unsigned int TimeTickLimit, const unsigned int InitCfltVector, State * CfltVctrList, int* VctrNum){
	int i = 1;
	unsigned int conflictvctr = Stack[StackTop].ConflictVector;
	while (conflictvctr!=0){
		if ((conflictvctr%2) == 0){
			(*VctrNum) += 1;
			CfltVctrList[*VctrNum].ConflictVector = (conflictvctr>>1)|InitCfltVector;
			CfltVctrList[*VctrNum].timeticks = i;
			printf("the current conflict number is: %d\n", CfltVctrList[*VctrNum].ConflictVector);
			printf("the site number is: %d\n", i);
		}
		conflictvctr /= 2;
		++i;
	}
}

void SetForbiddenTable(unsigned int* SrcRow, int Len, unsigned int* Res){
	int i = 0;
	int j = 0;
	int lasttag = -1;
	int toEnd = -1;
	int * tmp = (int *)malloc(sizeof(int)*Len);
	for (; i<Len; i++)
		tmp[i] = -1;
//	memset(tmp, -1, sizeof(int)*Len);
#ifdef DEBUG
	int t = 0;
	printf("Let me see what happened when forbidden table coming into here:\n");
	while (t<Len-1)
		printf("%d ", Res[t++]);
	printf("\n");
#endif
	for (i = 0; i<Len; i++){
		if (SrcRow[i] == 1){
			tmp[j++] = i;	
		}
	}
	if (tmp[0] == -1)
		return;
	else {
#ifdef DEBUG
	printf("tmp arr middle value:\n");
	for (i = 0; i < j; i++)
		printf("%d ", tmp[i]);
	printf("\n");
	printf("tmp len is: %d\n", j);
#endif
		lasttag = 0;
loop:
		for (i = lasttag+1; i<j; i++){
#ifdef DEBUG
			printf("amount of value 1 is: %d\n", j);
			printf("tmp left is: %d\ntmp right is: %d\n", tmp[i], tmp[lasttag]);
#endif
			Res[tmp[i]-tmp[lasttag]-1] = 1;	
#ifdef DEBUG
	int d = 0;
	printf("middle value of forbidden table:\n");
	while (d<Len-1)
		printf("%d ", Res[d++]);
	printf("\n");
#endif
		}
		lasttag += 1;
#ifdef DEBUG
	int t = 0;
	printf("middle value of forbidden table:\n");
	while (t<Len-1)
		printf("%d ", Res[t++]);
	printf("\n");
#endif
		if (i<j)
			goto loop;
	}
	free(tmp);
}

#endif
