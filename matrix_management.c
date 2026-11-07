#include <stdlib.h>
#include "matrix_management.h"

/* All the memory allocations of the matrixes is done in a central way:
after each memory allocation of a matrix, it is saved in a static array.
While releasing the memory, the program goes over the array and releases each matrix */

#define MAX_MATRIX_COUNT 50
static double **allocated_matrixes[MAX_MATRIX_COUNT];
static int allocated_matrixes_rows[MAX_MATRIX_COUNT];
static int matrixes_count = 0;

/********************/
/* Matrix utilities */
/********************/

void free_mat(double **mat, int rows) {
    int i;
    for(i = 0; i < rows; i++) {
        free(mat[i]);
    }
    free(mat);
}

void free_allocated_matrixes(void) {
    int i;
    for(i = 0; i < matrixes_count; i++) {
        free_mat(allocated_matrixes[i], allocated_matrixes_rows[i]);
    }
    matrixes_count = 0;
}

/* Create and allocate memory for a zero matrix */
double **create_zero_mat(int rows, int cols) {
    double **mat;
    int i;
    mat = (double**)calloc(rows, sizeof(double*));
    if(mat == NULL) {
        return NULL;
    }
    for(i = 0; i < rows; i++) {
        mat[i] = (double*)calloc(cols, sizeof(double));
        if(mat[i] == NULL) {
            while(i > 0) {
                i--;
                free(mat[i]);
            }
            free(mat);
            return NULL;
        }
    }

    if(matrixes_count >= MAX_MATRIX_COUNT) {
        free_mat(mat, rows);
        return NULL;
    }
    allocated_matrixes[matrixes_count] = mat;
    allocated_matrixes_rows[matrixes_count++] = rows;
    return mat;
}
