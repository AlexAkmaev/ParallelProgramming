#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpich/mpi.h"

#define ISIZE 100
#define JSIZE 100

int my_id = 0;
int num_procs = 1;
const int root_process = 0;
short ierr = 0;

double *a;  //  [ISIZE] * [JSIZE]
double *b;  //  [ISIZE] * [JSIZE]

int main(int argc, char **argv) {
    ierr += MPI_Init(&argc, &argv);

    ierr += MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr += MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // elems per one process
    int rows_per_proc = (ISIZE - 1) / num_procs;
    int data_per_proc = rows_per_proc * JSIZE;
    a = (double *) calloc(data_per_proc, sizeof(double));
    b = (double *) calloc(data_per_proc, sizeof(double));

    int i, j;
    // parallel for outer idx
    for (i = 0; i < rows_per_proc; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i * JSIZE + j] = 10 * (rows_per_proc * my_id + i + 1) + j;
        }
    }

    for (i = 0; i < rows_per_proc; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i * JSIZE + j] = sin(0.00001 * a[i * JSIZE + j]);
        }
    }

    for (i = 0; i < rows_per_proc; i++) {
        for (j = 0; j < JSIZE; j++) {
            b[i * JSIZE + j] = a[i * JSIZE + j] * 1.5;
        }
    }

    double *data = NULL;
    if (my_id == root_process) {
        data = (double *) calloc(ISIZE * JSIZE, sizeof(double));
    }

    ierr += MPI_Gather(b, data_per_proc, MPI_DOUBLE, data, data_per_proc, MPI_DOUBLE, root_process, MPI_COMM_WORLD);
    free(a);
    free(b);

    // process the remainder separately, if needed
    if (rows_per_proc * num_procs != ISIZE - 1 && my_id == root_process) {
        int remainder = (ISIZE - 1) % num_procs;
        double *a_remain = (double *) calloc(remainder * JSIZE, sizeof(double));
        for (i = 0; i < remainder; i++) {
            for (j = 0; j < JSIZE; j++) {
                a_remain[i * JSIZE + j] = 10 * (rows_per_proc * num_procs + i + 1) + j;
            }
        }

        for (i = 0; i < remainder; i++) {
            for (j = 0; j < JSIZE; j++) {
                a_remain[i * JSIZE + j] = sin(0.00001 * a_remain[i * JSIZE + j]);
            }
        }

        for (i = 0; i < remainder; i++) {
            for (j = 0; j < JSIZE; j++) {
                data[(rows_per_proc * num_procs + i) * JSIZE + j] = a_remain[i * JSIZE + j] * 1.5;
            }
        }
        free(a_remain);
    }

    if (my_id == root_process) {
        FILE *ff = fopen("res_parallel.txt","w");
        for (i = 0; i < ISIZE; i++) {
            for (j = 0; j < JSIZE; j++) {
                fprintf(ff,"%f ",data[i * JSIZE + j]);
            }
            fprintf(ff,"\n");
        }
        fclose(ff);
        free(data);
    }

    ierr += MPI_Finalize();

    return ierr;
}
