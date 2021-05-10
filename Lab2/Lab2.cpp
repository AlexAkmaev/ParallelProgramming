#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <pthread.h>

using namespace std;

namespace {

const double lhs = 0.001d;
const double rhs = 1.0d;
double precision = 0.001d;  // default precision
#define NUM_THREADS 4

pthread_mutex_t lock_res;

typedef struct _thread_data_t
{
	double lhs;
	double rhs;
	int steps_num;
	double* part_I;
} thread_data_t;

typedef double(*pointFunc)(double);

double f(double x)
{
	return sin(1.0 / x);
}

void* simpson_integral(void* arg)
{
	thread_data_t& data = *((thread_data_t*) arg);

    double h = (data.rhs - data.lhs + 0.0d) / data.steps_num;
	double res = 0.0, a = 0.0, b = h;
    for(int i = 0; i < data.steps_num; ++i)
    {
        a = data.lhs + i * h;
        b = a + h;
        res += h * (f(a) + 4.0 * f((a + b) / 2.0) + f(b)) / 6.0;
    }

    pthread_mutex_lock(&lock_res);
    *data.part_I += res;
    pthread_mutex_unlock(&lock_res);
    pthread_exit(NULL);

    return NULL;
}

double integral(double lhs, double rhs)
{
	pthread_t thr[NUM_THREADS];
	int rc = 0;
	thread_data_t thr_data[NUM_THREADS];

    double I = 0.0, I1 = I;

    pthread_mutex_init(&lock_res, NULL);
    double segment_per_thread = (rhs - lhs) / NUM_THREADS;

    int steps_num;
    for (steps_num = 1; steps_num <= 2 || fabs(I - I1) > precision; steps_num *= 2)
    {
    	I1 = I;
    	I = precision;

        int need_threads = min(steps_num, NUM_THREADS);

    	for (int i = 0; i < need_threads; ++i)
        {
    		thr_data[i].lhs = i * segment_per_thread + lhs;
            thr_data[i].rhs = (i + 1) * segment_per_thread + lhs;
            thr_data[i].steps_num = steps_num / need_threads;
            thr_data[i].part_I = &I;

            if (rc = pthread_create(&thr[i], NULL, simpson_integral, &thr_data[i]))
            {
                cerr << "Error: pthread_create, rc:" << rc << endl;
                return EXIT_FAILURE;
            }
        }

        for (int i = 0; i < need_threads; ++i)
        	pthread_join(thr[i], NULL);
    }
    cout << "Total steps: " << steps_num / 2 << endl;
    return I;
}

}  // anonymous namespace

int main(int argc, char** argv)
{
	if (argc >= 3)
    {
        cerr << "Error: wrong number of arguments\n";
        return EXIT_FAILURE;
    }

	if (argc == 2)
		precision = stod(argv[1]);

	double I = integral(lhs, rhs);

	cout << setprecision(string(argv[1]).length() + 2);
	cout << "∫sin(1/x)dx on [" << lhs << ", " << rhs << "] = " << I <<
            "\nPrecision of calculation = " << precision << endl;

    return EXIT_SUCCESS;
}
