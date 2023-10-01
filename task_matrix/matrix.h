#ifndef MATRIX_PARALLEL_MATRIX_PARALLEL
#define MATRIX_PARALLEL_MATRIX_PARALLEL

#ifndef MATRIX_SIZE
    #define MATRIX_SIZE 512
#endif

void matrix_init(double matrix[MATRIX_SIZE][MATRIX_SIZE]);

void matrix_multiply_seq(double A[MATRIX_SIZE][MATRIX_SIZE],
                         double B[MATRIX_SIZE][MATRIX_SIZE],
                         double C[MATRIX_SIZE][MATRIX_SIZE]);

#endif
