#include "pi_series.h"
#include <math.h>

/*
	compute PI using Leibniz-Gregory truncated series
	sequential implementation of the code
*/
double pi_series(long num_terms, long num_threads)
{
	double sum = 0.0;

	for (unsigned long k = 0; k <= num_terms; k++)
	{
		sum += pow(-1, k)/(2*k + 1);
	}

	return 4 * sum;
}
