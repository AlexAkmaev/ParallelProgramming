#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

namespace {

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
    double h = X / M;
    double tau = T / K;

    for (long m = 0; m < M; ++m)  // x = m * h
    {
        long k;
        long steps_passed = 0;
        for(k = 0; k < K; ++k)  // t = k * tau
        {
            if (m == 0)
                utx[k] = psi(k * tau);
            else if (k == 0)
                utx[K * m] = fi(m * h);
            else if (m == 1 || k == K - 1)
                utx[K * m + k] += (ftx(k * tau, (m - 1) * h) - (utx[K * (m - 1) + k]- utx[K * (m - 1) + k - 1]) / tau) * h;
            else
                utx[K * m + k] = (ftx(k * tau, (m - 1) * h) - (utx[K * (m - 1) + k + 1] - utx[K * (m - 1) + k - 1]) / (2.0 * tau)) * 2.0 * h + utx[K * (m - 1) + k];
        }
    }
}

}  // anonymous namespace


int main(int argc, char **argv)
{
    // default max number of steps
    long M = 4000;  // X = M*h
    long K = 6000;  // T = K*tau
    if (argc == 3)
    {
        M = stol(argv[1]);
        K = stol(argv[2]);
    }

	vector<double> utx(M * K);

	Evaluate(utx, M, K);

    return 0;
}
