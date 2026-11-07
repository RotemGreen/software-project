# Variables
CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LDLIBS = -lm
OBJ = symnmf.o symnmf_algo.o symnmf_io.o matrix_management.o

# Default target
symnmf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o symnmf $(LDLIBS)

symnmf.o: symnmf.c symnmf_algo.h symnmf_io.h

symnmf_algo.o: symnmf_algo.c symnmf_algo.h matrix_management.h

symnmf_io.o: symnmf_io.c symnmf_io.h matrix_management.h

matrix_management.o: matrix_management.c matrix_management.h

# Clean target
clean:
	rm -f symnmf $(OBJ)
