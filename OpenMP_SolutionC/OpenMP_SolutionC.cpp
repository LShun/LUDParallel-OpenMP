#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>  
#include "../Matrix_Formatting/Matrix_Formatting.cpp"

#define VERBOSE true
#define VALIDATION true
#define DEBUG true

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
	cout << "                OpenMP - Solution C (OMP-SIMD) - ILP + DS + Data-level Parallelism (SIMD)               " << endl;
	cout << "========================================================================================================" << endl;
	SetColor(7);
}

//do LU decomposition
//a is the matrix that will be split up into l and u
//array size for all is size x size
void l_u_d(float** a, float** l, float** u, int size, int chunkSize)
{
	//////initialize a simple lock for parallel region
	//omp_lock_t lock;

	//omp_init_lock(&lock);
	//for each column...
	//make the for loops of lu decomposition parallel. Parallel region
#pragma omp parallel shared(a,l,u)
	{
		for (int i = 0; i < size; i++)
		{
			//for each row....
			//rows are split into seperate threads for processing
#pragma omp for schedule(dynamic, chunkSize)
			for (int j = 0; j < size; j++)
			{
				//if j is smaller than i, set l[j][i] to 0
				if (j < i)
				{
					l[j][i] = 0;
					continue;
				}
				//otherwise, do some math to get the right value
				float tmp_reduction = a[j][i];
#pragma omp simd reduction(+:tmp_reduction)
				for (int k = 0; k < i; k++)
				{
					//deduct from the current l cell the value of these 2 values multiplied
					tmp_reduction = tmp_reduction - l[j][k] * u[k][i];
				}
				l[j][i] = tmp_reduction;
			}
			//for each row...
			//rows are split into seperate threads for processing
#pragma omp for schedule(dynamic, chunkSize)
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
				float tmp_reduction = a[i][j] / l[i][i];
#pragma omp simd reduction(+:tmp_reduction)
				for (int k = 0; k < i; k++)
				{
					tmp_reduction = tmp_reduction - ((l[i][k] * u[k][j]) / l[i][i]);
				}
				u[i][j] = tmp_reduction;
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
	int size, chunkSize;

	//// batch section start - set how many threads you want to use (uncomment below if you want batch, and then comment out the user input section)
	//// set how many threads you want to use
	//int threads[][2] = { {2,64}, {2,128}, {16,256}, {32,512}, {64,1024}, {128,2048} };
	//int chunkSize = 16;
	//// batch section end

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

	// user input section start
	int thrds;
	get_matrix_size(size);
	get_thread_count(thrds);
	get_chunk_size(chunkSize);
	int threads[][2] = { {thrds, size} };
	// user input section end

	printHeader("Execution Information");

	cout << "Chunk size is : " << chunkSize << endl;
	SetColor(10);
	cout << "SIMD enabled." << endl << endl;
	SetColor(7);

	for (int i = 0; i < sizeof(threads) / sizeof(threads[0]); i++) {
		int threadCount = threads[i][0];
		if (DEBUG) {
			cout << "Number of threads: ";
			SetColor(6);
			cout << threadCount << endl;
			SetColor(7);
			fflush(stdout);
		}

		omp_set_num_threads(threadCount);

		//seed rng
		srand(1);

		//size of matrix
		int size = threads[i][1];
		if (DEBUG) {
			cout << "Your input matrix size is: ";
			SetColor(6);
			cout << size << " x " << size << endl;
			SetColor(7);
			fflush(stdout);
		}

		//initalize matrices
		float** a = new float* [size];
		float** l = new float* [size];
		float** u = new float* [size];
		initialize_matrices(a, l, u, size);

		//fill a with random values
		random_fill(a, size);

		//print A
		if (VERBOSE) {
			printHeader("A Matrix: ");
			print_matrix_2D(a, size);
		}

		//do LU decomposition
		runtime = omp_get_wtime();
		l_u_d(a, l, u, size, chunkSize);
		runtime = omp_get_wtime() - runtime;

		if (VERBOSE) {
			//print l and u
			printHeader("L Matrix: ");
			print_matrix_2D(l, size);
			printHeader("U Matrix: ");
			print_matrix_2D(u, size);
		}

		//get the runtime of the job
		if (VALIDATION) {
			matrix_validation(a, l, u, size);
		}

		cout << "LUD Decomposition Time: ";
		SetColor(6);
		cout << runtime;
		SetColor(7);
		cout << " seconds" << endl;;
	}
	return 0;
}




