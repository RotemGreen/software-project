#ifndef MATRIX_MANAGEMENT_H
#define MATRIX_MANAGEMENT_H

double **create_zero_mat(int rows, int cols);
void free_mat(double **mat, int rows);
void free_allocated_matrixes(void);

#endif
