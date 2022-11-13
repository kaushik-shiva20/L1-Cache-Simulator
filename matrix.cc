#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <fstream>

using namespace std;

void multiply(unsigned n);
void block_multiply(unsigned n, unsigned block_size);

void usage(){
	cout << "Usage: ";	
	cout << "multiply|block_multiply <n> <block_size>" << endl;
	cout << "<block_size> required only by block_multiply" << endl;
}

/********************************
 *  *      MAIN                    *
 *   ********************************/
int main(int argc, char** argv){
        if (argc < 3){
                usage(); 
		return -1;
        }
        int n = atoi(argv[2]);
       	if (n<=0){
                usage();
		return -1; 
	}
	if (!strcmp(argv[1], "multiply")){
                multiply(n);
        }
        else if (!strcmp(argv[1], "block_multiply")){
                if (argc < 4){
                	usage(); 
			return -1;
                }
                int block_size = atoi(argv[3]);
		if (block_size <=0 || block_size > n){
			usage(); 
			return -1;
		}
                block_multiply(n,block_size);
        }else{
                usage(); 
		return -1;
        }
        return 0;
}


/* This function generates the memory accesses trace for a naive matrix multiplication code (see pseudocode below).
The code computes c=a*b, where a, b and c are square matrices containing n*n single-precision floating-point values.
Assume that the matrices are stored in row-major order, as follows:

a[0,0] a[0,1] ... a[0,n-1] a[1,0] a[1,1] ... a[1,n-1] ... a[n-1,0] a[n-1,1] ... a[n-1,n-1]

Assume that the three matrices are stored contiguously in memory starting at address 0x0.
The trace should contain all memory accesses to matrices a, b and c. You can ignore variables i, j, k and parameter n.
You can ignore the initialization of matrix c (i.e., assume it is already initialized to all 0s.

   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
	 for (int k = 0; k < n; k++) {
            c[i,j] = a[i,k] * b[k,j] + c[i,j];
         }
      }
   }

The trace should be written to standard output, and it should follow the format required by your cache simulator.
*/
void multiply(unsigned n){
    int a[n][n];
    int b[n][n];
    int c[n][n];

    ofstream matrix_output;
    matrix_output.open("testcases/traces/matrix_out.t");

    if(!matrix_output.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }
    //matrix_output << "START" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                c[i][j] = a[i][k] * b[k][j] + c[i][j];

                matrix_output << "r " << "0x" << hex << (0x0 + i*n*4 + k*4) << dec << endl;  // read a[i][k]
                matrix_output << "r " << "0x" << hex << (0x0 + 4*n*n + k*n*4 + j*4) << dec << endl;  // read b[k][j]
                matrix_output << "r " << "0x" << hex << (0x0 + 4*n*n + 4*n*n + i*n*4 + j*4) << dec << endl;  // read c[i][j]
                matrix_output << "w " << "0x" << hex << (0x0 + 4*n*n + 4*n*n + i*n*4 + j*4) << dec << endl;  // write c[i][j]
            }
        }
    }
    //matrix_output << "STOP" << endl;
    matrix_output.close();
}

/* This functi n generates the memory accesses trace for a matrix multiplication code that uses blocking (see pseudocode below).
The code computes c=a*b, where a, b and c are square matrices containing n*n single-precision floating-point values.
Assume that the matrices are stored in row-major order, as follows:

a[0,0] a[0,1] ... a[0,n-1] a[1,0] a[1,1] ... a[1,n-1] ... a[n-1,0] a[n-1,1] ... a[n-1,n-1]

Assume that the three matrices are stored contiguously in memory starting at address 0x0.
The trace should contain all memory accesses to matrices a, b and c. You can ignore variables ii, jj, kk, i, j, k and parameters n and block_size.
You can ignore the initialization of matrix c (i.e., assume it is already initialized to all 0s.

for (ii = 0; ii < n; ii+=block_size) {
  for (jj = 0; jj < n; jj+=block_size) {
    for (kk = 0; kk < n; kk+=block_size) {
      for (i = ii; i < ii+block_size; i++) {
        for (j = jj; j < jj+block_size; j++) {
          for (k = kk; k < kk+block_size; k++) {
            c[i,j] = a[i,k] * b[k,j] + c[i,j];

          }
        }
      }
    }
  }
}

The trace should be written to standard output, and it should follow the format required by your cache simulator.
*/
void block_multiply(unsigned n, unsigned block_size){
    int a[n][n];
    int b[n][n];
    int c[n][n];

    ofstream block_matrix_output;
    block_matrix_output.open("testcases/traces/block_matrix_out.t");

    if(!block_matrix_output.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }

    for (int ii = 0; ii < n; ii+=block_size) {
        for (int jj = 0; jj < n; jj+=block_size) {
            for (int kk = 0; kk < n; kk+=block_size) {
                for (int i = ii; i < ii+block_size; i++) {
                    for (int j = jj; j < jj+block_size; j++) {
                        for (int k = kk; k < kk+block_size; k++) {
                            c[i][j] = a[i][k] * b[k][j] + c[i][j];
                            block_matrix_output << "r " << "0x" << hex << (0x0 + i*n*4 + k*4) << dec << endl;  // read a[i][k]
                            block_matrix_output << "r " << "0x" << hex << (0x0 + 4*n*n + k*n*4 + j*4) << dec << endl;  // read b[k][j]
                            block_matrix_output << "r " << "0x" << hex << (0x0 + 4*n*n + 4*n*n + i*n*4 + j*4) << dec << endl;  // read c[i][j]
                            block_matrix_output << "w " << "0x" << hex << (0x0 + 4*n*n + 4*n*n + i*n*4 + j*4) << dec << endl;  // write c[i][j]
                        }
                    }
                }
            }
        }
    }
}

