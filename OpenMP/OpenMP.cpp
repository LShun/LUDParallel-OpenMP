#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>  

#include "../Matrix_Formatting/Matrix_Formatting.cpp"
#include "OpenMP.h"

#define VERBOSE true // show the actual matrix output & full validation matrices, disable when just benchmarking
#define VALIDATION true // validate results, disable for improved performance
#define DEBUG true // show debug information, disable for even shorter output to copy paste to excel sheets

using namespace std;

void print_title() {
	SetColor(3);
	cout << "========================================================================================================" << endl;
	cout << "=   ==           ==      ==         =======      =======     =====    ====     ===      ===  =====     =" << endl;
	cout << "=   ==           ==      ==         ==      =    ==        ==       =      =   =  =    =  =  ==    =   =" << endl;
	cout << "=   ==           ==      ==         ==       =   ==       ==       =        =  =   =  =   =  ==     =  =" << endl;
	cout << "=   ==           ==      ==         ==       =   =======  ==       =        =  =    ==    =  ==    =   =" << endl;
	cout << "=   ==           ==      ==         ==       =   ==       ==       =        =  =          =  =====     =" << endl;
	cout << "=   ==           ==      ==         ==       =   ==       ==       =        =  =          =  ==        =" << endl;
	cout << "=   ==           ==      ==         ==      =    ==        ==       =      =   =          =  ==        =" << endl;
	cout << "=   ==========    ========          =======      =======     =====    ====     =          =  ==        =" << endl;
	cout << "========================================================================================================" << endl;
	cout << "                OpenMP - Solution A (OMP-ILP) - Instruction-level (for-loop) Parallelism                " << endl;
	cout << "========================================================================================================" << endl;
	SetColor(7);
}

// perform LU decomposition
// a is the matrix that will be split up into l and u
// array size for all is size x size
void l_u_d(float** a, float** l, float** u, int size)
{
	//for each column...
	//make the for loops of lu decomposition parallel. Parallel region
#pragma omp parallel shared(a,l,u)
	{
		//// get threads
		//cout << "Threads: " << omp_get_num_threads() << endl;
		for (int i = 0; i < size; i++)
		{
			//for each row....
			//rows are split into seperate threads for processing
#pragma omp for
			for (int j = 0; j < size; j++)
			{
				//if j is smaller than i, set l[j][i] to 0
				if (j < i)
				{
					l[j][i] = 0;
					continue;
				}
				//otherwise, do some math to get the right value
				l[j][i] = a[j][i];
				for (int k = 0; k < i; k++)
				{
					//deduct from the current l cell the value of these 2 values multiplied
					l[j][i] = l[j][i] - l[j][k] * u[k][i];
				}
			}
			//for each row...
			//rows are split into seperate threads for processing
#pragma omp for
			for (int j = 0; j < size; j++)
			{
				//if j is smaller than i, set u's current index to 0
				if (j < i)
				{
					u[i][j] = 0;
					continue;
				}
				//if they're equal, set u's current index to 1
				if (j == i)
				{
					u[i][j] = 1;
					continue;
				}
				//otherwise, do some math to get the right value
				u[i][j] = a[i][j] / l[i][i];
				for (int k = 0; k < i; k++)
				{
					u[i][j] = u[i][j] - ((l[i][k] * u[k][j]) / l[i][i]);
				}
			}
		}
	}
}

//initialize the matrices
void initialize_matrices(float** a, float** l, float** u, int size)
{
	//for each row in the 2d array, initialize the values
	//values are processed by seperate threads
#pragma omp for schedule(static)
	for (int i = 0; i < size; ++i)
	{
		a[i] = new float[size];
		l[i] = new float[size];
		u[i] = new float[size];
	}
}

//fill the array with random values (done for a)
void random_fill(float** matrix, int size)
{
	//fill a with random values
	if (VERBOSE) {
		printHeader("Producing random values");
	}

	//fill a with random values
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			matrix[i][j] = (float) ((rand() % 10) + 1);
		}
	}

	//Ensure the matrix is diagonal dominant to guarantee invertible-ness
	//diagCount well help keep track of which column the diagonal is in
	int diagCount = 0;
	float sum = 0;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			//Sum all column vaalues
			sum += abs(matrix[i][j]);
		}
		//Remove the diagonal  value from the sum
		sum -= abs(matrix[i][diagCount]);
		//Add a random value to the sum and place in diagonal position
		matrix[i][diagCount] = sum + ((rand() % 5) + 1);
		++diagCount;
		sum = 0;
	}
}

void matrix_validation(float** a, float** l, float** u, int size)
{
	float** check = new float* [size];
	//for sending whole matrices to each thread
	float** a2 = new float* [size];
	float** l2 = new float* [size];
	float** u2 = new float* [size];
	initialize_matrices(a2, l2, u2, size);

	for (int i = 0; i < size; ++i)
	{
		check[i] = new float[size];
	}

	a2 = a;
	l2 = l;
	u2 = u;

	//matrix multiplication
	printHeader("Matrix Multiplication for Validation:  ");
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			check[i][j] = 0;
			for (int k = 0; k < size; k++)
			{
				check[i][j] += u2[k][j] * l2[i][k];
			}

			check[i][j] = round(check[i][j]);
		}
	}
	
	if (VERBOSE) {
		printHeader("Check Matrix: ");
		print_matrix_2D(check, size);
	}
	
	int error = 0;
	cout << "Validation Results:  " << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (check[i][j] != a[i][j])
			{
				error = 1;
				cout << "Error index is : " << check[i][j] << endl;
			}

		}
	}

	if (error == 1)
	{
		cout << "Failed" << endl;
	}
	else
		cout << "Success" << endl;
}

int main(int argc, char** argv)
{
	double runtime;
	int size;
	/* int threads[] = { 2,4,8,16,32,64,128,256,512 }; */ // uncomment this if want batch, and uncomment the user input section

	print_title();

	// print info
	printHeader("Parameters");
	cout << "Verbose Output: ";
	printYesNo(VERBOSE);
	cout << "Validation: ";
	printYesNo(VALIDATION);
	cout << "Debug Output: ";
	printYesNo(DEBUG);
	cout << endl;
	fflush(stdout);

	//seed rng
	srand(1);

	// user input section start
	int thrds;
	get_matrix_size(size);
	get_thread_count(thrds);
	int threads[1] = {thrds};
	// user input section end
	
	cout << "Your input matrix size is: ";
	SetColor(6);
	cout << size << " x " << size << endl;
	SetColor(7);
	fflush(stdout);

	for (int i = 0; i < sizeof(threads) / sizeof(threads[0]); i++) {
		omp_set_num_threads(threads[i]);
		if (DEBUG) {
			cout << "Number of threads: ";
			SetColor(6);
			cout << threads[i] << endl;
			SetColor(7);
			fflush(stdout);
		}

		//initalize matrices
		float** a = new float* [size];
		float** l = new float* [size];
		float** u = new float* [size];
		initialize_matrices(a, l, u, size);

		random_fill(a, size);

		if (VERBOSE) {
			//print A
			printHeader("A Matrix: ");
			print_matrix_2D(a, size);
		}

		//do LU decomposition
		runtime = omp_get_wtime();
		l_u_d(a, l, u, size);
		//get the runtime of the job
		runtime = omp_get_wtime() - runtime;
		if (VERBOSE) {
			//print l and u
			printHeader("L Matrix: ");
			print_matrix_2D(l, size);
			printHeader("U Matrix: ");
			print_matrix_2D(u, size);
		}
		if (VALIDATION) {
			matrix_validation(a, l, u, size);
		}

		cout << endl << "LUD Decomposition Time: ";
		SetColor(6);
		cout << runtime;
		SetColor(7);
		cout << " seconds" << endl;
		fflush(stdout);
	}
	return 0;
}




