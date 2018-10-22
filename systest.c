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

typedef volatile unsigned int vuint32;
typedef volatile char vuint8;

// 13x13 matrix A
const char A[13][13] = {
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

// Structure to hold the matrix registers
union matrix_control_register {
   struct {
	vuint32 read_data :1;
	vuint32 calc :1;
	vuint32 ready :1;
	vuint32 RESERVED :21;
	vuint32 element_index :8;
   } B;
   vuint32 A;
};

struct apbmatrix_regs {
   union matrix_control_register matrix_control;
   vuint32 row[4];
   vuint32 column[4];
   vuint32 results[172];
};


int apbmatrix_test() {
   struct apbmatrix_regs *reg = (struct apbmatrix_regs*) APBMATRIX_ADDRESS;

   // state read data
   reg->matrix_control.B.read_data = 1;

   // load operands
   reg->row[1] = 1; // dummy value
   reg->row[2] = 1; // dummy value
   reg->row[3] = 1; // dummy value
   reg->row[4] = 1; // dummy value

   reg->column[1] = 1; // dummy value
   reg->column[2] = 1; // dummy value
   reg->column[3] = 1; // dummy value
   reg->column[4] = 1; // dummy value

   // state is calculate
   reg->matrix_control.A = (1 << 30) | 0;

   printf("\n Begin! \n");
   while (!reg->matrix_control.B.ready)
	; // pause until state ready
   
   // print result
   printf("\n%d\n", reg->results[0]);
   printf("\n End! \n");
}

main()

{
	
	printf("\n\n  Testing matrix multiplication!!!\n");
	apbmatrix_test();
	printf("\n\n  Done testing matrix multiplication!!!\n");

	report_start();

	base_test();

	report_end();
}
