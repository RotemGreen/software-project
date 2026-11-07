#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "matrix_management.h"
#include "symnmf_algo.h"

static PyObject* set_python_error(void) {
    PyErr_SetString(PyExc_RuntimeError, "An Error Has Occurred");
    return NULL;
}

/* Converts the input matrix's dimension into a c type */
static void convert_dim_to_c(PyObject *points, MatrixDimensions *dim) {
    PyObject *first_row;
    dim->rows = PyList_Size(points);
    if (dim->rows == 0) {
        dim->cols = 0;
    }
    else {
        first_row = PyList_GetItem(points, 0);
        dim->cols = PyList_Size(first_row);
    }
}

/* Converts the input matrix into a c type */
static double** convert_mat_to_c(PyObject *points, MatrixDimensions dim){
    double **mat;
    int i;
    int j;
    PyObject *curr_row;
    PyObject *curr_val;
    mat = create_zero_mat(dim.rows, dim.cols);
    if(mat == NULL) {
        return NULL;
    }

    for(i = 0; i < dim.rows; i++) {
        curr_row = PyList_GetItem(points, i);
        if(curr_row == NULL) {
            return NULL;
        }
        for(j = 0; j < dim.cols; j++) {
            curr_val = PyList_GetItem(curr_row, j);
            if(curr_val == NULL) {
                return NULL;
            }
            mat[i][j] = PyFloat_AsDouble(curr_val);
            if (PyErr_Occurred()) {
                return NULL;
            }
        }
    }
    return mat;
}

/* Convert the input matrix into a python type */
static PyObject* convert_matrix_to_python(double **mat, int rows, int cols) {
    int i;
    int j;
    PyObject *py_mat;
    PyObject *row;
    PyObject *py_val;

    py_mat = PyList_New(rows);
    if(!py_mat) {
        return NULL;
    }

    for(i = 0; i < rows; i++) {
        row = PyList_New(cols);
        if (!row) {
            Py_DECREF(py_mat);
            return NULL;
        }
        for(j = 0; j < cols; j++) {
            py_val = PyFloat_FromDouble(mat[i][j]);
            if (py_val == NULL) {
                Py_DECREF(row);
                Py_DECREF(py_mat);
                return NULL;
            }
            if (PyList_SetItem(row, j, py_val) != 0) {
                Py_DECREF(py_val);
                Py_DECREF(row);
                Py_DECREF(py_mat);
                return NULL;
            }
        }
        if (PyList_SetItem(py_mat, i, row) != 0) {
            Py_DECREF(row);
            Py_DECREF(py_mat);
            return NULL;
        }
    }
    return py_mat;
}

/* Calculate the similarity matrix in c from PyObject containing the points matrix */
static double** sym_c_logic(PyObject *points, MatrixDimensions *matrixes_dim) {
    MatrixDimensions points_dim;
    double **points_c;
    double **sym_mat;

    convert_dim_to_c(points, &points_dim);
    matrixes_dim->rows = points_dim.rows;
    matrixes_dim->cols = points_dim.rows;
    if(points_dim.rows == 0) {
        return NULL;
    }
    points_c = convert_mat_to_c(points, points_dim);
    if(points_c == NULL) {
        return NULL;
    }
    sym_mat = calc_similarity_mat(points_c, *matrixes_dim, points_dim.cols);
    return sym_mat;
}


/* The main function exposed to python that calculates the similarity matrix */
static PyObject* sym(PyObject *self, PyObject *args) {
    PyObject* points;
    MatrixDimensions sym_dim;
    PyObject *py_sym_mat;
    double **sym_mat;


    if(!PyArg_ParseTuple(args, "O", &points)) {
        return NULL; 
    }

    sym_mat = sym_c_logic(points, &sym_dim);
    if (sym_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }
    py_sym_mat = convert_matrix_to_python(sym_mat, sym_dim.rows, sym_dim.cols);
    free_allocated_matrixes();
    if (py_sym_mat == NULL) {
        return set_python_error();
    }
    
    return py_sym_mat;
}

/* The main function exposed to python that calculates the diagonal matrix */
static PyObject* ddg(PyObject *self, PyObject *args) {
    PyObject* points;
    MatrixDimensions matrixes_dim;
    double **sym_mat;
    double **diag_mat;
    PyObject* py_diag_mat;

    if(!PyArg_ParseTuple(args, "O", &points)) {
        return NULL; 
    }

    sym_mat = sym_c_logic(points, &matrixes_dim);
    if (sym_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }
    diag_mat = calc_diagonal_mat(sym_mat, matrixes_dim);
    if (diag_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }

    py_diag_mat = convert_matrix_to_python(diag_mat, matrixes_dim.rows, matrixes_dim.cols);
    free_allocated_matrixes();
    if (py_diag_mat == NULL) {
        return set_python_error();
    }
    return py_diag_mat;
}

/*The main function exposed to python that calculates the normalized matrix */
static PyObject* norm(PyObject *self, PyObject *args) {
    PyObject* points;
    MatrixDimensions matrixes_dim;
    double **sym_mat;
    double **diag_mat;
    double **norm_mat;
    PyObject* py_norm_mat;

    if(!PyArg_ParseTuple(args, "O", &points)) {
        return NULL; 
    }

    sym_mat = sym_c_logic(points, &matrixes_dim);
    if (sym_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }
    diag_mat = calc_diagonal_mat(sym_mat, matrixes_dim);
    if (diag_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }
    norm_mat = calc_normalized_matrix(diag_mat, sym_mat, matrixes_dim); 
    if (norm_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }
    py_norm_mat = convert_matrix_to_python(norm_mat, matrixes_dim.rows, matrixes_dim.cols);
    free_allocated_matrixes();
    if (py_norm_mat == NULL) {
        return set_python_error();
    }
    return py_norm_mat;
}

static PyObject* py_symnmf(PyObject *self, PyObject *args) {
    PyObject *h_obj;
    PyObject *w_obj;
    MatrixDimensions h_dim;
    MatrixDimensions w_dim;
    double **h_mat;
    double **w_mat;
    double **result;
    PyObject *py_result;
    int max_iter;
    double epsilon;

    if(!PyArg_ParseTuple(args, "OOid", &h_obj, &w_obj, &max_iter, &epsilon)) {
        return NULL;
    }

    convert_dim_to_c(h_obj, &h_dim);
    convert_dim_to_c(w_obj, &w_dim);
    if(h_dim.rows == 0 || h_dim.cols == 0 || w_dim.rows != w_dim.cols || h_dim.rows != w_dim.rows) {
        return set_python_error();
    }

    h_mat = convert_mat_to_c(h_obj, h_dim);
    w_mat = convert_mat_to_c(w_obj, w_dim);
    if(h_mat == NULL || w_mat == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }

    result = optimize_symnmf(h_mat, w_mat, h_dim.rows, h_dim.cols, max_iter, epsilon);
    if(result == NULL) {
        free_allocated_matrixes();
        return set_python_error();
    }

    py_result = convert_matrix_to_python(result, h_dim.rows, h_dim.cols);
    free_allocated_matrixes();
    if (py_result == NULL) {
        return set_python_error();
    }
    return py_result;
}


/* Func array */
static PyMethodDef symnmfMethods[] = {
    {"symnmf",
      (PyCFunction) py_symnmf,
      METH_VARARGS,
      PyDoc_STR("symnmf(h, w, max_iter, epsilon)\n"
                "--\n"
                "\n"
                "Optimize the decomposition matrix H\n"
                "\n"
                "Args:\n"
                "    h (list): Initial decomposition matrix represented as a list of lists of floats.\n"
                "    w (list): Normalized similarity matrix represented as a list of lists of floats.\n"
                "    max_iter (int): Maximum number of update iterations.\n"
                "    epsilon (float): Convergence threshold for the squared Frobenius norm.\n"
                "\n"
                "Returns:\n"
                "    list: A list of lists of floats that represent the optimized decomposition matrix.")},
    {"sym",                   /* the Python method name that will be used */
      (PyCFunction) sym, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */
      PyDoc_STR("sym(points)\n" 
                "--\n"                                              
                "\n"
                "Calculate the symetric matrix\n"
                "\n"
                "Args:\n"
                "    points (list): Input list of data points which are arrays of coordinates (float).\n"
                "\n"
                "Returns:\n"
                "    list: A list of lists of floats that represent the adjacency matrix.")}, /*  The docstring for the function */ 

    {"ddg",                   /* the Python method name that will be used */
      (PyCFunction) ddg, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */
      PyDoc_STR("ddg(points)\n" 
                "--\n"                                              
                "\n"
                "Calculate the diagonal matrix\n"
                "\n"
                "Args:\n"
                "    points (list): Input list of data points which are arrays of coordinates (float).\n"
                "\n"
                "Returns:\n"
                "    list: A list of lists of floats that represent the diagonal matrix.")}, /*  The docstring for the function */ 

    {"norm",                   /* the Python method name that will be used */
      (PyCFunction) norm, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */
      PyDoc_STR("norm(points)\n" 
                "--\n"                                              
                "\n"
                "Calculate the norm matrix\n"
                "\n"
                "Args:\n"
                "    points (list): Input list of data points which are arrays of coordinates (float).\n"
                "\n"
                "Returns:\n"
                "    list: A list of lists of floats that represent the norm matrix.")}, /*  The docstring for the function */ 

    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */                          
};


/* Module build */
static struct PyModuleDef symnmfmodule_definition = {
    PyModuleDef_HEAD_INIT,
    "symnmfmodule", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    symnmfMethods /* the PyMethodDef array from before containing the methods of the extension */
};


PyMODINIT_FUNC PyInit_symnmfmodule(void)
{
    PyObject *m;
    m = PyModule_Create(&symnmfmodule_definition);
    if (!m) {
        return NULL;
    }
    return m;
}
