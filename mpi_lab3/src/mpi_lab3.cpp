//============================================================================
// Name        : mpi_lab3.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <stdio.h>
#include "mpi.h"
using namespace std;
int main(int argc, char **argv) {
	int numProc, rankProc;
	int *messages;
	messages = new int[numProc];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rankProc);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	for (int i = 0; i < numProc; i++) {
		*(messages + i) = i;
	}
	MPI_Status Status;
	int *message = new int;
	int m = 3;
	for (int k = 0; k < m; k++) {
		if (rankProc == 0) {
			MPI_Scatter(messages, 1, MPI_INT, message, 1, MPI_INT, 0,
			MPI_COMM_WORLD);
			printf("Нулевой процесс совершил рассылку в цикле  %d \n", k);
			MPI_Gather(message, 1, MPI_INT, messages, 1, MPI_INT, 0,
			MPI_COMM_WORLD);
			printf("Родительский процесс собрал данные в цикле  %d , messages[1] = %d\n", k, *(messages + 1));
			//printf("Родительский процесс собрал данные в цикле  %d \n", k);
		} else {
			MPI_Scatter(messages, 1, MPI_INT, message, 1, MPI_INT, 0,
			MPI_COMM_WORLD);
			//MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
			//MPI_COMM_WORLD, &Status);
			printf("Процесс %d получил сообщение от родительского процесса %d\n",
					rankProc, *message);
			*message += rankProc;
			MPI_Gather(message, 1, MPI_INT, messages, 1, MPI_INT, 0,
			MPI_COMM_WORLD);
			//MPI_Send(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			//printf("Процесс %d отправил ответ родительскому процессу\n",
			//rankProc);
		}
	}
	MPI_Finalize();
}

