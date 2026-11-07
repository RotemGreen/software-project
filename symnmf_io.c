#include <stdio.h>
#include "matrix_management.h"
#include "symnmf_io.h"

/************************/
/* Input/output helpers */
/************************/

/* Given a file_name of a csv file that represents a matrix, 
   calculate the matrixes' number of columns and rows */
MatrixDimensions get_dimensions(char *file_name) {
    int rows;
    int cols;
    int iter;
    int ch;
    int is_curr_c_backslashN;
    FILE *f;
    MatrixDimensions m;
    rows = 0;
    cols = 1;
    iter = 0;
    is_curr_c_backslashN = 0;
    f = fopen(file_name, "r");
    if (f == NULL) {
        m.rows = 0;
        m.cols = 0;
        return m;
    }
    while ((ch = fgetc(f)) != EOF) {
        iter++;
        if (rows == 0) {
            if (ch == ',') {
                cols++;
            }
        }
        if (ch == '\n') {
            rows++;
            is_curr_c_backslashN = 1;
        }
        else {
            is_curr_c_backslashN = 0;
        }
    }
    if (iter == 0) {
        m.rows = 0;
        m.cols = 0;
        fclose(f);
        return m;
    }

    /* Edge-case: if the file ends with EOF without \n beforehand*/
    if (is_curr_c_backslashN == 0) {
        rows++;
    }
    m.rows = rows;
    m.cols = cols;
    fclose(f);
    return m;
}

/* Given a csv file, process the file and return its content as a matrix */
double **process_input(MatrixDimensions m, char *file_name) {
    int i;
    int j;
    double **mat;
    FILE *f;
    mat = create_zero_mat(m.rows, m.cols);
    if(mat == NULL) {
        return NULL;
    }
    f = fopen(file_name, "r");
    if(f == NULL) {
        return NULL;
    }

    for(i = 0; i < m.rows; i++) {
        for(j = 0; j < m.cols; j++) {
            if (fscanf(f, "%lf,", &mat[i][j]) != 1) {
                fclose(f);
                return NULL;
            }
        }
    }
    fclose(f);
    return mat;
}

/* Print the given matrix */
void print_mat(double **mat, MatrixDimensions m) {
    int i;
    int j;
    for(i = 0; i < m.rows; i++) {
        for(j = 0; j < m.cols; j++) {
            if (j == m.cols -1) {
                printf("%.4f", mat[i][j]);
            }
            else {
                printf("%.4f,", mat[i][j]);
            }
        }
        printf("\n");
    }
}
