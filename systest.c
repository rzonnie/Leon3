/*
 * Test for APBMatrixMultiplication
 *
 * Author: Remi Jonkman
 * Date: October 22nd 2018
 *
 * This application tests the matrix multiplication of adbmatrix.vhd.
 * It is required adbmatrix is connected to the leon3 core.
 *
 */

// Define address of matrix multiplication peripheral
#define APBMATRIX_ADDRESS 0x80000500

typedef volatile signed int vint32;
typedef volatile char vint8;

// 13x13 matrix A
/*const char A[13][13] = {
    {29,1,5,82,117,113,115,67,91,107,75,73,39},
    {84,90,82,59,125,102,122,72,35,59,2,75,22},
    {109,44,125,33,46,125,125,67,16,11,13,124,78},
    {82,30,48,3,3,69,0,24,123,108,92,127,16},
    {117,77,26,77,30,74,71,63,68,12,66,126,84},
    {39,86,124,25,99,101,26,9,8,91,97,101,104},
    {7,67,121,95,84,109,92,14,88,105,120,119,84},
    {90,18,94,29,126,84,82,28,103,23,101,108,122},
    {56,11,39,107,15,59,104,120,106,83,102,38,103},
    {14,81,100,76,42,3,5,5,31,22,29,77,15},
    {118,59,124,89,85,3,58,74,22,49,78,51,28},
    {118,123,87,64,93,32,91,20,85,102,65,119,16},
    {33,81,8,114,69,77,63,114,42,34,62,34,103}
};

// 13x13 matrix B
const char B[13][13] = {
    {103,121,77,98,63,29,105,86,61,52,10,120,101},
    {30,16,70,81,86,71,62,46,108,21,102,81,103},
    {41,54,73,65,4,100,120,16,126,13,16,69,39},
    {69,73,41,5,16,72,39,93,110,31,59,74,106},
    {98,66,92,81,38,25,114,26,8,58,117,64,115},
    {13,58,62,6,53,11,38,100,85,10,0,55,15},
    {72,111,62,42,110,112,105,63,67,60,1,70,88},
    {23,1,100,115,11,4,56,116,36,6,76,61,75},
    {82,110,115,122,73,120,125,93,25,13,84,43,24},
    {72,100,87,14,49,94,11,64,52,82,72,42,118},
    {108,126,18,60,107,111,30,108,43,84,13,17,41},
    {96,50,125,121,7,16,17,44,26,81,95,90,75},
    {25,79,91,112,114,2,37,21,78,88,120,59,104},
};
*/

// These are the same matrices A and B transposed, only 4 chars have concatenated to 1 int
const int A[13][4] = {
    {1376059677,1131639157,1229679451,39},
    {995252820,1215981181,1258437411,22},
    {561851501,1132297518,2081229584,78},
    {53485138,402670851,2136763515,16},
    {1293569397,1061636638,2118257732,84},
    {427578919,152724835,1700879112,104},
    {1601782535,240938324,2004379992,84},
    {492704346,475157630,1818564455,122},
    {1797720888,2020096783,644240234,103},
    {1281642766,84214570,1293751839,15},
    {1501313910,1245315925,860762390,28},
    {1079475062,341516381,2000774741,16},
    {1913147681,1916751173,574497322,103}
};

const int B[13][4] = {
    {1649244519,1449729343,2013934653,101},
    {1363546142,775833430,1365644652,103},
    {1095317033,276325380,1158679934,39},
    {86591813,1562855440,1245388654,106},
    {1365000802,443685158,1081424392,115},
    {104741389,1680214837,922749525,15},
    {708734792,1063874670,1174486083,88},
    {1935933719,1949828107,1028392484,75},
    {2054385234,1568503881,726928665,24},
    {240608328,1074486833,709382708,118},
    {1007844972,1813933931,286086187,41},
    {2038248032,739315719,1516196122,75},
    {1885032217,354746994,997742670,104}
};

// Result matrix encapsulated in register (for now)!

// Structure to hold the matrix registers
union matrix_control_register {
   struct {
	vint32 read_data :1;
	vint32 calc :1;
	vint32 ready :1;
	vint32 RESERVED :21;
	vint32 element_index :8;
   } B;
   vint32 A;
};

struct apbmatrix_regs {
   union matrix_control_register matrix_control;
   vint32 row[4];
   vint32 column[4];
   vint32 results[13][13];
};

// Define register
struct apbmatrix_regs *reg = (struct apbmatrix_regs*) APBMATRIX_ADDRESS;

void printMatrix(vint32 matrix[13][13]) {
  vint32* contentPointer = &matrix[0][0];  

  char i;
  printf("Matrix = \n");
  for (i = 0; i < 169; i++) {
    if (i % 13 == 0) {
      printf("\t[");
    }

    printf("%d", *(contentPointer + i));

    if (i % 13 == 13 - 1) {
      printf("],");
    } else {
      printf(",\t");
    }
  }
}

void matrixMultiply(const int A[13][4], const int B[13][4]) {
   printf("\n Begin! \n");

   // state read data
   reg->matrix_control.B.read_data = 1;
   
   // multiply all vectors
   char i, j, k = 0;
   for (i = 0; i < 13; i++) {
      // Load row elements
      reg->row[0] = A[i][0]; // row elements 0 - 3
      reg->row[1] = A[i][1]; // row elements 4 - 7
      reg->row[2] = A[i][2]; // row elements 8 - 11
      reg->row[3] = A[i][3]; // row elements 12 - 15

      for (j = 0; j < 13; j++) {
         // Load row elements
         reg->column[0] = A[j][0]; // column elements 0 - 3
         reg->column[1] = A[j][1]; // column elements 4 - 7
         reg->column[2] = A[j][2]; // column elements 8 - 11
         reg->column[3] = A[j][3]; // column elements 12 - 15

         // state is calculate
         reg->matrix_control.A = (1 << 30) | k;

         while (!reg->matrix_control.B.ready)
	    ; // pause until state ready
         
         // increase index
         k++;
      }
   }
   printf("\n End! \n");
}

main()

{
	matrixMultiply(A, B);
	printMatrix(reg->results);

	report_start();

	base_test();

	report_end();
}
