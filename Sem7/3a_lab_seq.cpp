#include <cstdio>
#include <cmath>

#define ISIZE 100
#define JSIZE 100

double a[ISIZE][JSIZE], b[ISIZE][JSIZE];

int main(int argc, char **argv) {
    int i, j;
    FILE *ff;

    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = 10 * i + j;
            b[i][j] = 0.;
        }
    }

    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = sin(0.00001 * a[i][j]);
        }
    }

    for (i = 0; i < ISIZE - 1; i++) {
        for (j = 0; j < JSIZE; j++) {
            b[i][j] = a[i + 1][j] * 1.5;
        }
    }

    ff = fopen("res_seq.txt","w");
    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            fprintf(ff,"%f ",b[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);
}
