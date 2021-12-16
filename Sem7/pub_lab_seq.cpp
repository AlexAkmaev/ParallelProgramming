#include <cstdio>
#include <cmath>

#define ISIZE 1000
#define JSIZE 1000

int main(int argc, char **argv) {
    auto **a = (double **)calloc(ISIZE, sizeof(double*));
    for (int i = 0; i < ISIZE; ++i) {
        a[i] = (double *)calloc(JSIZE, sizeof(double));
    }

    int i, j;
    FILE *ff;

    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = 10 * i + j;
        }
    }

    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = sin(0.00001 * a[i][j]);
        }
    }

    ff = fopen("result.txt","w");
    for(i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            fprintf(ff,"%f ",a[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);

    return 0;
}
