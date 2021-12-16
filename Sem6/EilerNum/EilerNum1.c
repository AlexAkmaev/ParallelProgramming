#include <stdio.h>
#include <time.h>

unsigned long long fact(int n) {
    	int i = 1;
	unsigned long long res = 1;
	for (i = 1; i <= n; ++i) {
            res *= i;
        }
	return res;
}

int main(int argc, char **argv) {
	int i = 0;
	long double sum = 1;
	clock_t currentTime = clock();
	for (i = 1; i < 65; ++i) {
		sum += 1.0 / fact(i);
	}
	currentTime = clock() - currentTime;
	long double res = (long double) currentTime / CLOCKS_PER_SEC;
	printf("e = %.15Lf\n%.7Lf\n", sum, res);
	return 0;
}
