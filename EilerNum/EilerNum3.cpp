#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace boost::multiprecision;

const int precision = 1000;

int1024_t fact(int n) {
	int1024_t res = 1ull;
	for (int i = 1; i <= n; ++i) {
        res *= i;
    }

	return res;
}

int main(int argc, char **argv) {
	clock_t currentTime = clock();

	number<cpp_dec_float<precision>> sum = 1;
	number<cpp_dec_float<precision>> one = 1;
	for (int i = 1; i < 1000; ++i) {
		sum += one / number<cpp_dec_float<precision>>(fact(i));
	}

	currentTime = clock() - currentTime;
	long double res = (long double) currentTime / CLOCKS_PER_SEC;

	std::cout << "e = " << std::setprecision(precision) << sum << "\n";
	std::cout << "time = " << std::setprecision(10) << res << "\n";  // ~ 0.195 sec

    return 0;
}
