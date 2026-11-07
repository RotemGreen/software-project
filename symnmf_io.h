#ifndef SYMNMF_IO_H
#define SYMNMF_IO_H

#include "symnmf.h"

MatrixDimensions get_dimensions(char *file_name);
double **process_input(MatrixDimensions m, char *file_name);
void print_mat(double **mat, MatrixDimensions m);

#endif
