
#include <cstdlib>
#include <iostream>
#include <cilk/cilk.h>
#include "hwtimer.h"
using namespace std;

//GLOBAL CONSTANTS

int const MAX_NUM = 10;
int const MIN_NUM = 0;


//TAKEN FROM STACKOVERFLOW TO CALCULATE THE ZORDER NUMBER FROM COORDINATES
//ONLY USED FOR PRINTING
uint32_t calcZOrder(uint16_t xPos, uint16_t yPos)
{
    static const uint32_t MASKS[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const uint32_t SHIFTS[] = {1, 2, 4, 8};

    uint32_t x = xPos;  // Interleave lower 16 bits of x and y, so the bits of x
    uint32_t y = yPos;  // are in the even positions and bits from y in the odd;

    x = (x | (x << SHIFTS[3])) & MASKS[3];
    x = (x | (x << SHIFTS[2])) & MASKS[2];
    x = (x | (x << SHIFTS[1])) & MASKS[1];
    x = (x | (x << SHIFTS[0])) & MASKS[0];

    y = (y | (y << SHIFTS[3])) & MASKS[3];
    y = (y | (y << SHIFTS[2])) & MASKS[2];
    y = (y | (y << SHIFTS[1])) & MASKS[1];
    y = (y | (y << SHIFTS[0])) & MASKS[0];

    const uint32_t result = x | (y << 1);
    return result;
};
//Recursively randomizes a matrix
void populateMatrixZOrder(int *M, int n){
	if (n==1){
		M[0] = rand()%(MAX_NUM - MIN_NUM + 1) + MIN_NUM;
	}else{
		populateMatrixZOrder(&M[0],n/2);//fills(1,1)
		populateMatrixZOrder(&M[n*n/4],n/2);//fills(1,2)
		populateMatrixZOrder(&M[n*n/2],n/2);//fills(2,1)
		populateMatrixZOrder(&M[n*n/2 + n*n/4],n/2);//fills(2,2)
	}
};

int* initializeMatrix(int n){	
	int* matrix = new int[n*n];	
	cout << "Matrix Initialized" << endl;
	return matrix;
};

void deleteMatrix(int *M, int n){
	delete [] M;		
	cout << "Matrix deleted" << endl;
	return;	
};

void Mult(int *C, int *A, int *B, int n){
	
	if (n ==1){
		C[0] = C[0] + A[0] * B[0];
	}else{
		//Partition each matrix into 4 quadrants (1,1),(1,2),(2,1),(2,2)
		//Using a pointer to the start of each quadrant recursively multiply once down to unit matrices
		Mult(&C[0],&A[0],&B[0], n/2);
		Mult(&C[n*n/4],&A[0],&B[n*n/4],n/2);
		Mult(&C[n*n/2 + n*n/4],&A[n*n/2],&B[n*n/4],n/2);
		Mult(&C[n*n/2],&A[n*n/2],&B[0],n/2);
		
		Mult(&C[n*n/2],&A[n*n/2 + n*n/4],&B[n*n/2], n/2);
		Mult(&C[n*n/2 + n*n/4],&A[n*n/2 + n*n/4],&B[n*n/2 + n*n/4],n/2);
		Mult(&C[n*n/4],&A[n*n/4],&B[n*n/2 + n*n/4],n/2);
		Mult(&C[0],&A[n*n/4],&B[n*n/2],n/2);
		
	}	
};

void printZOrder(int *M, int n){	
	for (int i =0; i < n*n; ++i){
		cout<< " [" << M[i] << "] ";
	}
	cout << "\n";
	return;
};
//prints out matrix in zorder into rowwise
void printRowWise(int *M, int n){	
	for(int i = 0;i < n ; ++i){
		for(int j = 0;j < n; ++j){
		  cout << " [" << M[calcZOrder(j,i)] << "] ";
		}
		cout << "\n";
	}
};

int main(int argc, char* argv[]){		
		
	srand ( time(NULL) ); //sets random seed
	int n;
	
	//get size of n
	cout << "Enter size of n*n Matrices" << endl;
	cin >> n;
	
	//start timer
	hwtimer_t timer;
	initTimer(&timer);	
	startTimer(&timer);	
	//allocate space for 3 matrices	
	int* matrixA = initializeMatrix(n);
	int* matrixB = initializeMatrix(n);	
	int* matrixC = initializeMatrix(n);
	
	//Randomize the 2 matrices A and B
	populateMatrixZOrder(matrixA,n);
	populateMatrixZOrder(matrixB,n);
	
	//Multiply them together to fill in matrix C
	Mult(matrixC, matrixA, matrixB,n);
	
	stopTimer(&timer);//stops timer
	
	//Currently set to only print out human readable Rowwise Matrices
	//printZOrder(matrixA,n);
	cout << "\n";
	printRowWise(matrixA,n);
	
	//printZOrder(matrixB,n);
	cout << "\n";
	printRowWise(matrixB,n);

	//printZOrder(matrixC,n);
	cout << "\n";
	printRowWise(matrixC,n);
	
	deleteMatrix(matrixA,n);
	deleteMatrix(matrixB,n);
	deleteMatrix(matrixC,n);//Memory clean up
	
	long long multTime = getTimerNs(&timer);
	
	cout << "Total time: " << multTime << "ns" << endl;//timer output
	
	return 0;
};
