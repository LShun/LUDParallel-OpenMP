#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <Windows.h>
using namespace std;

#define MAX_THREADS 512 // define the maximum thread that the user can use for the program

void print_matrix_2D(float**, int);

void print_matrix_footer(int size);

void print_matrix_header(int size);

//print the matrix out
void print_matrix_2D(float** matrix, int size)
{
	// print the header for the matrix
	//for each row...
	print_matrix_header(size);
	for (int i = 0; i < size; i++)
	{
		cout << left << setw(9) << setprecision(3) << "|" << left << setw(9);
		//for each column
		for (int j = 0; j < size; j++)
		{
			//print out the cell
			cout << left << setw(9) << setprecision(3) << matrix[i][j] << left << setw(9);
		}
		//new line when ever row is done
		cout << left << setw(9) << setprecision(3) << "|" << left << setw(9);
		cout << endl;
	}
	// print the footer the matrix
	print_matrix_footer(size);
}

void print_matrix_footer(int size)
{
	cout << left << setw(9) << setprecision(3) << "|__" << left << setw(9);
	for (int i = 1; i < size; i++)
	{
		cout << left << setw(9) << setprecision(3) << "" << left << setw(9);
	}
	cout << left << setw(9) << setprecision(3) << "       __|" << left << setw(9) << endl;
}

void print_matrix_header(int size)
{
	cout << left << setw(9) << setprecision(3) << " __" << left << setw(9);
	for (int i = 1; i < size; i++)
	{
		cout << left << setw(9) << setprecision(3) << "" << left << setw(9);
	}
	cout << left << setw(9) << setprecision(3) << "       __" << left << setw(9) << endl;
	cout << left << setw(9) << setprecision(3) << "|" << left << setw(9);
	for (int i = 0; i < size; i++)
	{
		cout << left << setw(9) << setprecision(3) << "" << left << setw(9);
	}
	cout << left << setw(9) << setprecision(3) << "|" << left << setw(9) << endl;
}

void printHeader(string text) {
	string matrixTitleDecoration = "";
	char matrixDecorationSymbol = '*';
	int textLength = text.length();
	matrixTitleDecoration.append(text.length(), matrixDecorationSymbol);
	cout << endl << left << matrixTitleDecoration << endl;
	cout << left << text << endl;
	cout << left << matrixTitleDecoration << endl;
}

void SetColor(int value) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), value);
}


void get_matrix_size(int& size)
{
	int N, temp;
	bool error = true;
	cout << "Please enter the size of your square matrix: " << endl;
	fflush(stdout);
	N = scanf_s("%d", &size);

	while ((N != 1 && error == true) || size < 2 || size % 2 != 0) {
		while ((temp = getchar()) != EOF && temp != '\n');
		SetColor(4);
		cout << "Invalid input... Please enter a number larger or equal to 2 and is multiplication of 2: " << endl;
		SetColor(7);
		fflush(stdout);
		N = scanf_s("%d", &size);
		if (size > 0) {
			error = false;
		}
	}
}

// gets thread count, a limit of MAX_THREADS is set to prevent potential buffer overflow
void get_thread_count(int& size)
{
	int N, temp;
	bool error = true;
	cout << "Please enter the number of threads you want to use: " << endl;
	fflush(stdout);
	N = scanf_s("%d", &size);

	while ((N != 1 && error == true) || size < 2 || size > MAX_THREADS) {
		while ((temp = getchar()) != EOF && temp != '\n');
		SetColor(4);
		cout << "Invalid. Please enter a number larger or equal to 2, less than or equal to " << MAX_THREADS << ": " << endl;
		SetColor(7);
		fflush(stdout);
		N = scanf_s("%d", &size);
		if (size > 0) {
			error = false;
		}
	}
}

// gets chunk size, a limit of MAX_THREADS is imposed to prevent potential buffer overflow.
void get_chunk_size(int& size)
{
	int N, temp;
	bool error = true;
	cout << "Please enter the chunk size you want to use: " << endl;
	fflush(stdout);
	N = scanf_s("%d", &size);

	while ((N != 1 && error == true) || size < 1) {
		while ((temp = getchar()) != EOF && temp != '\n');
		SetColor(4);
		cout << "Invalid. Please enter a number equal or larger than 1: " << endl;
		SetColor(7);
		fflush(stdout);
		N = scanf_s("%d", &size);
		if (size > 0) {
			error = false;
		}
	}
}

void printYesNo(bool value)
{
	if (value == true) {
		SetColor(10);
		cout << "Yes" << endl;
		SetColor(7);
	}
	else {
		SetColor(4);
		cout << "No" << endl;
		SetColor(7);
	}
}