#include <stdlib.h>
#include <math.h>
#include "symnmf_algo.h"

/************************/
/* Basic matrix helpers */
/************************/

double calc_euclidean_distance(double *xi, double *xj, int d) {
    double val;
    int i;
    val = 0;
    for(i = 0; i < d; i++) {
        val += (xi[i] - xj[i])*(xi[i] - xj[i]);
    }
    return val;
}

/* Calculate the sum of the given matrix's row values */
double get_row_sum(double **mat, MatrixDimensions dim, int row_idx){
    int j;
    double sum;
    sum = 0;
    for(j = 0; j < dim.cols; j++) {
        sum += mat[row_idx][j];
    }
    return sum;
}

/**********************************************/
/* Graph matrix construction and calculations */
/**********************************************/

double calc_similarity_element(double *xi, double *xj, int d) {
    double euclidean_dist;
    euclidean_dist = calc_euclidean_distance(xi, xj, d);
    return exp(-euclidean_dist / 2.0);
}

/* Calculate the similarity matrix A */
double **calc_similarity_mat(double **points, MatrixDimensions dim, int d) {
    int i;
    int j;
    double *xi;
    double *xj;
    double **mat;

    mat = create_zero_mat(dim.rows, dim.rows);
    if(mat == NULL) {
        return NULL;
    }

    /* Assumption: dim.rows = dim.cols */
    for(i = 0; i < dim.rows; i++) {
        xi = points[i];
        for(j = i + 1; j < dim.rows; j++) {
            xj = points[j];
            mat[i][j] = mat[j][i] = calc_similarity_element(xi, xj, d);
        }
    }
    return mat;
}

/* Calculate the diagonal matrix D */
double **calc_diagonal_mat(double **similarity_mat, MatrixDimensions dim) {
    double **mat;
    int i;
    mat = create_zero_mat(dim.rows, dim.cols);
    if(mat == NULL) {
        return NULL;
    }
    for(i = 0; i < dim.rows; i++) {
        mat[i][i] = get_row_sum(similarity_mat, dim, i);
    }
    return mat;
}

/* Calculate the normalized matrix W */
double **calc_normalized_matrix(double **D, double **A, MatrixDimensions mat_dim) {
    double **W;
    double denominator;
    int i;
    int j;

    W = create_zero_mat(mat_dim.rows, mat_dim.cols);
    if(W == NULL) {
        return NULL;
    }

    /* Optimization: since the matrix D is diagonal, 
       the multiplication : D^(-0.5) * A * D^(-0.5) could be calculated by:
       (D^(-0.5))[i][i] * A[i][j] * (D^(-0.5))[j][j] = A[i][j] / (sqrt(D[i][i] * D[j][j])) */
    for(i = 0; i < mat_dim.rows; i++) {
        for(j = 0; j < mat_dim.cols; j++) {
            denominator = sqrt(D[i][i] * D[j][j]);
            if (denominator == 0) {
                W[i][j] = 0;
            }
            else {
                W[i][j] = A[i][j] / denominator;
            }
        }
    }

    return W;
}

/*******************************/
/* Symnmf optimization helpers */
/*******************************/

/* Calculate the squared Frobenius norm of the difference between two matrixes */
static double calc_squared_frobenius_diff(double **mat1, double **mat2, MatrixDimensions dim) {
    double diff;
    double sum;
    int i;
    int j;

    /* Compare two same-shaped matrixes using the squared Frobenius norm */
    sum = 0;
    for(i = 0; i < dim.rows; i++) {
        for(j = 0; j < dim.cols; j++) {
            diff = mat1[i][j] - mat2[i][j];
            sum += diff * diff;
        }
    }
    return sum;
}

static void copy_matrix(double **source, double **target, MatrixDimensions dim) {
    int i;
    int j;
    for(i = 0; i < dim.rows; i++) {
        for(j = 0; j < dim.cols; j++) {
            target[i][j] = source[i][j];
        }
    }
}

/* Optimize H using the symnmf multiplicative update rule */
double **optimize_symnmf(double **h, double **w, int n, int k, int max_iter, double epsilon) {
    double **prev_h;
    double **next_h;
    double *ht_h;
    double *wh;
    double *h_ht_h;
    double numerator;
    double denominator;
    MatrixDimensions h_dim;
    int iter;
    int i;
    int j;
    int l;
    int m;

    h_dim.rows = n;
    h_dim.cols = k;
    prev_h = create_zero_mat(n, k);
    next_h = create_zero_mat(n, k);
    ht_h = (double*)calloc(k * k, sizeof(double));
    wh = (double*)calloc(n * k, sizeof(double));
    h_ht_h = (double*)calloc(n * k, sizeof(double));
    if(prev_h == NULL || next_h == NULL || ht_h == NULL || wh == NULL || h_ht_h == NULL) {
        free(ht_h);
        free(wh);
        free(h_ht_h);
        return NULL;
    }

    copy_matrix(h, prev_h, h_dim);
    for(iter = 0; iter < max_iter; iter++) {
        /* Pre-compute H^T * H once for the whole iteration */
        for(i = 0; i < k; i++) {
            for(j = 0; j < k; j++) {
                ht_h[i * k + j] = 0;
                for(l = 0; l < n; l++) {
                    ht_h[i * k + j] += prev_h[l][i] * prev_h[l][j];
                }
            }
        }

        /* Pre-compute W * H so each update can reuse the same numerators */
        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                wh[i * k + j] = 0;
                for(l = 0; l < n; l++) {
                    wh[i * k + j] += w[i][l] * prev_h[l][j];
                }
            }
        }

        /* Precompute H * (H^T * H) which supplies the denominator matrix */
        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                h_ht_h[i * k + j] = 0;
                for(m = 0; m < k; m++) {
                    h_ht_h[i * k + j] += prev_h[i][m] * ht_h[m * k + j];
                }
            }
        }

        for(i = 0; i < n; i++) {
            for(j = 0; j < k; j++) {
                numerator = wh[i * k + j];
                /* Add a small constant to avoid division by zero in extreme cases */
                denominator = h_ht_h[i * k + j] + 1e-6;
                next_h[i][j] = prev_h[i][j] * (0.5 + 0.5 * (numerator / denominator));
            }
        }

        /* Stop once the update changes H by less than epsilon. */
        if(calc_squared_frobenius_diff(next_h, prev_h, h_dim) < epsilon) {
            copy_matrix(next_h, prev_h, h_dim);
            break;
        }

        /* Otherwise, use the new values as the next iteration's input */
        copy_matrix(next_h, prev_h, h_dim);
    }

    free(ht_h);
    free(wh);
    free(h_ht_h);
    return prev_h;
}
