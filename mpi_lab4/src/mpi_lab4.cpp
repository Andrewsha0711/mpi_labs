//============================================================================
// Name        : mpi_lab4.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include "mpi.h"
using namespace std;

string MultiplyNumbers(string x, string y) {
	int x_numbers[x.length()] = { 0 };
	int x_zeros[x.length()] = { 0 };
	int y_numbers[y.length()] = { 0 };
	int y_zeros[y.length()] = { 0 };
	int result_numbers[x.length() * y.length()] = { 0 };
	int result_zeros[x.length() * y.length()] = { 0 };

	for (int i = 0; i < x.length(); i++) {
		x_numbers[i] = x[i] - '0';
		x_zeros[i] = x.length() - i - 1;
		y_numbers[i] = y[i] - '0';
		y_zeros[i] = y.length() - i - 1;

	}
	for (int i = 0; i < x.length(); i++) {
		for (int j = 0; j < y.length(); j++) {
			result_numbers[i * x.length() + j] = x_numbers[i] * y_numbers[j];
			result_zeros[i * x.length() + j] = x_zeros[i] + y_zeros[j];
		}
	}

	int res_numbers[x.length() + y.length() + 1] = { 0 };
	int res_zeros[x.length() + y.length() + 1] = { 0 };

	for (int i = 0; i < x.length() + y.length(); i++) {
		for (int j = 0; j < sizeof(result_numbers) / sizeof(int); j++) {
			if (result_zeros[j] == i) {
				res_numbers[i] += result_numbers[j];
				res_numbers[i + 1] += res_numbers[i] / 10;
				res_numbers[i] = res_numbers[i] % 10;
				res_zeros[i] = i;
			}
		}
	}
	string answer = "";
	for (int i = x.length() + y.length(); i > -1; i--) {
		if (res_numbers[i] != 0 || res_zeros[i] != 0) {
			answer += to_string(res_numbers[i]);
		}
	}
	return answer;
}

int main(int argc, char **argv) {
	int numProc, rankProc;
	string source[] = { "213456", "456789", "123456", "123456" };
	int a = sizeof(source) / sizeof(source[0]);
	int l = source[0].length();
	int size = a * l;
	char numbers[a][size];
	for (int k = 0; k < a; k++) {
		for (int i = 0; i < size - l; i++) {
			numbers[k][i] = '0';
		}
		for (int i = size-l; i < size; i++) {
			numbers[k][i] = (source[k])[i-(size-l)];
		}
	}
//	string x = "123456789";
//	string y = "123456789";

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rankProc);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	MPI_Datatype bigInteger;
	MPI_Type_contiguous(size, MPI_CHAR, &bigInteger);
	MPI_Type_commit(&bigInteger);

	if (rankProc == 0) {
		for (int j = 0; j < a - 1; j++) {
			int k = 0;
			for (int i = 1; i < numProc; i++) {
				MPI_Send(numbers[k], 1, bigInteger, i, 0, MPI_COMM_WORLD);
//				cout << "Send to proc " << i << " " << string(numbers[k])
//						<< endl;
//				cout << "Send to proc " << i << " "
//						<< string(numbers[k + 1]) << endl;
				MPI_Send(numbers[k + 1], 1, bigInteger, i, 0, MPI_COMM_WORLD);
				k += 2;
				//			MPI_Send(numbers[a-1-i], i, bigInteger, 1, 0, MPI_COMM_WORLD);
			}
			k = 0;
			for (int i = 1; i < numProc; i++) {
				char result[size];
				MPI_Recv(result, 1, bigInteger, i, 0, MPI_COMM_WORLD, &status);
				for (int j = 0; j < size; j++) {
					numbers[k][j] = result[j];
				}
//				cout << string(numbers[k]) << endl;
				k++;
			}
			/*
			 for (int i = 1; i < numProc; i += 2) {
			 char result1[size];
			 char result2[size];
			 MPI_Recv(result1, 1, bigInteger, i, 0, MPI_COMM_WORLD, &status);
			 MPI_Recv(result2, 1, bigInteger, i + 1, 0,
			 MPI_COMM_WORLD, &status);
			 string res = MultiplyNumbers(string(result1), string(result2));
			 char result[size];
			 for (int i = 0; i < size - res.length(); i++) {
			 result[i] = '0';
			 }
			 for (int i = size - res.length(); i < size; i++) {
			 result[i] = res[i - (size - res.length())];
			 }
			 for (int j = 0; j < size; j++) {
			 numbers[i][j] = result[j];
			 }
			 //				cout << string(numbers[i]) << endl;
			 }
			 */
		}
	} else {
		for (int j = 0; j < a - 1; j++) {
			char x[size];
			char y[size];
			MPI_Recv(x, 1, bigInteger, 0, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(y, 1, bigInteger, 0, 0, MPI_COMM_WORLD, &status);
			string strx = "", stry = "";
			for (int i = 0; i < size; i++) {
				strx += x[i];
				stry += y[i];
			}
			cout << "x " << strx << endl;
			cout << "y" << stry << endl;
			string res = MultiplyNumbers(strx, stry);
			char result[size];
			for (int i = 0; i < size; i++) {
				result[i] = res[i + (res.length()-size)];
			}
			cout << "result " << string(result) << endl;
			MPI_Send(result, 1, bigInteger, 0, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Type_free(&bigInteger);
	MPI_Finalize();
}
