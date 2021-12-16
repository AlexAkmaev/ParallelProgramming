#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpich/mpi.h"

#define ISIZE 1000
#define JSIZE 1000

int my_id = 0;
int num_procs = 1;
const int root_process = 0;
const int send_data_tag = 2001;
const int return_data_tag = 2002;
short ierr = 0;

int main(int argc, char **argv) {
    ierr += MPI_Init(&argc, &argv);

    ierr += MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr += MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // elems per one process
    int row_count = ISIZE / num_procs;
    int data_count = row_count * JSIZE;
    double *proc_buf = (double *)calloc(data_count, sizeof(double));

    int i, j;
    for (i = 0; i < row_count; i++) {
        for (j = 0; j < JSIZE; j++) {
            // recalc mult for needed result
            proc_buf[ISIZE * i + j] = 10 * (row_count * my_id + i) + j;
        }
    }

    for (i = 0; i < row_count; i++) {
        for (j = 0; j < JSIZE; j++) {
            proc_buf[ISIZE * i + j] = sin(0.00001 * proc_buf[ISIZE * i + j]);
        }
    }

    double *data = (double*)calloc(ISIZE * JSIZE, sizeof(double));
    MPI_Gather(proc_buf, data_count, MPI_DOUBLE, data, data_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // process the remainder separately, if needed
    if (row_count * num_procs != ISIZE && my_id == 0) {
        int remainder = ISIZE % num_procs;
        for (i = 0; i < remainder; i++) {
            for (j = 0; j < JSIZE; j++) {
                // recalc mult for needed result
                data[(row_count * num_procs + i) * JSIZE + j] = 10 * (row_count * num_procs + i) + j;
            }
        }

        for (i = 0; i < remainder; i++) {
            for (j = 0; j < JSIZE; j++) {
                data[(row_count * num_procs + i) * JSIZE + j] = sin(0.00001 * data[(row_count * num_procs + i) * JSIZE + j]);
            }
        }
    }
    free(proc_buf);

    if (my_id == root_process) {
        FILE *ff = fopen("result.txt","w");
        for(i = 0; i < ISIZE; i++) {
            for (j = 0; j < JSIZE; j++) {
                fprintf(ff, "%f ", data[i * ISIZE + j]);
            }

            fprintf(ff, "\n");
        }
        fclose(ff);
    }

    free(data);
    MPI_Finalize();

    return ierr;
}
