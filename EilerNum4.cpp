#include <iostream>
#include <iomanip>
#include "mpich/mpi.h"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace boost::multiprecision;

const int precision = 1000;
const int num_rows = 720;
const int send_data_tag = 2001;
const int return_data_tag = 2002;

number<cpp_dec_float<precision>> array1[num_rows];
number<cpp_dec_float<precision>> array2[num_rows];

int1024_t fact(int n) {
	int1024_t res = 1ull;
	for (int i = 1; i <= n; ++i) {
        res *= i;
    }

	return res;
}

int main(int argc, char **argv) {
	number<cpp_dec_float<precision>> sum, partial_sum;
    MPI_Status status;
    int my_id, root_process, num_procs, an_id, num_rows_to_receive, avg_rows_per_process,
        sender, num_rows_received, start_row, end_row, num_rows_to_send;

    int ierr = MPI_Init(&argc, &argv);
    root_process = 0;

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(my_id == root_process) {

         clock_t currentTime = clock();

         avg_rows_per_process = num_rows / num_procs;

         /* initialize an array1 of members of the Taylor series*/

         number<cpp_dec_float<precision>> one = 1.0;
         array1[0] = 1.0;
         for(int i = 1; i < num_rows; i++) {
            array1[i] = one / number<cpp_dec_float<precision>>(fact(i));
         }

         /* distribute a portion of the bector to each child process */

         for(an_id = 1; an_id < num_procs; an_id++) {
            start_row = an_id * avg_rows_per_process + 1;
            end_row   = (an_id + 1) * avg_rows_per_process;

            if ((num_rows - end_row) < avg_rows_per_process)
               end_row = num_rows - 1;

            num_rows_to_send = end_row - start_row + 1;

            ierr = MPI_Send( &num_rows_to_send, 1, MPI_INT,
                  an_id, send_data_tag, MPI_COMM_WORLD);

            ierr = MPI_Send( &array1[start_row], num_rows_to_send*sizeof(number<cpp_dec_float<precision>>), MPI_BYTE,
                  an_id, send_data_tag, MPI_COMM_WORLD);
         }

         /* Calculate the sum of the values in the segment assigned
          * to the root process */

         sum = 0.0;
         for(int i = 0; i < avg_rows_per_process + 1; i++) {
            sum += array1[i];
         }

         /* Collect the partial sums from the slave processes, and add
          * them to the sum */

         for(an_id = 1; an_id < num_procs; an_id++) {

            ierr = MPI_Recv( &partial_sum, 1*sizeof(number<cpp_dec_float<precision>>), MPI_BYTE, MPI_ANY_SOURCE,
                  return_data_tag, MPI_COMM_WORLD, &status);

            sender = status.MPI_SOURCE;

            sum += partial_sum;
         }

         currentTime = clock() - currentTime;
         long double res = (long double) currentTime / CLOCKS_PER_SEC;

         std::cout << "e = " << std::setprecision(precision) << sum << std::endl;
         std::cout << "time = " << std::setprecision(10) << res << std::endl;

    } else {

         /* Receive array1 segment, storing it in array2. */

         ierr = MPI_Recv( &num_rows_to_receive, 1, MPI_INT,
               root_process, send_data_tag, MPI_COMM_WORLD, &status);

         ierr = MPI_Recv( &array2, num_rows_to_receive*sizeof(number<cpp_dec_float<precision>>), MPI_BYTE,
               root_process, send_data_tag, MPI_COMM_WORLD, &status);

         num_rows_received = num_rows_to_receive;

         partial_sum = 0.0;
         for(int i = 0; i < num_rows_received; i++) {
            partial_sum += array2[i];
         }

         ierr = MPI_Send( &partial_sum, 1*sizeof(number<cpp_dec_float<precision>>), MPI_BYTE, root_process,
               return_data_tag, MPI_COMM_WORLD);
    }

    ierr = MPI_Finalize();

    return 0;
}
