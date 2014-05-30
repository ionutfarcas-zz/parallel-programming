#include "pi_series.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/*
	argument for each created thread;
	start_index - lower index for fractional summation, for each thread
	end_index - higher index for fractional summation, for each thread
	temp_sum - each thread will compute a partial sum of PI series	
*/
struct pthread_args
{
	unsigned long start_index;
	unsigned long end_index;
	double temp_sum;
};

/*
	thread kernel
	each thread will compute a partial sum of PI series
*/
void *pi_series_kernel(void *ptr)
{	
	unsigned long start = ((struct pthread_args*)ptr)->start_index;
	unsigned long end = ((struct pthread_args*)ptr)->end_index;

	double local_sum = 0.0;
	
	for(unsigned long k = start ; k <= end ; k++)
	{
		local_sum += pow(-1, k)/(2*k + 1);
	}
		
	((struct pthread_args*)ptr)->temp_sum = local_sum;
	
	return NULL;	
}

/*
	function that gathers all the threads and puts together each thread computed result
	compute PI using the truncated Leibniz-Gregory series
*/

double pi_series(long num_terms, long num_threads)
{
	/*
		allocate memory for each thread and its argument
	*/
	pthread_t *thread = thread = malloc((long)num_threads * sizeof(*thread));
	struct pthread_args *thread_arg = malloc((long)num_threads * sizeof(*thread_arg));

	double sum = 0.0;
	
	/*
		ratio that will determine the amount of work for each thread
	*/

	double ratio = (double)num_terms/num_threads;

	/*
		create the lower index and upper index for each thread + create threads
	*/
	for (int i = 0; i < num_threads; i++)
	{	
		
		thread_arg[i].start_index = i * ratio;
		thread_arg[i].end_index = ((i + 1) * ratio) - 1;
		pthread_create(thread + i, NULL, &pi_series_kernel, thread_arg + i);
	}

	/*
		join threads and sum the partial results
	*/

	for (int i = 0; i < num_threads; i++)
	{
		pthread_join(thread[i], NULL );
		sum += thread_arg[i].temp_sum;
	}

	/*
		return the final result
	*/	
	
	free(thread);
	free(thread_arg);
	return 4*sum;
}
