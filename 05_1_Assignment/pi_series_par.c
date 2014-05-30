#include "pi_series.h"
#include <math.h>
#include <omp.h>

/*
	compute PI using Leibniz-Gregory truncated series
	sequential implementation of the code
*/
double pi_series(long num_terms, long num_threads)
{
	double sum = 0.0;

	// set no. of threads
	omp_set_num_threads(num_threads);

	#pragma omp parallel for reduction(+: sum)
	for (unsigned long k = 0; k < num_terms; k++)
	{
		sum += ((1.0 - 2*(k & 1)) / (2*k + 1));
	}

	return 4.0*sum;
}
