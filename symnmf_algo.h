#ifndef SYMNMF_ALGO_H
#define SYMNMF_ALGO_H

#include "symnmf.h"
#include "matrix_management.h"

double **calc_similarity_mat(double **points, MatrixDimensions dim, int d);
double **calc_diagonal_mat(double **similarity_mat, MatrixDimensions dim);
double **calc_normalized_matrix(double **D, double **A, MatrixDimensions mat_dim);
double **optimize_symnmf(double **h, double **w, int n, int k, int max_iter, double epsilon);

#endif
