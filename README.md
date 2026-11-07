# SymNMF Clustering (C + Python)

Implementation of **Symmetric Non-negative Matrix Factorization (SymNMF)** clustering, compared against **K-means** using the silhouette score.
Built for the *Software Project* course.

The core algorithm (similarity matrix, degree matrix, normalized similarity, and the iterative update of H) is written in **C** and exposed to **Python** through a custom **C extension (Python C API)**. The Python layer handles I/O, the random initialization of H (NumPy), and the analysis against K-means (scikit-learn).

## Pipeline
1. Similarity matrix `A` (Gaussian kernel on pairwise squared distances)
2. Diagonal degree matrix `D`
3. Normalized similarity `W = D^-1/2 A D^-1/2`
4. Optimize `H >= 0` minimizing `||W - HH^T||_F^2` with multiplicative updates (beta = 0.5, eps = 1e-4, max 300 iterations)
5. Hard clustering: each point goes to the cluster with the highest score in its row of `H`

## Files
| File | Purpose |
|---|---|
| `symnmf.py` | Python CLI: `sym`, `ddg`, `norm`, `symnmf` goals; initializes H and calls the C module |
| `symnmfmodule.c` | Python C API wrapper |
| `symnmf.c`, `symnmf_algo.c/.h`, `symnmf_io.c/.h`, `matrix_management.c/.h`, `symnmf.h` | C implementation (standalone executable for `sym`/`ddg`/`norm`) |
| `kmeans.py` | K-means (from an earlier assignment), used as baseline |
| `analysis.py` | Compares SymNMF vs K-means with `sklearn.metrics.silhouette_score` |
| `io_utils.py` | Input parsing helpers |
| `setup.py`, `Makefile` | Build the C extension / the C executable |

## Build & run
```bash
# C executable (compiled with -ansi -Wall -Wextra -Werror -pedantic-errors)
make
./symnmf sym data.txt        # also: ddg, norm

# Python C extension
python3 setup.py build_ext --inplace
python3 symnmf.py <k> <goal> data.txt     # goal: symnmf | sym | ddg | norm
python3 analysis.py <k> data.txt          # prints silhouette scores
```
Input: a `.txt` file with one comma-separated data point per line. Output matrices are printed with 4 decimal places.

Requires: Python 3, NumPy, scikit-learn, gcc (and Python dev headers).
