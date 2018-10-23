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
    vint32 results[13][13];
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
void matrixMultiply(const int A[13][4], const int B[13][4]) {
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

            // calc is done, so reset
            reg->matrix_control.B.calc = 0;

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
    matrixMultiply(A, B);
    //printMatrix(reg->results);

    // Spits 64578 and < so not quite working as expected just yet
    printf("%d\n", reg->results[0][0]);
    printf("%d\n", reg->results[12][12]);

    report_start();

    base_test();

    report_end();

    return EXIT_SUCCESS;
}
