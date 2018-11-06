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

// Define program exit mnemonics
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// Type definitions
typedef volatile signed int vint32;
typedef volatile char vint8;

// Matrix control register wrapper
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

// Register wrapper
struct apbmatrix_regs {
    union matrix_control_register matrix_control;
    vint32 row[4];
    vint32 column[4];
    vint32 result;
};

// These are the same matrices A and B transposed, only 4 chars have concatenated to 1 int
// Reason being it a manual method to be certain 32 bits read at once, instead of 4 8 bit reads
const int A[13][4] = {
        {1376059677, 1131639157, 1229679451, 39},
        {995252820,  1215981181, 1258437411, 22},
        {561851501,  1132297518, 2081229584, 78},
        {53485138,   402670851,  2136763515, 16},
        {1293569397, 1061636638, 2118257732, 84},
        {427578919,  152724835,  1700879112, 104},
        {1601782535, 240938324,  2004379992, 84},
        {492704346,  475157630,  1818564455, 122},
        {1797720888, 2020096783, 644240234,  103},
        {1281642766, 84214570,   1293751839, 15},
        {1501313910, 1245315925, 860762390,  28},
        {1079475062, 341516381,  2000774741, 16},
        {1913147681, 1916751173, 574497322,  103}
};

const int B[13][4] = {
        {1649244519, 1449729343, 2013934653, 101},
        {1363546142, 775833430,  1365644652, 103},
        {1095317033, 276325380,  1158679934, 39},
        {86591813,   1562855440, 1245388654, 106},
        {1365000802, 443685158,  1081424392, 115},
        {104741389,  1680214837, 922749525,  15},
        {708734792,  1063874670, 1174486083, 88},
        {1935933719, 1949828107, 1028392484, 75},
        {2054385234, 1568503881, 726928665,  24},
        {240608328,  1074486833, 709382708,  118},
        {1007844972, 1813933931, 286086187,  41},
        {2038248032, 739315719,  1516196122, 75},
        {1885032217, 354746994,  997742670,  104}
};

// Result matrix
int C[13][13] = {};

const char M1[13][13] = {
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

	const char M2[13][13] = {
	{103,30,41,69,98,13,72,23,82,72,108,96,25},
	{121,16,54,73,66,58,111,1,110,100,126,50,79},
	{77,70,73,41,92,62,62,100,115,87,18,125,91},
	{98,81,65,5,81,6,42,115,122,14,60,121,112,},
	{63,86,4,16,38,53,110,11,73,49,107,7,114},
	{29,71,100,72,25,11,112,4,120,94,111,16,2},
	{105,62,120,39,114,38,105,56,125,11,30,17,37},
	{86,46,16,93,26,100,63,116,93,64,108,44,21},
	{61,108,126,110,8,85,67,36,25,52,43,26,78},
	{52,21,13,31,58,10,60,6,13,82,84,81,88},
	{10,102,16,59,117,0,1,76,84,72,13,95,120},
	{120,81,69,74,64,55,70,61,43,42,17,90,59},
	{101,103,39,106,115,15,88,75,24,118,41,75,104} 
	};

	vint32 M3[13][13];

// Register defined as global
struct apbmatrix_regs *reg = (struct apbmatrix_regs *) APBMATRIX_ADDRESS;

/**
 * Print the contents of a 13x13 matrix with signed 32 bit elements.
 * @param matrix 13x13 32 bit signed integers
 */
void printMatrix(vint32 matrix[13][13]) {
    char i, j;
    printf("Matrix = \n");
    for (i = 0; i < 13; i++) {
        if (i == 0) {
            printf("\t[");
        }

        for (j = 0; j < 13; j++) {

            printf("%d", matrix[i][j]);
        }

        if (i == 12) {
            printf("],");
        } else {
            printf(",\t");
        }
    }
}

/**
 * This function is used to interface with the hardware module apbmatrix which implemnets a hardware structure
 * to efficiently multiply vectors. The hardware module itself is connected to the on-chip apb bus, which is basically
 * a secondary bus with lower bandwidth. The result is stored in dedicated registers in apbmatrix and can hold an enitre
 * 13x13 matrix with 32 bit signed elements.
 * @param A 13x4 matrix (actually 13x13 8 bit elements)
 * @param B 13x4 matrix (actually 13x13 8 bit elements)
 */
void matrixMultiply(const int A[13][4], const int B[13][4], int C[13][13]) {
    printf("\n Begin! \n");

    // multiply all vectors
    char i, j, k = 0;
    for (i = 0; i < 13; i++) {
        // state read data
        reg->matrix_control.B.read_data = 1;

        // Load row elements
        reg->row[0] = A[i][0]; // row elements 0 - 3
        reg->row[1] = A[i][1]; // row elements 4 - 7
        reg->row[2] = A[i][2]; // row elements 8 - 11
        reg->row[3] = A[i][3]; // row elements 12 - 15

        for (j = 0; j < 13; j++) {
            // state read data
            reg->matrix_control.B.read_data = 1;

            // Load row elements
            reg->column[0] = B[j][0]; // column elements 0 - 3
            reg->column[1] = B[j][1]; // column elements 4 - 7
            reg->column[2] = B[j][2]; // column elements 8 - 11
            reg->column[3] = B[j][3]; // column elements 12 - 15

            // state is calculate
            reg->matrix_control.A = (1 << 30) | k;

            while (!reg->matrix_control.B.ready); // pause until state ready

            // fetch result
            C[i][j] = reg->result;

            // calc is done, so reset
            reg->matrix_control.B.ready = 0;

            // increase index
            k++;
        }
    }

    printf("\n End! \n");
}

/**
 * Program starts executing here
 * @return int (either 0 or 1 showing respectively success or failure)
 */
int main() {
    reg->matrix_control.B.ready = 1;
	
	int i, j, k;
	for (i=0;i<1;i++){
		for (j=0;j<1;j++){
			for(k=0;k<13;k++){
				M3[i][j] += M1[i][k]*M2[k][j];
			}
		}
	}
	
	reg->matrix_control.B.ready = 0;	

    //matrixMultiply(A, B, C);
    printMatrix(M3);
    //int i, j;
    /*for (i = 0; i < 13; i++) {
        for (j = 0; j < 13; j++) {
            printf("%d\n", C[i][j]);
        }
    }*/

    report_start();

    base_test();

    report_end();

    return EXIT_SUCCESS;
}
