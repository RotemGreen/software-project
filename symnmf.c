/*read user cmd arguments: which goal is selected (there are 3), and the file name*/
/*output: the rewuired matrix*/
#include <stdio.h>
#include "symnmf_algo.h"
#include "symnmf_io.h"

/***************/
/* CLI helpers */
/***************/

static void print_error(void) {
    printf("An Error Has Occurred");
}

/* Check if 2 strings are equal */
int are_strings_equal(char *s1, char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            return 0;
        }
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

/* Returns goal index: 0 = sym, 1 = ddg, 2 = norm, -1 = invalid */
int get_goal_index(char *goal) {
    char *valid_goals[] = {"sym", "ddg", "norm"};
    int i;

    for (i = 0; i < 3; i++) {
        if (are_strings_equal(goal, valid_goals[i])) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    char *file_name;
    MatrixDimensions points_mat_dim; /* Dimensions: [matrix rows] x [matrix cols] */
    MatrixDimensions matrixes_dim; /* Dimensions: [matrix rows] x [matrix rows] */
    int d;
    double **points;
    double **similarity_mat;
    double **diagonal_mat;
    double **normalized_mat;
    char *goal;
    int goal_index;

    if(argc != 3){
        print_error();
        free_allocated_matrixes();
        return 1;
    }

    goal = argv[1];
    file_name = argv[2];

    goal_index = get_goal_index(goal);
    if (goal_index == -1) {
        print_error();
        free_allocated_matrixes();
        return 1;
    }

    points_mat_dim = get_dimensions(file_name);
    if (points_mat_dim.rows == 0 || points_mat_dim.cols == 0) {
        print_error();
        free_allocated_matrixes();
        return 1;
    }
    d = points_mat_dim.cols;

    matrixes_dim.rows = points_mat_dim.rows;
    matrixes_dim.cols = points_mat_dim.rows;

    points = process_input(points_mat_dim, file_name);
    if (points == NULL) {
        print_error();
        free_allocated_matrixes();
        return 1;
    }
    similarity_mat = calc_similarity_mat(points, matrixes_dim, d);
    if (similarity_mat == NULL) {
        print_error();
        free_allocated_matrixes();
        return 1;
    }


    /* The goal is sym */
    if (goal_index == 0) {
        print_mat(similarity_mat, matrixes_dim);
    }
    else {
        diagonal_mat = calc_diagonal_mat(similarity_mat, matrixes_dim);
        if (diagonal_mat == NULL) {
            print_error();
            free_allocated_matrixes();
            return 1;
        }
        /* The goal is ddg */
        if (goal_index == 1) {
            print_mat(diagonal_mat, matrixes_dim);
        }

        /* The goal is norm */
        else {
            normalized_mat = calc_normalized_matrix(diagonal_mat, similarity_mat, matrixes_dim);
            if (normalized_mat == NULL) {
                print_error();
                free_allocated_matrixes();
                return 1;
            }
            print_mat(normalized_mat, matrixes_dim);
        }
    }

    free_allocated_matrixes();

    return 0;
}
