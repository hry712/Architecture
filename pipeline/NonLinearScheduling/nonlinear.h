#ifndef _NONLINEAR_H_
#define _NONLINEAR_H_

#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct StateNode{
	unsigned int _ConflictVectorValue;
	unsigned int _IntervalTicks;
} StateNode;

typedef struct AvgTicksItem{
	int _Site;
	float _AvgTicks;
} AvgTicksItem;

void SetForbiddenTable(unsigned int* SrcRow, int Len, unsigned int* Res);
void InitStack(StateNode* Stack, int* StackTop, unsigned int ConflictVectorValue, unsigned int TickAmountSetting);
int GenerateSubVectorList(unsigned int CurrentVector, const unsigned int InitVector, StateNode* SubVectorList);
int IsInStack(StateNode* Stack, int* StackTop, unsigned int CurrentVector);
int FindVectorInStack(StateNode* Stack, int* StackTop, unsigned int CurrentVector);
int SaveOnePath(StateNode* Stack, int CoincideAt, int* StackTop, StateNode PathsBuffer[][20], int PathCount);
void Push(StateNode* Stack, int* StackTop, StateNode NewNode);
StateNode Pop(StateNode* Stack, int* StackTop);
void DFS(StateNode* Stack, int* StackTop, const unsigned int InitVector, const unsigned int TickAmountSetting, unsigned int CurrentVector, StateNode PathsBuffer[][20], int* PathCount);
int FindMinAvgIntervalTicks(StateNode PathBuffer[][20], int PathCount, const int TickAmountSetting);
void CalcAvgIntervalTicks(StateNode PathsBuffer[][20], int PathCount, float* AvgTicksBuffer);
int GenerateMinAvgIntervalsList(float* AvgTicksBuffer, int PathCount, int* Candidates);
int FindMinThroughputRate(int* Candidates, int Len, StateNode PathsBuffer[][20], const int TickAmountSetting);

int FindMinThroughputRate(int* Candidates, int Len, StateNode PathsBuffer[][20], const int TickAmountSetting){
	int i = 0;
	int j = 0;
	int k = 0;
	int TotalTicks[30] = {0};
	int minSite = 0;
	int minTicks = 100;

	for (; i < Len; i++){
		k = 0;
		for (j = 0; j < TickAmountSetting-2; j++){ 
			if (PathsBuffer[Candidates[i]][k]._ConflictVectorValue == 0)
				k = 0;
			TotalTicks[Candidates[i]] += PathsBuffer[Candidates[i]][k++]._IntervalTicks;
		}
		TotalTicks[Candidates[i]] += TickAmountSetting;
#ifdef PRINTDETAILS
		printf("Show throughput traffic at site: %d\n", Candidates[i]);
		printf("Traffic: %d\n", TotalTicks[Candidates[i]]);
#endif
		if (TotalTicks[Candidates[i]] < minTicks){
			minTicks = TotalTicks[Candidates[i]];
			minSite = Candidates[i];
		}
	}

	return minSite;
}

int GenerateMinAvgIntervalsList(float* AvgTicksBuffer, int PathCount, int* Candidates){
	int i = 0;
	int j = 0;
	float minIntervalTicks = 100;

	for (; i < PathCount; i++){
		if (AvgTicksBuffer[i] < minIntervalTicks)
			minIntervalTicks = AvgTicksBuffer[i];
	}

	for (i = 0; i < PathCount; i++)
		if (fabs(minIntervalTicks - AvgTicksBuffer[i]) < 0.0001)
			Candidates[j++] = i;
	return j;

}

void CalcAvgIntervalTicks(StateNode PathsBuffer[][20], int PathCount, float* AvgTicksBuffer){
	int i = -1;
	int j = 0;
	float sum = 0;

	if (-1 == PathCount)
		return;
	else {
		for (i = 0; i < PathCount; i++){
			while (PathsBuffer[i][j]._ConflictVectorValue)
				sum += PathsBuffer[i][j++]._IntervalTicks;
			AvgTicksBuffer[i] = sum / j;
			j = 0;
			sum = 0;
		}
	}
}

#ifdef PRINTDETAILS
void PrintPath(StateNode Paths[][20], int Len){
	int i = 0;
	int j = 0;
	printf("Show Paths Content:\n");
	for (; i<Len; i++){
		while (Paths[i][j]._ConflictVectorValue)		
			printf("(%d, %d)  ", Paths[i][j]._ConflictVectorValue, Paths[i][j++]._IntervalTicks);
		j = 0;
		printf("\n");
	}
}
void PrintSubVector(StateNode* SubList, int Amount){
	int i = 0;
	printf("Show the Sub Vectors below:\n");
	for (; i<=Amount; i++){
		printf("(%d, %d) ", SubList[i]._ConflictVectorValue, SubList[i]._IntervalTicks);
	}
	printf("\n");
}

void PrintStack(StateNode* Stack, const int* StackTop){
	int i = 0;
	printf("Show the Current Stack Contents:\n");
	for (; i<=*StackTop; i++){
		printf("(%d, %d) ", Stack[i]._ConflictVectorValue, Stack[i]._IntervalTicks);
	}
	printf("\n");
}
#endif


int FindMinAvgIntervalTicks(StateNode PathBuffer[][20], int PathCount, const int TickAmountSetting){
	int res;
	int minavgIntervalListLength = 0;
	int candidates[20];
	float avgticks[20];

	CalcAvgIntervalTicks(PathBuffer, PathCount, avgticks);
	minavgIntervalListLength = GenerateMinAvgIntervalsList(avgticks, PathCount, candidates);
	res = FindMinThroughputRate(candidates, minavgIntervalListLength, PathBuffer, TickAmountSetting);

	return res;
}


void InitStack(StateNode* Stack, int* StackTop, unsigned int ConflictVectorValue, unsigned int TickAmountSetting){
	(*StackTop) += 1;
	Stack[*StackTop]._ConflictVectorValue = ConflictVectorValue;
	Stack[*StackTop]._IntervalTicks = TickAmountSetting;
}


int GenerateSubVectorList(unsigned int CurrentVector, const unsigned int InitVector, StateNode* SubVectorList){
	int i = 1;
	int j = -1;
	unsigned int tmp = CurrentVector;
	unsigned int nextvector = 0;

	while (tmp){
		if (0 == tmp%2){
			nextvector = (tmp>>1)|InitVector;
			j += 1;
			SubVectorList[j]._ConflictVectorValue = nextvector;
			SubVectorList[j]._IntervalTicks = i;
		}
		tmp /= 2;
		++i;
	}

	return j;
}

int IsInStack(StateNode* Stack, int* StackTop, unsigned int CurrentVector){
	int i = 0;
	for (; i<=*StackTop; i++){
		if (CurrentVector == Stack[i]._ConflictVectorValue)
			return 1;
	}

	if (i>*StackTop)
		return 0;
}

int FindVectorInStack(StateNode* Stack, int* StackTop, unsigned int CurrentVector){
	int i = 0;
	for (; i<=*StackTop; i++)
		if (CurrentVector == Stack[i]._ConflictVectorValue)
				return i;

	if (i>*StackTop)
		return -1;
}

int SaveOnePath(StateNode* Stack, int CoincideAt, int* StackTop, StateNode PathsBuffer[][20], int PathCount){
	int i = CoincideAt;	
	int j = 0;
	for (; i<=*StackTop; i++, j++){
		PathsBuffer[PathCount][j] = Stack[i];
	}
	
	if (1 == (*StackTop - CoincideAt)){
		PathCount += 1;
		PathsBuffer[PathCount][0] = Stack[*StackTop];
		PathsBuffer[PathCount][1] = Stack[*StackTop-1];
	} 
	return PathCount+1;
}

void Push(StateNode* Stack, int* StackTop, StateNode NewNode){
	(*StackTop) += 1;
	Stack[*StackTop] = NewNode;
}

StateNode Pop(StateNode* Stack, int* StackTop){
	StateNode res = Stack[*StackTop];
	Stack[*StackTop]._ConflictVectorValue = 0;
	Stack[*StackTop]._IntervalTicks = 0;
	(*StackTop) -= 1;
	return res;
}

void DFS(StateNode* Stack, int* StackTop, const unsigned int InitVector, const unsigned int TickAmountSetting, unsigned int CurrentVector, StateNode PathsBuffer[][20], int* PathCount){
	int i;
	int j;
	int CoincideAt;
	int SubVectorNum = 0;
	unsigned int nextVector;
	StateNode SubVectorList[20];
	StateNode tmpNode;
	memset(SubVectorList, 0, sizeof(StateNode)*20);
	if (-1 == *StackTop){
		InitStack(Stack, StackTop, CurrentVector, TickAmountSetting);
		SubVectorNum = GenerateSubVectorList(CurrentVector, InitVector, SubVectorList);
#ifdef PRINTDETAILS
		PrintSubVector(SubVectorList, SubVectorNum);
#endif
		if (-1 == SubVectorNum){
			*PathCount = SaveOnePath(Stack, 0, StackTop, PathsBuffer, *PathCount);
#ifdef PRINTDETAILS
			printf("Afer Save Path:\n");
			PrintPath(PathsBuffer, *PathCount);
#endif

			tmpNode = Pop(Stack, StackTop);
#ifdef PRINTDETAILS
			printf("After Pop Opeartion\n");
			PrintStack(Stack, StackTop);
#endif
		} else {
			for (i = 0; i <= SubVectorNum; i++){
				if (IsInStack(Stack, StackTop, SubVectorList[i]._ConflictVectorValue)){
					CoincideAt = FindVectorInStack(Stack, StackTop, SubVectorList[i]._ConflictVectorValue);
#ifdef PRINTDETAILS
					printf("Find coinciding at site %d\n", CoincideAt);
#endif
					*PathCount = SaveOnePath(Stack, CoincideAt, StackTop, PathsBuffer, *PathCount);
					tmpNode._ConflictVectorValue = InitVector;
					tmpNode._IntervalTicks = TickAmountSetting;
					Push(Stack, StackTop, tmpNode);
					*PathCount = SaveOnePath(Stack, CoincideAt, StackTop, PathsBuffer, *PathCount);
					Pop(Stack, StackTop);
#ifdef PRINTDETAILS
			printf("Afer Save Path:\n");
			PrintPath(PathsBuffer, *PathCount);
			printf("Afer Save Path:\n");
			PrintPath(PathsBuffer, *PathCount);
#endif
				} else {
					Push(Stack, StackTop, SubVectorList[i]);
#ifdef PRINTDETAILS
					printf("After Push operation\n");
					PrintStack(Stack, StackTop);
#endif
					DFS(Stack, StackTop, InitVector, TickAmountSetting, SubVectorList[i]._ConflictVectorValue, PathsBuffer, PathCount);
				}
			}
			Pop(Stack, StackTop);
#ifdef PRINTDETAILS
			printf("After Pop Opeartion\n");
			PrintStack(Stack, StackTop);
#endif
		}
	} else {
		SubVectorNum = GenerateSubVectorList(CurrentVector, InitVector, SubVectorList);
#ifdef PRINTDETAILS
		PrintSubVector(SubVectorList, SubVectorNum);
#endif
		if (-1 == SubVectorNum){
			*PathCount = SaveOnePath(Stack, 0, StackTop, PathsBuffer, *PathCount);
			Pop(Stack, StackTop);
#ifdef PRINTDETAILS
			printf("Afer Save Path:\n");
			PrintPath(PathsBuffer, *PathCount);
			printf("After Pop Opeartion\n");
			PrintStack(Stack, StackTop);
#endif
		} else {
			for (i = 0; i <= SubVectorNum; i++){
				if (IsInStack(Stack, StackTop, SubVectorList[i]._ConflictVectorValue)){
					CoincideAt = FindVectorInStack(Stack, StackTop, SubVectorList[i]._ConflictVectorValue);
#ifdef PRINTDETAILS
					printf("Find coinciding at site %d\n", CoincideAt);
#endif
					*PathCount = SaveOnePath(Stack, CoincideAt, StackTop, PathsBuffer, *PathCount);
					tmpNode._ConflictVectorValue = InitVector;
					tmpNode._IntervalTicks = TickAmountSetting;
					Push(Stack, StackTop, tmpNode);
					*PathCount = SaveOnePath(Stack, CoincideAt, StackTop, PathsBuffer, *PathCount);
					Pop(Stack, StackTop);
#ifdef PRINTDETAILS
			printf("Afer Save Path:\n");
			PrintPath(PathsBuffer, *PathCount);
#endif
				} else {
					Push(Stack, StackTop, SubVectorList[i]);
#ifdef PRINTDETAILS
					printf("After Push operation\n");
					PrintStack(Stack, StackTop);
#endif
					DFS(Stack, StackTop, InitVector, TickAmountSetting,  SubVectorList[i]._ConflictVectorValue, PathsBuffer, PathCount);
				}
			}
			Pop(Stack, StackTop);
#ifdef PRINTDETAILS
			printf("After Pop Opeartion\n");
			PrintStack(Stack, StackTop);
#endif
		}
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
