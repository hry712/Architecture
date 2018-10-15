#include <stdio.h>
#include <stdlib.h>
#include "nonlinear.h"

#define StackSize 15

int main(void){
	unsigned int funcs = 0;
	unsigned int timeticks = 0;
	unsigned int **apttable = NULL;
	unsigned int *forbiddentable = NULL;
	unsigned int InitConflictVector = 0;
	unsigned int ConflictVector = 0;
	unsigned int StackCursor = 0;
	int MinimumAvgSite = -1;
	StateNode Paths[20][20];
	StateNode VectorStack[StackSize];
	memset(Paths, 0, sizeof(StateNode)*20*20);
	memset(VectorStack, 0, sizeof(StateNode)*StackSize); 

	printf("input function number and time tick number(seperated with space): ");
	scanf("%d %d", &funcs, &timeticks);
	apttable = (unsigned int **)malloc(sizeof(unsigned int*)*funcs);
	int i = 0;
	for (i = 0; i<timeticks; i++){
		apttable[i] = (unsigned int*)malloc(sizeof(unsigned int)*timeticks);
	}
	forbiddentable = (unsigned int*)malloc(sizeof(unsigned int)*(timeticks-1));
	
#ifdef DEBUG
	printf("The initial value of forbidden table:\n");
	int d = 0;
	while (d<timeticks-1)
		printf("%d ", forbiddentable[d++]);
	printf("\n");
#endif
	for (i = 0; i<timeticks-1; i++){
		forbiddentable[i] = 0;
		printf("site %d of forbiddentable ,value: %d\n", i, forbiddentable[i]);
	}

	// Input appointing table
	printf("Input appointing table(value 0-1):\n");
	int j = 0;
	for (i = 0; i<funcs; i++){
		for (j=0; j<timeticks; j++){
			scanf("%d", &apttable[i][j]);
		}
#ifdef DEBUG
	int t = 0;
	printf("middle value of apttable:\n");
	while (t<timeticks)
		printf("%d ", apttable[i][t++]);
	printf("\n");

	printf("***forbiddentable value print here:\n");
	t = 0;
	while (t<timeticks-1)
		printf("%d ", forbiddentable[t++]);
	printf("\n");
#endif
		SetForbiddenTable(apttable[i], timeticks, forbiddentable);
	}

	// Generating 

#ifdef DEBUG
	for (i=0; i<timeticks-1; i++)
		printf("%d ", forbiddentable[i]);
	printf("\n");
#endif
	// Generating initial Conflict Vector	
	for (i = 0; i<timeticks-1; i++)
		if (forbiddentable[i])
			InitConflictVector += 1<<i;
#ifdef DEBUG
	printf("Initial Conflict Vector is: %d\n", InitConflictVector);
#endif
	StackCursor = -1;
	int pathcount = 0;
	memset(Paths, 0, sizeof(unsigned int)*20*20);
	DFS(VectorStack, &StackCursor, InitConflictVector, timeticks, InitConflictVector, Paths, &pathcount);
#ifdef DEBUG
	printf("Show the Paths content:\n");
	i = 0;
	j = 0;
	for (; i< pathcount; i++){ 
		while (Paths[i][j]._ConflictVectorValue)
			printf("(%d, %d)  ", Paths[i][j]._ConflictVectorValue, Paths[i][j++]._IntervalTicks);
		j = 0;
		printf("\n");
	}
#endif

	MinimumAvgSite = FindMinAvgIntervalTicks(Paths, pathcount, timeticks);
	printf("Find the minimum average ticks at site %d:\n", MinimumAvgSite);
	j = 0;
		while (Paths[MinimumAvgSite][j]._ConflictVectorValue)
			printf("(%d, %d)  ", Paths[MinimumAvgSite][j]._ConflictVectorValue, Paths[MinimumAvgSite][j++]._IntervalTicks);
		printf("\n");

	return 0;
}
