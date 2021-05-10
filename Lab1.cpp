#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "mpich/mpi.h"

using namespace std;

namespace {

int my_id = 0;
int num_procs = 1;
const int root_process = 0;
const int send_data_tag = 2001;
const int return_data_tag = 2002;
short ierr = 0;
const double X = 1.0d;
const double T = 1.0d;

/**initial conditions**/
double fi(double x)
{
	return cos(M_PI * x);
}

double psi(double t)
{
	return exp(t);
}

/**right hand sight of equation**/
double ftx(double t, double x)
{
	return 2 * (x + t);
}

void Evaluate(vector<double>& utx, const long M, const long K)
{
	MPI_Status status;
    double h = X / M;
    double tau = T / K;
    long steps_per_process = K / num_procs + 1;

    for (long m = 0; m < M; ++m)  // x = m * h
    {
        long k;
        long steps_passed = 0;
        for(k = my_id * steps_per_process; k < K; ++k)  // t = k * tau
        {
            if (steps_passed >= steps_per_process)
                break;
            else if (m == 0)
                utx[k] = psi(k * tau);
            else if (k == 0)
                utx[K * m] = fi(m * h);
            else if (m == 1 || k == K - 1)
                utx[K * m + k] += (ftx(k * tau, (m - 1) * h) - (utx[K * (m - 1) + k]- utx[K * (m - 1) + k - 1]) / tau) * h;
            else
                utx[K * m + k] = (ftx(k * tau, (m - 1) * h) - (utx[K * (m - 1) + k + 1] - utx[K * (m - 1) + k - 1]) / (2.0 * tau)) * 2.0 * h + utx[K * (m - 1) + k];
            ++steps_passed;
        }

        long id;
        if (my_id == root_process)
        {
            for(id = 1; id < num_procs; ++id)
            {
                ierr += MPI_Recv(&utx[m * K + id * steps_per_process], steps_per_process, MPI_DOUBLE, id, send_data_tag, MPI_COMM_WORLD, &status);
                ierr += MPI_Send(&utx[m * K + id * steps_per_process - 1], 1, MPI_DOUBLE, id, send_data_tag, MPI_COMM_WORLD);
            }

            for(id = 1; id < num_procs; ++id)
            {
                long k = (id + 1) * steps_per_process;
                ierr += (k < K) ? MPI_Send(&utx[m * K + k], 1, MPI_DOUBLE, id, return_data_tag, MPI_COMM_WORLD) : 0;
            }
        }
        else
        {
            ierr += MPI_Send(&utx[m * K + my_id * steps_per_process], steps_passed, MPI_DOUBLE, root_process, send_data_tag, MPI_COMM_WORLD);
            ierr += MPI_Recv(&utx[m * K + my_id * steps_per_process - 1], 1, MPI_DOUBLE, root_process, send_data_tag, MPI_COMM_WORLD, &status);

            long k = (my_id + 1) * steps_per_process;
            ierr += (k < K) ? MPI_Recv(&utx[m * K + k], 1, MPI_DOUBLE, root_process, return_data_tag, MPI_COMM_WORLD, &status) : 0;
        }
	}
}

}  // anonymous namespace


int main(int argc, char **argv)
{
    // default max number of steps
    long M = 4000;  // X = M*h
    long K = 6000;  // T = K*tau
    if (my_id == root_process)
    {
        if (argc == 3)
        {
            M = stol(argv[1]);
            K = stol(argv[2]);
        }
    }
	ierr += MPI_Init(&argc, &argv);

	ierr += MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr += MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	vector<double> utx(M * K);

	clock_t currentTime = clock();
	Evaluate(utx, M, K);
	currentTime = clock() - currentTime;
	long double res = (long double) currentTime / CLOCKS_PER_SEC;

	if (my_id == root_process)
		cout << "time = " << setprecision(10) << res << endl;

//    if (my_id == root_process) {
//        for(auto&& d : utx) {
//            cout << d << " ";
//        }
//        cout << endl;
//
//	}
	ierr += MPI_Finalize();
	return ierr;
}
